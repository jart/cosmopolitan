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
#include "libc/calls/calls.h"
#include "libc/calls/termios.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/pty.h"
#include "libc/sysv/errfuns.h"

/**
 * Opens new pseudo teletypewriter.
 *
 * @param ilduce receives controlling tty rw fd on success
 * @param aworker receives subordinate tty rw fd on success
 * @param termp may be passed to tune a century of legacy behaviors
 * @param winp may be passed to set terminal display dimensions
 * @params flags is usually O_RDWR|O_NOCTTY
 * @return file descriptor, or -1 w/ errno
 */
int openpty(int *ilduce, int *aworker, char *name, const struct termios *termp,
            const struct winsize *winp) {
  return enosys();
  /* TODO(jart) */
  /* int fd, flags; */
  /* flags = O_RDWR | O_NOCTTY; */
  /* if ((fd = posix_openpt(flags)) != -1) { */
  /*   if (ioctl(m, TIOCSPTLCK, &n) || ioctl(m, TIOCGPTN, &n)) { */
  /*   } else { */
  /*     close(fd); */
  /*   } */
  /* } else { */
  /*   return -1; */
  /* } */
}
