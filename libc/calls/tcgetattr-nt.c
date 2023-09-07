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
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/ttydefaults.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/nt/struct/consolescreenbufferinfoex.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/baud.internal.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

textwindows int tcgetattr_nt(int fd, struct termios *tio) {
  int64_t hInput, hOutput;
  uint32_t inmode, outmode;

  if (__isfdkind(fd, kFdConsole)) {
    hInput = g_fds.p[fd].handle;
    hOutput = g_fds.p[fd].extra;
  } else if (fd == STDIN_FILENO ||   //
             fd == STDOUT_FILENO ||  //
             fd == STDERR_FILENO) {
    hInput = g_fds.p[STDIN_FILENO].handle;
    hOutput = g_fds.p[MAX(STDOUT_FILENO, fd)].handle;
  } else {
    return enotty();
  }

  if (!GetConsoleMode(hInput, &inmode) || !GetConsoleMode(hOutput, &outmode)) {
    return enotty();
  }

  bzero(tio, sizeof(*tio));

  tio->c_cc[VMIN] = 1;
  tio->c_cc[VINTR] = __vintr;
  tio->c_cc[VQUIT] = __vquit;
  tio->c_cc[VERASE] = CTRL('?');
  tio->c_cc[VWERASE] = CTRL('W');
  tio->c_cc[VKILL] = CTRL('U');
  tio->c_cc[VEOF] = CTRL('D');
  tio->c_cc[VMIN] = CTRL('A');
  tio->c_cc[VSTART] = _POSIX_VDISABLE;
  tio->c_cc[VSTOP] = _POSIX_VDISABLE;
  tio->c_cc[VSUSP] = _POSIX_VDISABLE;
  tio->c_cc[VREPRINT] = CTRL('R');
  tio->c_cc[VDISCARD] = CTRL('O');
  tio->c_cc[VLNEXT] = CTRL('V');

  tio->c_iflag = IUTF8;
  tio->c_lflag = ECHOE;
  tio->c_cflag = CS8 | CREAD;
  tio->_c_ispeed = B38400;
  tio->_c_ospeed = B38400;

  if (inmode & kNtEnableLineInput) {
    tio->c_lflag |= ICANON;
  }
  // kNtEnableEchoInput only works with kNtEnableLineInput enabled.
  if ((inmode & kNtEnableEchoInput) || (__ttymagic & kFdTtyEchoing)) {
    tio->c_lflag |= ECHO;
  }
  // The Windows console itself always echos control codes as ASCII.
  if ((inmode & kNtEnableEchoInput) || !(__ttymagic & kFdTtyEchoRaw)) {
    tio->c_lflag |= ECHOCTL;
  }
  if (!(__ttymagic & kFdTtyNoCr2Nl)) {
    tio->c_iflag |= ICRNL;
  }
  if (!(__ttymagic & kFdTtyNoIsigs)) {
    tio->c_lflag |= ISIG;
  }
  if ((inmode & kNtEnableProcessedInput) || (__ttymagic & kFdTtyMunging)) {
    tio->c_lflag |= IEXTEN;
  }

  if (outmode & kNtEnableProcessedOutput) {
    tio->c_oflag |= OPOST;
  }
  if (!(outmode & kNtDisableNewlineAutoReturn)) {
    tio->c_oflag |= OPOST | ONLCR;
  }

  return 0;
}
