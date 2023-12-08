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
#include "libc/assert.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/termios.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/baud.internal.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

textwindows int tcgetattr_nt(int fd, struct termios *tio) {
  int64_t hInput, hOutput;
  uint32_t inmode, outmode;

  // validate file descriptor
  if (!__isfdopen(fd)) return ebadf();
  if (!__isfdkind(fd, kFdConsole)) return enotty();

  // then completely ignore it
  hInput = GetConsoleInputHandle();
  hOutput = GetConsoleOutputHandle();
  unassert(GetConsoleMode(hInput, &inmode));
  unassert(GetConsoleMode(hOutput, &outmode));

  // now interpret the configuration
  bzero(tio, sizeof(*tio));
  memcpy(tio->c_cc, __ttyconf.c_cc, NCCS);
  tio->c_iflag = IUTF8;
  tio->c_lflag = ECHOE;
  tio->c_cflag = CS8 | CREAD;
  tio->_c_ispeed = B38400;
  tio->_c_ospeed = B38400;

  // kNtEnableLineInput and kNtEnableEchoInput only apply to programs
  // that call ReadFile() or ReadConsole(). since we do not use them,
  // the flags could serve the purpose of inter-process communication
  if ((inmode & kNtEnableLineInput) || !(__ttyconf.magic & kTtyUncanon)) {
    tio->c_lflag |= ICANON;
  }
  // kNtEnableEchoInput only works with kNtEnableLineInput enabled.
  if ((inmode & kNtEnableEchoInput) || !(__ttyconf.magic & kTtySilence)) {
    tio->c_lflag |= ECHO;
  }
  // The Windows console itself always echos control codes as ASCII.
  if ((inmode & kNtEnableEchoInput) || !(__ttyconf.magic & kTtyEchoRaw)) {
    tio->c_lflag |= ECHOCTL;
  }
  if (!(__ttyconf.magic & kTtyNoCr2Nl)) {
    tio->c_iflag |= ICRNL;
  }
  if (!(__ttyconf.magic & kTtyNoIsigs)) {
    tio->c_lflag |= ISIG;
  }
  if (inmode & kNtEnableProcessedInput) {
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

#endif /* __x86_64__ */
