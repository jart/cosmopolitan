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
#include "libc/calls/ioctl.h"
#include "libc/calls/termios.h"
#include "libc/fmt/itoa.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/pty.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

/**
 * Opens new pseudo teletypewriter.
 *
 * @param mfd receives controlling tty rw fd on success
 * @param sfd receives subordinate tty rw fd on success
 * @param tio may be passed to tune a century of legacy behaviors
 * @param wsz may be passed to set terminal display dimensions
 * @params flags is usually O_RDWR|O_NOCTTY
 * @return 0 on success, or -1 w/ errno
 */
int openpty(int *mfd, int *sfd, char *name, const struct termios *tio,
            const struct winsize *wsz) {
  int m, s, n;
  char buf[20];
  if ((m = open("/dev/ptmx", O_RDWR | O_NOCTTY)) != -1) {
    n = 0;
    if (!ioctl(m, TIOCSPTLCK, &n) && !ioctl(m, TIOCGPTN, &n)) {
      if (!name) name = buf;
      name[0] = '/', name[1] = 'd', name[2] = 'e', name[3] = 'v';
      name[4] = '/', name[5] = 'p', name[6] = 't', name[7] = 's';
      name[8] = '/', FormatInt32(name + 9, n);
      if ((s = open(name, O_RDWR | O_NOCTTY)) != -1) {
        if (tio) ioctl(s, TCSETS, tio);
        if (wsz) ioctl(s, TIOCSWINSZ, wsz);
        *mfd = m;
        *sfd = s;
        return 0;
      }
    }
    close(m);
  }
  return -1;
}
