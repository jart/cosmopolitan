/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/errno.h"
#include "libc/intrin/weaken.h"
#include "libc/paths.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/fflush.internal.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"
#include "libc/thread/thread.h"

/**
 * Spawns subprocess and returns pipe stream.
 *
 * The returned resource needs to be passed to pclose().
 *
 * This embeds the Cosmopolitan Command Interpreter which provides
 * Bourne-like syntax on all platforms including Windows.
 *
 * @param cmdline is a unix shell script
 * @param mode can be:
 *     - `"r"` for reading from subprocess standard output
 *     - `"w"` for writing to subprocess standard input
 *     - `"e"` for `O_CLOEXEC` on returned file
 * @raise EINVAL if `mode` is invalid or specifies read+write
 * @raise EMFILE if process `RLIMIT_NOFILE` has been reached
 * @raise ENFILE if system-wide file limit has been reached
 * @raise ECANCELED if thread was cancelled in masked mode
 * @raise ENOMEM if we require more vespene gas
 * @raise EAGAIN if `RLIMIT_NPROC` was exceeded
 * @raise EINTR if signal was delivered
 * @cancelationpoint
 */
FILE *popen(const char *cmdline, const char *mode) {
  FILE *f, *f2;
  int e, rc, pid, dir, flags, pipefds[2];
  flags = fopenflags(mode);
  if ((flags & O_ACCMODE) == O_RDONLY) {
    dir = 0;
  } else if ((flags & O_ACCMODE) == O_WRONLY) {
    dir = 1;
  } else {
    einval();
    return NULL;
  }
  if (_weaken(pthread_testcancel_np) &&
      (rc = _weaken(pthread_testcancel_np)())) {
    errno = rc;
    return 0;
  }
  if (pipe2(pipefds, O_CLOEXEC) == -1) return NULL;
  if ((f = fdopen(pipefds[dir], mode))) {
    switch ((pid = fork())) {
      case 0:
        unassert(dup2(pipefds[!dir], !dir) == !dir);
        // we can't rely on cloexec because cocmd builtins don't execve
        if (pipefds[0] != !dir) unassert(!close(pipefds[0]));
        if (pipefds[1] != !dir) unassert(!close(pipefds[1]));
        // "The popen() function shall ensure that any streams from
        //  previous popen() calls that remain open in the parent
        //  process are closed in the new child process." -POSIX
        for (int i = 0; i < __fflush.handles.i; ++i) {
          if ((f2 = __fflush.handles.p[i]) && f2->pid) {
            close(f2->fd);
          }
        }
        _Exit(_cocmd(3,
                     (char *[]){
                         "popen",
                         "-c",
                         (char *)cmdline,
                         0,
                     },
                     environ));
      default:
        f->pid = pid;
        unassert(!close(pipefds[!dir]));
        if (!(flags & O_CLOEXEC)) {
          unassert(!fcntl(pipefds[dir], F_SETFD, 0));
        }
        return f;
      case -1:
        e = errno;
        fclose(f);
        close(pipefds[!dir]);
        errno = e;
        return NULL;
    }
  } else {
    e = errno;
    close(pipefds[0]);
    close(pipefds[1]);
    errno = e;
    return NULL;
  }
}
