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
#include "libc/calls/internal.h"
#include "libc/calls/struct/metatermios.internal.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/struct/termios.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/calls/termios.internal.h"
#include "libc/calls/ttydefaults.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

int tcgetattr_nt(int, struct termios *);

static int tcgetattr_metal(int fd, struct termios *tio) {
  bzero(tio, sizeof(*tio));
  tio->c_iflag = TTYDEF_IFLAG;
  tio->c_oflag = TTYDEF_OFLAG;
  tio->c_lflag = TTYDEF_LFLAG;
  tio->c_cflag = TTYDEF_CFLAG;
  return 0;
}

static int tcgetattr_bsd(int fd, struct termios *tio) {
  int rc;
  union metatermios mt;
  if ((rc = sys_ioctl(fd, TCGETS, &mt)) != -1) {
    if (IsXnu()) {
      COPY_TERMIOS(tio, &mt.xnu);
    } else {
      COPY_TERMIOS(tio, &mt.bsd);
    }
  }
  return rc;
}

/**
 * Obtains the termios struct.
 *
 * Here are the approximate defaults you can expect across platforms:
 *
 *     c_iflag        = ICRNL IXON
 *     c_oflag        = OPOST ONLCR NL0 CR0 TAB0 BS0 VT0 FF0
 *     c_cflag        = ISIG CREAD CS8
 *     c_lflag        = ISIG ICANON ECHO ECHOE ECHOK IEXTEN ECHOCTL ECHOKE
 *     cfgetispeed()  = B38400
 *     cfgetospeed()  = B38400
 *     c_cc[VINTR]    = CTRL-C
 *     c_cc[VQUIT]    = CTRL-\   # ignore this comment
 *     c_cc[VERASE]   = CTRL-?
 *     c_cc[VKILL]    = CTRL-U
 *     c_cc[VEOF]     = CTRL-D
 *     c_cc[VTIME]    = CTRL-@
 *     c_cc[VMIN]     = CTRL-A
 *     c_cc[VSTART]   = CTRL-Q
 *     c_cc[VSTOP]    = CTRL-S
 *     c_cc[VSUSP]    = CTRL-Z
 *     c_cc[VEOL]     = CTRL-@
 *     c_cc[VSWTC]    = CTRL-@
 *     c_cc[VREPRINT] = CTRL-R
 *     c_cc[VDISCARD] = CTRL-O
 *     c_cc[VWERASE]  = CTRL-W
 *     c_cc[VLNEXT]   = CTRL-V
 *     c_cc[VEOL2]    = CTRL-@
 *
 * @param fd open file descriptor that isatty()
 * @param tio is where result is stored
 * @return 0 on success, or -1 w/ errno
 * @asyncsignalsafe
 */
int tcgetattr(int fd, struct termios *tio) {
  int rc;
  if (fd < 0) {
    rc = einval();
  } else if (!tio || (IsAsan() && !__asan_is_valid(tio, sizeof(*tio)))) {
    rc = efault();
  } else if (fd < g_fds.n && g_fds.p[fd].kind == kFdZip) {
    rc = enotty();
  } else if (IsLinux()) {
    rc = sys_ioctl(fd, TCGETS, tio);
  } else if (IsBsd()) {
    rc = tcgetattr_bsd(fd, tio);
  } else if (IsMetal()) {
    rc = tcgetattr_metal(fd, tio);
  } else if (IsWindows()) {
    rc = tcgetattr_nt(fd, tio);
  } else {
    rc = enosys();
  }
  STRACE("tcgetattr(%d, [%s]) → %d% m", fd, DescribeTermios(rc, tio), rc);
  return rc;
}
