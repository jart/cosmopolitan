/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/landlock.h"
#include "libc/calls/struct/bpf.internal.h"
#include "libc/calls/struct/filter.internal.h"
#include "libc/calls/struct/seccomp.internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/struct/stat.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/libgen.h"
#include "libc/intrin/strace.internal.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/vendor.internal.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/stack.h"
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
#include "libc/thread/tls.h"

#ifdef __x86_64__
#define ARCHITECTURE AUDIT_ARCH_X86_64
#elif defined(__aarch64__)
#define ARCHITECTURE AUDIT_ARCH_AARCH64
#else
#error "unsupported architecture"
#endif

#define OFF(f) offsetof(struct seccomp_data, f)

#define UNVEIL_READ                                             \
  (LANDLOCK_ACCESS_FS_READ_FILE | LANDLOCK_ACCESS_FS_READ_DIR | \
   LANDLOCK_ACCESS_FS_REFER)
#define UNVEIL_WRITE \
  (LANDLOCK_ACCESS_FS_WRITE_FILE | LANDLOCK_ACCESS_FS_TRUNCATE)
#define UNVEIL_EXEC (LANDLOCK_ACCESS_FS_EXECUTE)
#define UNVEIL_CREATE                                             \
  (LANDLOCK_ACCESS_FS_MAKE_CHAR | LANDLOCK_ACCESS_FS_MAKE_DIR |   \
   LANDLOCK_ACCESS_FS_MAKE_REG | LANDLOCK_ACCESS_FS_MAKE_SOCK |   \
   LANDLOCK_ACCESS_FS_MAKE_FIFO | LANDLOCK_ACCESS_FS_MAKE_BLOCK | \
   LANDLOCK_ACCESS_FS_MAKE_SYM)

#define FILE_BITS                                                 \
  (LANDLOCK_ACCESS_FS_READ_FILE | LANDLOCK_ACCESS_FS_WRITE_FILE | \
   LANDLOCK_ACCESS_FS_EXECUTE)

static const struct sock_filter kUnveilBlacklistAbiVersionBelow3[] = {
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(arch)),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, ARCHITECTURE, 1, 0),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS),
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_truncate, 1, 0),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_setxattr, 0, 1),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | (1 & SECCOMP_RET_DATA)),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
};

static const struct sock_filter kUnveilBlacklistLatestAbi[] = {
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(arch)),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, ARCHITECTURE, 1, 0),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_KILL_PROCESS),
    BPF_STMT(BPF_LD | BPF_W | BPF_ABS, OFF(nr)),
    BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_linux_setxattr, 0, 1),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ERRNO | (1 & SECCOMP_RET_DATA)),
    BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW),
};

static int landlock_abi_version;
static int landlock_abi_errno;

__attribute__((__constructor__)) void init_landlock_version() {
  int e = errno;
  landlock_abi_version =
      landlock_create_ruleset(0, 0, LANDLOCK_CREATE_RULESET_VERSION);
  landlock_abi_errno = errno;
  errno = e;
}

/**
 * Long living state for landlock calls.
 * fs_mask is set to use all the access rights from the latest landlock ABI.
 * On init, the current supported abi is checked and unavailable rights are
 * masked off.
 *
 * As of 6.2, the latest abi is v3.
 *
 * TODO:
 *  - Integrate with pledge and remove the file access?
 *  - Stuff state into the .protected section?
 */
_Thread_local static struct {
  uint64_t fs_mask;
  int fd;
} State;

