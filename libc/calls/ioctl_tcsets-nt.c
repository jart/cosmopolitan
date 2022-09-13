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
#include "libc/calls/internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/calls/struct/metatermios.internal.h"
#include "libc/calls/termios.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/nt/enum/version.h"
#include "libc/nt/version.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

textwindows int ioctl_tcsets_nt(int ignored, uint64_t request,
                                const struct termios *tio) {
  int64_t in, out;
  bool32 ok, inok, outok;
  uint32_t inmode, outmode;
  inok = GetConsoleMode((in = __getfdhandleactual(0)), &inmode);
  outok = GetConsoleMode((out = __getfdhandleactual(1)), &outmode);
  if (inok | outok) {

    if (inok) {
      if (request == TCSETSF) {
        FlushConsoleInputBuffer(in);
      }
      inmode &=
          ~(kNtEnableLineInput | kNtEnableEchoInput | kNtEnableProcessedInput);
      inmode |= kNtEnableWindowInput;
      if (tio->c_lflag & ICANON) {
        inmode |= kNtEnableLineInput;
      }
      if (tio->c_lflag & ECHO) {
        /*
         * kNtEnableEchoInput can be used only if the ENABLE_LINE_INPUT mode
         * is also enabled. --Quoth MSDN
         */
        inmode |= kNtEnableEchoInput | kNtEnableLineInput;
      }
      if (tio->c_lflag & (IEXTEN | ISIG)) {
        inmode |= kNtEnableProcessedInput;
      }
      if (IsAtLeastWindows10()) {
        inmode |= kNtEnableVirtualTerminalInput;
      }
      ok = SetConsoleMode(in, inmode);
      NTTRACE("SetConsoleMode(%p, %s) → %hhhd", in,
              DescribeNtConsoleInFlags(inmode), ok);
    }

    if (outok) {
      outmode &= ~(kNtDisableNewlineAutoReturn);
      outmode |= kNtEnableProcessedOutput;
      if (!(tio->c_oflag & ONLCR)) {
        outmode |= kNtDisableNewlineAutoReturn;
      }
      if (IsAtLeastWindows10()) {
        outmode |= kNtEnableVirtualTerminalProcessing;
      }
      ok = SetConsoleMode(out, outmode);
      NTTRACE("SetConsoleMode(%p, %s) → %hhhd", out,
              DescribeNtConsoleOutFlags(outmode), ok);
    }

    return 0;
  } else {
    return enotty();
  }
}
