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
#include "libc/calls/struct/stat.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/threaded.h"
#include "libc/runtime/internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/at.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/pr.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"

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
  if (State.fd == -1) return eperm();
  if ((rc = prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) != -1 &&
      (rc = landlock_restrict_self(State.fd, 0)) != -1 &&
      (rc = sys_close(State.fd)) != -1)
    State.fd = -1;
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
  if ((rc = landlock_create_ruleset(0, 0, LANDLOCK_CREATE_RULESET_VERSION)) < 0) {
    if (errno == EOPNOTSUPP) errno = ENOSYS;
    return -1;
  }
  if (rc < 2) State.fs_mask &= ~LANDLOCK_ACCESS_FS_REFER;
  const struct landlock_ruleset_attr attr = {
      .handled_access_fs = State.fs_mask,
  };
  if ((rc = landlock_create_ruleset(&attr, sizeof(attr), 0)) < 0) return -1;
  // grant file descriptor a higher number that's less likely to interfere
  if ((fd = __sys_fcntl(rc, F_DUPFD, 100)) == -1) return err_close(-1, rc);
  if (sys_close(rc) == -1) return err_close(-1, fd);
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
 *    unveil("/etc", "r");
 *
 * Unveiling restricts the visibility of the filesystem to a set of allowed
 * paths with specific operations. This system call is supported natively on
 * OpenBSD and polyfilled on Linux using the Landlock LSM[1].
 *
 * On OpenBSD, accessing paths outside of the allowed set raises ENOENT, and
 * accessing ones with incorrect permissions raises EACCES. On Linux, both these
 * cases raise EACCES.
 *
 * Using unveil is irreversible. On OpenBSD, the first call immediately enforces
 * the filesystem visibilty, and existing paths can only be updated with equal
 * or lesser permissions. Filesystem operations that try to access invisible
 * paths will raise ENOENT, and operations without the correct permissions raise
 * EACCES. Unveiling can be disabled by either passing two NULL arguments or by
 * calling pledge() without the "unveil" promise.
 *
 * Landlock is more permissive than OpenBSD's unveil. Filesystem visibility is
 * only enforced after disabling, and path permissions can be increased at any
 * time. Finally, both accessing invisible paths or ones with incorrect
 * permissions will raise EACCES.
 *
 * `permissions` is a string consisting of zero or more of the following
 * characters:
 *
 * - 'r' makes `path` available for read-only path operations, corresponding to
 *   the pledge promise "rpath".
 * - `w` makes `path` available for write operations, corresponding to the
 *   pledge promise "wpath".
 * - `x` makes `path` available for execute operations, corresponding to the
 *   pledge promises "exec" and "execnative".
 * - `c` allows `path` to be created and removed, corresponding to the pledge
 *   promise "cpath".
 *
 * [1] https://docs.kernel.org/userspace-api/landlock.html
 *
 * @return 0 on success, or -1 w/ errno
 * @raise ENOSYS if host os isn't Linux or OpenBSD
 * @raise ENOSYS if Landlock isn't supported on this kernel
 * @raise EINVAL if one argument is set and the other is not
 * @raise EINVAL if an invalid character in `permissions` was found
 */
int unveil(const char *path, const char *permissions) {
  int rc;
  __enable_tls();
  if (IsLinux()) {
    rc = sys_unveil_linux(path, permissions);
  } else {
    rc = sys_unveil(path, permissions);
  }
  STRACE("unveil(%#s, %#s) → %d% m", path, permissions, rc);
  return rc;
}
