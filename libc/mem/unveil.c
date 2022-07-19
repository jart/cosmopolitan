/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/calls/internal.h"
#include "libc/calls/landlock.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/bpf.h"
#include "libc/calls/struct/filter.h"
#include "libc/calls/struct/seccomp.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/errno.h"
#include "libc/intrin/kprintf.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/runtime/internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/audit.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/nrlinux.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/pr.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"

#define OFF(f) offsetof(struct seccomp_data, f)

#define UNVEIL_READ                                             \
  (LANDLOCK_ACCESS_FS_READ_FILE | LANDLOCK_ACCESS_FS_READ_DIR | \
   LANDLOCK_ACCESS_FS_REFER)
#define UNVEIL_WRITE (LANDLOCK_ACCESS_FS_WRITE_FILE)
#define UNVEIL_EXEC  (LANDLOCK_ACCESS_FS_EXECUTE)
#define UNVEIL_CREATE                                             \
  (LANDLOCK_ACCESS_FS_MAKE_CHAR | LANDLOCK_ACCESS_FS_MAKE_DIR |   \
   LANDLOCK_ACCESS_FS_MAKE_REG | LANDLOCK_ACCESS_FS_MAKE_SOCK |   \
   LANDLOCK_ACCESS_FS_MAKE_FIFO | LANDLOCK_ACCESS_FS_MAKE_BLOCK | \
   LANDLOCK_ACCESS_FS_MAKE_SYM)

#define FILE_BITS                                                 \
  (LANDLOCK_ACCESS_FS_READ_FILE | LANDLOCK_ACCESS_FS_WRITE_FILE | \
   LANDLOCK_ACCESS_FS_EXECUTE)

static const struct sock_filter kBlacklistLandlock[] = {
    // clang-format off
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(arch)),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, AUDIT_ARCH_X86_64, 1, 0),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS),
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_landlock_create_ruleset, 2, 0),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_landlock_add_rule,       1, 0),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_landlock_restrict_self,  0, 1),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | (1 & SECCOMP_RET_DATA)),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
    // clang-format on
};

/**
 * Long living state for landlock calls.
 * fs_mask is set to use all the access rights from the latest landlock ABI.
 * On init, the current supported abi is checked and unavailable rights are
 * masked off.
 *
 * As of 5.19, the latest abi is v2.
 *
 * TODO:
 *  - Integrate with pledge and remove the file access?
 *  - Stuff state into the .protected section?
 */
_Thread_local static struct {
  uint64_t fs_mask;
  int fd;
} State = {
    .fs_mask = UNVEIL_READ | UNVEIL_WRITE | UNVEIL_EXEC | UNVEIL_CREATE,
    .fd = 0,
};

static int unveil_final(void) {
  int rc;
  struct sock_fprog sandbox = {
      .filter = kBlacklistLandlock,
      .len = ARRAYLEN(kBlacklistLandlock),
  };
  if ((rc = prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) != -1 &&
      (rc = landlock_restrict_self(State.fd, 0)) != -1 &&
      (rc = sys_close(State.fd)) != -1 &&
      (rc = prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &sandbox)) != -1) {
    State.fd = 0;
  }
  return rc;
}

static int err_close(int rc, int fd) {
  int serrno = errno;
  sys_close(fd);
  errno = serrno;
  return rc;
}

static int unveil_init(void) {
  int rc, fd;
  if ((rc = landlock_create_ruleset(0, 0, LANDLOCK_CREATE_RULESET_VERSION)) <
      0) {
    if (errno == EOPNOTSUPP) errno = ENOSYS;
    return -1;
  }
  if (rc < 2) State.fs_mask &= ~LANDLOCK_ACCESS_FS_REFER;
  const struct landlock_ruleset_attr attr = {
      .handled_access_fs = State.fs_mask,
  };
  // [undocumented] landlock_create_ruleset() always returns o_cloexec
  //                assert(__sys_fcntl(rc, F_GETFD, 0) == FD_CLOEXEC);
  if ((rc = landlock_create_ruleset(&attr, sizeof(attr), 0)) < 0) return -1;
  // grant file descriptor a higher number that's less likely to interfere
  if ((fd = __sys_fcntl(rc, F_DUPFD_CLOEXEC, 100)) == -1) {
    return err_close(-1, rc);
  }
  if (sys_close(rc) == -1) {
    return err_close(-1, fd);
  }
  State.fd = fd;
  return 0;
}

