/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/paths.h"
#include "libc/runtime/runtime.h"
#include "libc/stdio/internal.h"
#include "libc/stdio/stdio.h"
#include "libc/sysv/consts/f.h"
#include "libc/sysv/consts/fd.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/errfuns.h"

/**
 * Spawns subprocess and returns pipe stream.
 * @see pclose()
 */
FILE *popen(const char *cmdline, const char *mode) {
  FILE *f;
  int e, pid, dir, flags, pipefds[2];
  flags = fopenflags(mode);
  if ((flags & O_ACCMODE) == O_RDONLY) {
    dir = 0;
  } else if ((flags & O_ACCMODE) == O_WRONLY) {
    dir = 1;
  } else {
    errno = EINVAL;
    return NULL;
  }
  if (pipe(pipefds) == -1) return NULL;
  fcntl(pipefds[dir], F_SETFD, FD_CLOEXEC);
  if ((f = fdopen(pipefds[dir], mode))) {
    switch ((pid = vfork())) {
      case 0:
        dup2(pipefds[!dir], !dir);
        systemexec(cmdline);
        _exit(127);
      default:
        f->pid = pid;
        close(pipefds[!dir]);
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
