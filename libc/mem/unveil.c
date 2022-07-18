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
#include "libc/calls/landlock.h"
#include "libc/calls/strace.internal.h"
#include "libc/calls/struct/stat.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/syscall_support-sysv.internal.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/pr.h"
#include "libc/sysv/consts/s.h"
#include "libc/sysv/errfuns.h"

#define FILE_BITS                                                 \
  (LANDLOCK_ACCESS_FS_READ_FILE | LANDLOCK_ACCESS_FS_WRITE_FILE | \
   LANDLOCK_ACCESS_FS_EXECUTE)

/*
 * Long living state for landlock calls.
 * The bits are set at runtime to handle future API additions.
 * As of 5.19, the latest abi is v2.
 *
 * TODO:
 *  - Documentation for sys_unveil.
 *  - Integrate with pledge and remove the file access?
 *  - Stuff state into the .protected section?
 */
_Thread_local static struct {
  int abi;
  int fd;
  uint64_t read;
  uint64_t write;
  uint64_t exec;
  uint64_t create;
} State = {
    .abi = 2,
    .read = LANDLOCK_ACCESS_FS_READ_FILE | LANDLOCK_ACCESS_FS_READ_DIR |
            LANDLOCK_ACCESS_FS_REFER,
    .write = LANDLOCK_ACCESS_FS_WRITE_FILE,
    .exec = LANDLOCK_ACCESS_FS_EXECUTE,
    .create = LANDLOCK_ACCESS_FS_REMOVE_DIR | LANDLOCK_ACCESS_FS_MAKE_CHAR |
              LANDLOCK_ACCESS_FS_MAKE_DIR | LANDLOCK_ACCESS_FS_MAKE_REG |
              LANDLOCK_ACCESS_FS_MAKE_SOCK | LANDLOCK_ACCESS_FS_MAKE_FIFO |
              LANDLOCK_ACCESS_FS_MAKE_BLOCK | LANDLOCK_ACCESS_FS_MAKE_SYM,
};

static int unveil_final(void) {
  int rc;
  if (State.fd == -1) return 0;
  assert(State.fd > 0);
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
  if ((rc = landlock_create_ruleset(0, 0, LANDLOCK_CREATE_RULESET_VERSION)) < 0)
    return -1;
  State.abi = rc;
  if (State.abi < 2) State.read &= ~LANDLOCK_ACCESS_FS_REFER;
  const struct landlock_ruleset_attr attr = {
      .handled_access_fs = State.read | State.write | State.exec | State.create,
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
  if (!path && !permissions) return unveil_final();
  struct landlock_path_beneath_attr pb = {0};
  for (const char *c = permissions; *c != '\0'; c++) {
    switch (*c) {
      case 'r':
        pb.allowed_access |= State.read;
        break;
      case 'w':
        pb.allowed_access |= State.write;
        break;
      case 'x':
        pb.allowed_access |= State.exec;
        break;
      case 'c':
        pb.allowed_access |= State.create;
        break;
      default:
        return einval();
    }
  }
  if ((rc = sys_open(path, O_PATH | O_CLOEXEC, 0)) == -1) return rc;
  pb.parent_fd = rc;
  struct stat st;
  if ((rc = fstat(pb.parent_fd, &st)) == -1) return err_close(rc, pb.parent_fd);
  if (!S_ISDIR(st.st_mode)) pb.allowed_access &= FILE_BITS;
  if ((rc = landlock_add_rule(State.fd, LANDLOCK_RULE_PATH_BENEATH, &pb, 0)))
    return err_close(rc, pb.parent_fd);
  sys_close(pb.parent_fd);
  return rc;
}

/**
 * Unveil parts of a restricted filesystem view.
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