static int unveil_final(void) {
  int e, rc;
  struct sock_fprog sandbox = {
      .filter = kUnveilBlacklistLatestAbi,
      .len = ARRAYLEN(kUnveilBlacklistLatestAbi),
  };
  if (landlock_abi_version < 3) {
    sandbox = (struct sock_fprog){
        .filter = kUnveilBlacklistAbiVersionBelow3,
        .len = ARRAYLEN(kUnveilBlacklistAbiVersionBelow3),
    };
  }
  e = errno;
  prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);
  errno = e;
  if ((rc = landlock_restrict_self(State.fd, 0)) != -1 &&
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
  State.fs_mask = UNVEIL_READ | UNVEIL_WRITE | UNVEIL_EXEC | UNVEIL_CREATE;
  if (landlock_abi_version == -1) {
    errno = landlock_abi_errno;
    if (errno == EOPNOTSUPP) {
      errno = ENOSYS;
    }
    return -1;
  }
  if (landlock_abi_version < 2) {
    State.fs_mask &= ~LANDLOCK_ACCESS_FS_REFER;
  }
  if (landlock_abi_version < 3) {
    State.fs_mask &= ~LANDLOCK_ACCESS_FS_TRUNCATE;
  }
  const struct landlock_ruleset_attr attr = {
      .handled_access_fs = State.fs_mask,
  };
  // [undocumented] landlock_create_ruleset() always returns O_CLOEXEC
  //                assert(__sys_fcntl(rc, F_GETFD) == FD_CLOEXEC);
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

int sys_unveil_linux(const char *path, const char *permissions) {
#pragma GCC push_options
#pragma GCC diagnostic ignored "-Wframe-larger-than="
  struct {
    char lbuf[PATH_MAX];
    char buf1[PATH_MAX];
    char buf2[PATH_MAX];
    char buf3[PATH_MAX];
    char buf4[PATH_MAX];
  } b;
  CheckLargeStackAllocation(&b, sizeof(b));
#pragma GCC pop_options
  int rc;
  const char *dir;
  const char *last;
  const char *next;

  if (!State.fd && (rc = unveil_init()) == -1) return rc;
  if ((path && !permissions) || (!path && permissions)) return einval();
  if (!path && !permissions) return unveil_final();
  struct landlock_path_beneath_attr pb = {0};
  for (const char *c = permissions; *c != '\0'; c++) {
    switch (*c) {
      case 'r':
        pb.allowed_access |= UNVEIL_READ;
        break;
      case 'w':
        pb.allowed_access |= UNVEIL_WRITE;
        break;
      case 'x':
        pb.allowed_access |= UNVEIL_EXEC;
        break;
      case 'c':
        pb.allowed_access |= UNVEIL_CREATE;
        break;
      default:
        return einval();
    }
  }
  pb.allowed_access &= State.fs_mask;

  // landlock exposes all metadata, so we only technically need to add
  // realpath(path) to the ruleset. however a corner case exists where
  // it isn't valid, e.g. /dev/stdin -> /proc/2834/fd/pipe:[51032], so
  // we'll need to work around this, by adding the path which is valid
  if (strlen(path) + 1 > PATH_MAX) return enametoolong();
  last = path;
  next = path;
  for (int i = 0;; ++i) {
    if (i == 64) {
      // give up
      return eloop();
    }
    int err = errno;
    if ((rc = sys_readlinkat(AT_FDCWD, next, b.lbuf, PATH_MAX)) != -1) {
      if (rc < PATH_MAX) {
        // we need to nul-terminate
        b.lbuf[rc] = 0;
        // last = next
        strcpy(b.buf1, next);
        last = b.buf1;
        // next = join(dirname(next), link)
        strcpy(b.buf2, next);
        dir = dirname(b.buf2);
        if ((next = __join_paths(b.buf3, PATH_MAX, dir, b.lbuf))) {
          // next now points to either: buf3, buf2, lbuf, rodata
          strcpy(b.buf4, next);
          next = b.buf4;
        } else {
          return enametoolong();
        }
      } else {
        // symbolic link data was too long
        return enametoolong();
      }
    } else if (errno == EINVAL) {
      // next wasn't a symbolic link
      errno = err;
      path = next;
      break;
    } else if (i && (errno == ENOENT || errno == ENOTDIR)) {
      // next is a broken symlink, use last
      errno = err;
      path = last;
      break;
    } else {
      // readlink failed for some other reason
      return -1;
    }
  }

  // now we can open the path
  BLOCK_CANCELATION;
  rc = sys_openat(AT_FDCWD, path, O_PATH | O_NOFOLLOW | O_CLOEXEC, 0);
  ALLOW_CANCELATION;
  if (rc == -1) return rc;

  pb.parent_fd = rc;
  struct stat st;
  if ((rc = sys_fstat(pb.parent_fd, &st)) == -1) {
    return err_close(rc, pb.parent_fd);
  }
  if (!S_ISDIR(st.st_mode)) {
    pb.allowed_access &= FILE_BITS;
  }
  if ((rc = landlock_add_rule(State.fd, LANDLOCK_RULE_PATH_BENEATH, &pb, 0))) {
    return err_close(rc, pb.parent_fd);
  }
  sys_close(pb.parent_fd);
  return rc;
}

/**
 * Makes files accessible, e.g.
 *
 *     unveil(".", "r");     // current directory + children are visible
 *     unveil("/etc", "r");  // make /etc readable too
 *     unveil(0, 0);         // commit and lock policy
 *
 * Unveiling restricts a view of the filesystem to a set of allowed
 * paths with specific privileges.
 *
 * Once you start using unveil(), the entire file system is considered
 * hidden. You then specify, by repeatedly calling unveil(), which paths
 * should become unhidden. When you're finished, you call `unveil(0,0)`
 * which commits your policy.
 *
 * This function requires OpenBSD or Linux 5.13+ (2022+). If the kernel
 * support isn't available (or we're in an emulator like Qemu or Blink)
 * then zero is returned and nothing happens (instead of raising ENOSYS)
 * because the files are still unveiled. Use `unveil("", 0)` to feature
 * check the host system, which is defined as a no-op that'll fail if
 * the host system doesn't have the necessary features that allow
 * unveil() impose bona-fide security restrictions. Otherwise, if
 * everything is good, a return value `>=0` is returned, where `0` means
 * OpenBSD, and `>=1` means Linux with Landlock LSM, in which case the
 * return code shall be the maximum supported Landlock ABI version.
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
 * 5. Use ftruncate() rather than truncate() if you wish for portability
 *    to Linux kernels versions released before February 2022. One issue
 *    Landlock hadn't addressed as of ABI version 2 was restrictions
 *    over truncate() and setxattr() which could permit certain kinds of
 *    modifications to files outside the sandbox. When your policy is
 *    committed, we install a SECCOMP BPF filter to disable those calls,
 *    however similar trickery may be possible through other unaddressed
 *    calls like ioctl(). Using the pledge() function in addition to
 *    unveil() will solve this, since it installs a strong system call
 *    access policy. Linux 6.2 has improved this situation with Landlock
 *    ABI v3, which added the ability to control truncation operations -
 *    this means the SECCOMP BPF filter will only disable truncate() on
 *    Linux 6.1 or older.
 *
 * 6. Set your process-wide policy at startup from the main thread. On
 *    OpenBSD unveil() will apply process-wide even when called from a
 *    child thread; whereas with Linux, calling unveil() from a thread
 *    will cause your ruleset to only apply to that thread in addition
 *    to any descendent threads it creates.
 *
 * 7. Always specify at least one path. OpenBSD has unclear semantics
 *    when `unveil(0,0)` is used without any previous calls.
 *
 * 8. On OpenBSD calling `unveil(0,0)` will prevent unveil() from being
 *    used again. On Linux this is allowed, because Landlock is able to
 *    do that securely, i.e. the second ruleset can only be a subset of
 *    the previous ones.
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
 * @return 0 on success, or -1 w/ errno; note: if `unveil("",0)` is used
 *     to perform a feature check, then on Linux a value greater than 0
 *     shall be returned which is the supported Landlock ABI version
 * @raise EPERM if unveil() is called after locking
 * @raise EINVAL if one argument is set and the other is not
 * @raise EINVAL if an invalid character in `permissions` was found
 * @raise ENOSYS if `unveil("",0)` was used and security isn't possible
 * @raise EOPNOTSUPP if `unveil("",0)` was used and Landlock LSM is disabled
 * @note on Linux this function requires Linux Kernel 5.13+ and version 6.2+
 *     to properly support truncation operations
 * @see [1] https://docs.kernel.org/userspace-api/landlock.html
 */
int unveil(const char *path, const char *permissions) {
  int e, rc;
  e = errno;
  if (path && !*path) {
    // OpenBSD will always fail on both unveil("",0) and unveil("",""),
    // since an empty `path` is invalid and `permissions` is mandatory.
    // Cosmopolitan Libc uses it as a feature check convention, to test
    // if the host environment enables unveil() to impose true security
    // restrictions because the default behavior is to silently succeed
    // so that programs will err on the side of working if distributed.
    if (permissions) return einval();
    if (IsOpenbsd()) return 0;
    if (landlock_abi_version != -1) {
      unassert(landlock_abi_version >= 1);
      return landlock_abi_version;
    } else {
      unassert(landlock_abi_errno);
      errno = landlock_abi_errno;
      return -1;
    }
  } else if (!IsTiny() && IsGenuineBlink()) {
    rc = 0;  // blink doesn't support landlock; avoid noisy log warnings
  } else if (IsLinux()) {
    rc = sys_unveil_linux(path, permissions);
  } else {
    rc = sys_unveil(path, permissions);
  }
  if (rc == -1 && errno == ENOSYS) {
    errno = e;
    rc = 0;
  }
  STRACE("unveil(%#s, %#s) → %d% m", path, permissions, rc);
  return rc;
}
