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
#include "libc/sysv/consts/f.h"

#undef fcntl

/**
 * Does things with file descriptor, e.g.
 *
 *     CHECK_NE(-1, fcntl(fd, F_SETFD, FD_CLOEXEC));
 *
 * This function lets you duplicate file descriptors without running
 * into an edge case where they take over stdio handles:
 *
 *     CHECK_GE((newfd = fcntl(oldfd, F_DUPFD,         3)), 3);
 *     CHECK_GE((newfd = fcntl(oldfd, F_DUPFD_CLOEXEC, 3)), 3);
 *
 * This function implements file record locking, which lets independent
 * processes (and on Linux 3.15+, threads too!) lock arbitrary ranges
 * associated with a file. See `test/libc/calls/lock_test.c` and other
 * locking related tests in that folder.
 *
 * On Windows, the Cosmopolitan Libc polyfill for POSIX advisory locks
 * only implements enough of its nuances to support SQLite's needs. Some
 * possibilities, e.g. punching holes in lock, will raise `ENOTSUP`.
 *
 * @param fd is the file descriptor
 * @param cmd can be one of:
 *     - `F_GETFD` gets `FD_CLOEXEC` status of `fd
 *     - `F_SETFD` sets `FD_CLOEXEC` status of `arg` file descriptor
 *     - `F_GETFL` returns file descriptor status flags
 *     - `F_SETFL` sets file descriptor status flags
 *       - `O_NONBLOCK` may be changed
 *       - `O_APPEND` may be changed
 *       - `O_DIRECT` may be changed
 *       - Other bits (`O_ACCMODE`, `O_CREAT`, etc.) are ignored
 *     - `F_DUPFD` is like dup() but `arg` is a minimum result, e.g. 3
 *     - `F_DUPFD_CLOEXEC` ditto but sets `O_CLOEXEC` on returned fd
 *     - `F_SETLK` for record locking where `arg` is `struct flock *`
 *     - `F_SETLKW` ditto but waits for lock (SQLite avoids this)
 *     - `F_GETLK` to retrieve information about a record lock
 * @param arg can be FD_CLOEXEC, etc. depending
 * @return 0 on success, or -1 w/ errno
 * @raise EBADF if `fd` isn't a valid open file descriptor
 * @raise EINVAL if `cmd` is unknown or unsupported by os
 * @raise EINVAL if `cmd` is invalid or unsupported by os
 * @raise EPERM if pledge() is in play w/o `stdio` or `flock` promise
 * @raise ENOLCK if `F_SETLKW` would have exceeded `RLIMIT_LOCKS`
 * @raise EPERM if `cmd` is `F_SETOWN` and we weren't authorized
 * @raise ESRCH if `cmd` is `F_SETOWN` and process group not found
 * @raise ENOTSUP on Windows if locking operation isn't supported yet
 * @raise EDEADLK if `cmd` was `F_SETLKW` and waiting would deadlock
 * @raise EMFILE if `cmd` is `F_DUPFD` and `RLIMIT_NOFILE` is exceeded
 * @cancelationpoint when `cmd` is `F_SETLKW`
 * @asyncsignalsafe
 * @restartable
 */
int fcntl(int fd, int cmd, ...) {
  va_list va;
  uintptr_t arg;
  va_start(va, cmd);
  arg = va_arg(va, uintptr_t);
  va_end(va);
  if (cmd == F_GETFL) {
    return __fcntl_getfl(fd);
  } else if (cmd == F_GETFD) {
    return __fcntl_getfd(fd);
  } else if (cmd == F_SETFL) {
    return __fcntl_setfl(fd, arg);
  } else if (cmd == F_SETFD) {
    return __fcntl_setfd(fd, arg);
  } else if (cmd == F_DUPFD) {
    return __fcntl_dupfd(fd, arg);
  } else if (cmd == F_DUPFD_CLOEXEC) {
    return __fcntl_dupfd_cloexec(fd, arg);
  } else if (cmd == F_GETLK || cmd == F_SETLK || cmd == F_SETLKW) {
    return __fcntl_lock(fd, cmd, arg);
  } else {
    return __fcntl_misc(fd, cmd, arg);
  }
}
