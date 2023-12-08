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
#include "libc/calls/ttydefaults.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/nt/version.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/baud.internal.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"
#ifdef __x86_64__

textwindows int tcsetattr_nt(int fd, int opt, const struct termios *tio) {
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
  if (opt == TCSAFLUSH) FlushConsoleInputBytes();

  // now work on the configuration
  inmode &= ~(kNtEnableLineInput | kNtEnableEchoInput |
              kNtEnableProcessedInput | kNtEnableVirtualTerminalInput);
  inmode |= kNtEnableWindowInput;
  __ttyconf.magic = 0;
  if (tio->c_lflag & ICANON) {
    inmode |= kNtEnableLineInput | kNtEnableQuickEditMode;
  } else {
    // protip: hold down shift and you can use the mouse normally
    inmode &= ~kNtEnableQuickEditMode;
    __ttyconf.magic |= kTtyUncanon;
  }
  if (!(tio->c_iflag & ICRNL)) {
    __ttyconf.magic |= kTtyNoCr2Nl;
  }
  if (!(tio->c_lflag & ECHOCTL)) {
    __ttyconf.magic |= kTtyEchoRaw;
  }
  if (tio->c_lflag & ECHO) {
    // "kNtEnableEchoInput can be used only if the
    //  kNtEnableLineInput mode is also enabled." -MSDN
    if (tio->c_lflag & ICANON) {
      inmode |= kNtEnableEchoInput;
    }
  } else {
    __ttyconf.magic |= kTtySilence;
  }
  if (!(tio->c_lflag & ISIG)) {
    __ttyconf.magic |= kTtyNoIsigs;
  }
  memcpy(__ttyconf.c_cc, tio->c_cc, NCCS);
  if ((tio->c_lflag & ISIG) &&     //
      !(tio->c_lflag & ICANON) &&  //
      __ttyconf.vintr == CTRL('C')) {
    // allows ctrl-c to be delivered asynchronously via win32
    // we normally don't want win32 doing this 24/7 in the bg
    // because we don't have job control, tcsetpgrp, etc. yet
    // it's normally much better to let read-nt.c raise a sig
    // because read-nt only manages your tty whilst it's used
    inmode |= kNtEnableProcessedInput;
  }
  outmode &= ~kNtDisableNewlineAutoReturn;
  outmode |= kNtEnableProcessedOutput;
  if (!(tio->c_oflag & ONLCR)) {
    outmode |= kNtDisableNewlineAutoReturn;
  }
  if (IsAtLeastWindows10()) {
    outmode |= kNtEnableVirtualTerminalProcessing;
  }

  // tune the win32 configuration
  unassert(SetConsoleMode(hInput, inmode));
  unassert(SetConsoleMode(hOutput, outmode));
  return 0;
}

#endif /* __x86_64__ */
