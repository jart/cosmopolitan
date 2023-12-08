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
#include "libc/calls/calls.h"
#include "libc/calls/cp.internal.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/flock.h"
#include "libc/calls/struct/flock.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/dce.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/runtime/zipos.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/errfuns.h"

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
 *       - `O_SEQUENTIAL` may be changed (no-op on non-Windows)
 *       - `O_RANDOM` may be changed (no-op on non-Windows)
 *       - Other bits (`O_ACCMODE`, `O_CREAT`, etc.) are ignored
 *     - `F_DUPFD` is like dup() but `arg` is a minimum result, e.g. 3
 *     - `F_DUPFD_CLOEXEC` ditto but sets `O_CLOEXEC` on returned fd
 *     - `F_SETLK` for record locking where `arg` is `struct flock *`
 *     - `F_SETLKW` ditto but waits for lock (SQLite avoids this)
 *     - `F_GETLK` to retrieve information about a record lock
 *     - `F_OFD_SETLK` for better non-blocking lock (Linux 3.15+ only)
 *     - `F_OFD_SETLKW` for better blocking lock (Linux 3.15+ only)
 *     - `F_OFD_GETLK` for better lock querying (Linux 3.15+ only)
 *     - `F_FULLFSYNC` on MacOS for fsync() with release barrier
 *     - `F_BARRIERFSYNC` on MacOS for fsync() with even more barriers
 *     - `F_SETNOSIGPIPE` on MacOS and NetBSD to control `SIGPIPE`
 *     - `F_GETNOSIGPIPE` on MacOS and NetBSD to control `SIGPIPE`
 *     - `F_GETPATH` on MacOS and NetBSD where arg is `char[PATH_MAX]`
 *     - `F_MAXFD` on NetBSD to get max open file descriptor
 *     - `F_NOCACHE` on MacOS to toggle data caching
 *     - `F_GETPIPE_SZ` on Linux to get pipe size
 *     - `F_SETPIPE_SZ` on Linux to set pipe size
 *     - `F_NOTIFY` raise `SIGIO` upon `fd` events in `arg` (Linux only)
 *       - `DN_ACCESS` for file access
 *       - `DN_MODIFY` for file modifications
 *       - `DN_CREATE` for file creations
 *       - `DN_DELETE` for file deletions
 *       - `DN_RENAME` for file renames
 *       - `DN_ATTRIB` for file attribute changes
 *       - `DN_MULTISHOT` bitwise or for realtime signals (non-coalesced)
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
 * @raise EMFILE if `cmd` is `F_DUPFD` or `F_DUPFD_CLOEXEC` and
 *     `RLIMIT_NOFILE` would be exceeded
 * @cancelationpoint when `cmd` is `F_SETLKW` or `F_OFD_SETLKW`
 * @asyncsignalsafe
 * @restartable
 */
int fcntl(int fd, int cmd, ...) {
  int rc;
  va_list va;
  uintptr_t arg;

  va_start(va, cmd);
  arg = va_arg(va, uintptr_t);
  va_end(va);

  if (fd >= 0) {
    if (cmd >= 0) {
      if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
        rc = _weaken(__zipos_fcntl)(fd, cmd, arg);
      } else if (!IsWindows()) {
        if (cmd == F_SETLKW || cmd == F_OFD_SETLKW) {
          BEGIN_CANCELATION_POINT;
          rc = sys_fcntl(fd, cmd, arg, __sys_fcntl_cp);
          END_CANCELATION_POINT;
        } else {
          rc = sys_fcntl(fd, cmd, arg, __sys_fcntl);
          if (rc != -1 && (cmd == F_DUPFD || cmd == F_DUPFD_CLOEXEC) &&
              __isfdkind(rc, kFdZip)) {
            _weaken(__zipos_postdup)(fd, rc);
          }
        }
      } else {
        rc = sys_fcntl_nt(fd, cmd, arg);
      }
    } else {
      rc = einval();
    }
  } else {
    rc = ebadf();
  }

#if SYSDEBUG
  if (rc != -1 && cmd == F_GETFL) {
    STRACE("fcntl(%d, F_GETFL) → %s", fd, DescribeOpenFlags(rc));
  } else if (cmd == F_SETFL) {
    STRACE("fcntl(%d, F_SETFL, %s) → %d% m", fd, DescribeOpenFlags(arg), rc);
  } else if (cmd == F_GETFD ||         //
             cmd == F_GETOWN ||        //
             cmd == F_FULLFSYNC ||     //
             cmd == F_BARRIERFSYNC ||  //
             cmd == F_MAXFD) {
    STRACE("fcntl(%d, %s) → %d% m", fd, DescribeFcntlCmd(cmd), rc);
  } else if (cmd == F_GETFL) {
    STRACE("fcntl(%d, %s) → %s% m", fd, DescribeFcntlCmd(cmd),
           DescribeOpenFlags(rc));
  } else if (cmd == F_SETLK ||                       //
             cmd == F_SETLKW ||                      //
             cmd == F_GETLK ||                       //
             ((SupportsLinux() || SupportsXnu()) &&  //
              cmd != -1 &&                           //
              (cmd == F_OFD_SETLK ||                 //
               cmd == F_OFD_SETLKW ||                //
               cmd == F_OFD_GETLK))) {
    STRACE("fcntl(%d, %s, %s) → %d% m", fd, DescribeFcntlCmd(cmd),
           DescribeFlock(cmd, (struct flock *)arg), rc);
  } else if ((SupportsNetbsd() || SupportsXnu()) && cmd != -1 &&
             cmd == F_GETPATH) {
    STRACE("fcntl(%d, %s, [%s]) → %d% m", fd, DescribeFcntlCmd(cmd),
           !rc ? (const char *)arg : "n/a", rc);
  } else if (SupportsLinux() && cmd != -1 && cmd == F_NOTIFY) {
    STRACE("fcntl(%d, %s, %s) → %d% m", fd, DescribeFcntlCmd(cmd),
           DescribeDnotifyFlags(arg), rc);
  } else {
    STRACE("fcntl(%d, %s, %ld) → %d% m", fd, DescribeFcntlCmd(cmd), arg, rc);
  }
#endif

  return rc;
}
