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
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/winsize.h"
#include "libc/calls/termios.h"
#include "libc/dce.h"
#include "libc/intrin/kprintf.h"
#include "libc/log/rop.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/pty.h"
#include "libc/sysv/consts/termios.h"

struct IoctlPtmGet {
  int m;
  int s;
  char mname[16];
  char sname[16];
};

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
  int m, s, p;
  const char *t;
  struct IoctlPtmGet ptm;
  RETURN_ON_ERROR((m = posix_openpt(O_RDWR | O_NOCTTY)));
  if (!IsOpenbsd()) {
    RETURN_ON_ERROR(grantpt(m));
    RETURN_ON_ERROR(unlockpt(m));
    if (!(t = ptsname(m))) goto OnError;
    RETURN_ON_ERROR((s = open(t, O_RDWR)));
  } else {
    RETURN_ON_ERROR(ioctl(m, PTMGET, &ptm));
    close(m);
    m = ptm.m;
    s = ptm.s;
    t = ptm.sname;
  }
  *mfd = m;
  *sfd = s;
  if (name) strcpy(name, t);
  if (tio) ioctl(s, TCSETSF, tio);
  if (wsz) ioctl(s, TIOCSWINSZ, wsz);
  return 0;
OnError:
  if (m != -1) close(m);
  return -1;
}