static int sys_unveil_linux(const char *path, const char *permissions) {
  int rc;
  if (!State.fd && (rc = unveil_init()) == -1) return rc;
  if ((path && !permissions) || (!path && permissions)) return einval();
  if (!path && !permissions) return unveil_final();
  struct landlock_path_beneath_attr pb = {0};
  for (const char *c = permissions; *c != '\0'; c++) {
    switch (*c) {
      // clang-format off
      case 'r': pb.allowed_access |= UNVEIL_READ;   break;
      case 'w': pb.allowed_access |= UNVEIL_WRITE;  break;
      case 'x': pb.allowed_access |= UNVEIL_EXEC;   break;
      case 'c': pb.allowed_access |= UNVEIL_CREATE; break;
      default:  return einval();
        // clang-format on
    }
  }
  pb.allowed_access &= State.fs_mask;
  if ((rc = sys_open(path, O_PATH | O_CLOEXEC, 0)) == -1) return rc;
  pb.parent_fd = rc;
  struct stat st;
  if ((rc = sys_fstat(pb.parent_fd, &st)) == -1) {
    return err_close(rc, pb.parent_fd);
  }
  if (!S_ISDIR(st.st_mode)) pb.allowed_access &= FILE_BITS;
  if ((rc = landlock_add_rule(State.fd, LANDLOCK_RULE_PATH_BENEATH, &pb, 0))) {
    return err_close(rc, pb.parent_fd);
  }
  sys_close(pb.parent_fd);
  return rc;
}

/**
 * Restricts filesystem operations, e.g.
 *
 *    unveil(".", "r");     // current directory + children are visible
 *    unveil("/etc", "r");  // make /etc readable too
 *    unveil(0, 0);         // commit and lock policy
 *
 * Unveiling restricts a thread's view of the filesystem to a set of
 * allowed paths with specific privileges.
 *
 * Once you start using unveil(), the entire file system is considered
 * hidden. You then specify, by repeatedly calling unveil(), which paths
 * should become unhidden. When you're finished, you call `unveil(0,0)`
 * which commits your policy, after which further use is forbidden, in
 * the current thread, as well as any threads or processes it spawns.
 *
 * There are some differences between unveil() on Linux versus OpenBSD.
 *
 * 1. Build your policy and lock it in one go. On OpenBSD, policies take
 *    effect immediately and may evolve as you continue to call unveil()
 *    but only in a more restrictive direction. On Linux, nothing will
 *    happen until you call `unveil(0,0)` which commits and locks.
 *
 * 2. Try not to overlap directory trees. On OpenBSD, if directory trees
 *    overlap, then the most restrictive policy will be used for a given
 *    file. On Linux overlapping may result in a less restrictive policy
 *    and possibly even undefined behavior.
 *
 * 3. OpenBSD and Linux disagree on error codes. On OpenBSD, accessing
 *    paths outside of the allowed set raises ENOENT, and accessing ones
 *    with incorrect permissions raises EACCES. On Linux, both these
 *    cases raise EACCES.
 *
 * 4. Unlike OpenBSD, Linux does nothing to conceal the existence of
 *    paths. Even with an unveil() policy in place, it's still possible
 *    to access the metadata of all files using functions like stat()
 *    and open(O_PATH), provided you know the path. A sandboxed process
 *    can always, for example, determine how many bytes of data are in
 *    /etc/passwd, even if the file isn't readable. But it's still not
 *    possible to use opendir() and go fishing for paths which weren't
 *    previously known.
 *
 * This system call is supported natively on OpenBSD and polyfilled on
 * Linux using the Landlock LSM[1].
 *
 * @param path is the file or directory to unveil
 * @param permissions is a string consisting of zero or more of the
 *     following characters:
 *
 *     - 'r' makes `path` available for read-only path operations,
 *       corresponding to the pledge promise "rpath".
 *
 *     - `w` makes `path` available for write operations, corresponding
 *       to the pledge promise "wpath".
 *
 *     - `x` makes `path` available for execute operations,
 *       corresponding to the pledge promises "exec" and "execnative".
 *
 *     - `c` allows `path` to be created and removed, corresponding to
 *       the pledge promise "cpath".
 *
 * @return 0 on success, or -1 w/ errno
 * @raise ENOSYS if host os isn't Linux or OpenBSD
 * @raise ENOSYS if Landlock isn't supported on this kernel
 * @raise EINVAL if one argument is set and the other is not
 * @raise EINVAL if an invalid character in `permissions` was found
 * @raise EPERM if unveil() is called after locking
 * @see [1] https://docs.kernel.org/userspace-api/landlock.html
 */
int unveil(const char *path, const char *permissions) {
  int rc;
  if (IsLinux()) {
    rc = sys_unveil_linux(path, permissions);
  } else {
    rc = sys_unveil(path, permissions);
  }
  STRACE("unveil(%#s, %#s) → %d% m", path, permissions, rc);
  return rc;
}
