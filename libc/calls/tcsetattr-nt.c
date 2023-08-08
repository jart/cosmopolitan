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
#include "libc/calls/struct/metatermios.internal.h"
#include "libc/calls/termios.internal.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/nt/enum/version.h"
#include "libc/nt/version.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

textwindows int tcsetattr_nt(int fd, int opt, const struct termios *tio) {
  bool32 ok;
  int ttymagic;
  int64_t hInput, hOutput;
  uint32_t inmode, outmode;
  if (__isfdkind(fd, kFdConsole)) {
    // program manually opened /dev/tty in O_RDWR mode for cmd.exe
    hInput = g_fds.p[fd].handle;
    hOutput = g_fds.p[fd].extra;
  } else if (fd == 0 || fd == 1) {
    // otherwise just assume cmd.exe console stdio
    // there's no serial port support yet
    hInput = g_fds.p[0].handle;
    hOutput = g_fds.p[1].handle;
    fd = 0;
  } else {
    STRACE("tcsetattr(fd) must be 0, 1, or open'd /dev/tty");
    return enotty();
  }
  if (GetConsoleMode(hInput, &inmode) && GetConsoleMode(hOutput, &outmode)) {

    if (opt == TCSAFLUSH) {
      FlushConsoleInputBuffer(hInput);
    }
    inmode &=
        ~(kNtEnableLineInput | kNtEnableEchoInput | kNtEnableProcessedInput);
    inmode |= kNtEnableWindowInput;
    ttymagic = 0;
    if (tio->c_lflag & ICANON) {
      inmode |= kNtEnableLineInput;
    } else {
      ttymagic |= kFdTtyMunging;
      if (tio->c_cc[VMIN] != 1) {
        STRACE("tcsetattr c_cc[VMIN] must be 1 on Windows");
        return einval();
      }
    }
    if (!(tio->c_iflag & ICRNL)) {
      ttymagic |= kFdTtyNoCr2Nl;
    }
    if (!(tio->c_lflag & ECHOCTL)) {
      ttymagic |= kFdTtyEchoRaw;
    }
    if (tio->c_lflag & ECHO) {
      // "kNtEnableEchoInput can be used only if the
      //  kNtEnableLineInput mode is also enabled." -MSDN
      if (tio->c_lflag & ICANON) {
        inmode |= kNtEnableEchoInput;
      } else {
        // If ECHO is enabled in raw mode, then read(0) needs to
        // magically write(1) to simulate echoing. This normally
        // visualizes control codes, e.g. \r → ^M unless ECHOCTL
        // hasn't been specified.
        ttymagic |= kFdTtyEchoing;
      }
    }
    if (tio->c_lflag & (IEXTEN | ISIG)) {
      inmode |= kNtEnableProcessedInput;
    }
    if (IsAtLeastWindows10()) {
      inmode |= kNtEnableVirtualTerminalInput;
    }
    g_fds.p[fd].ttymagic = ttymagic;
    ok = SetConsoleMode(hInput, inmode);
    (void)ok;
    NTTRACE("SetConsoleMode(%p, %s) → %hhhd", hInput,
            DescribeNtConsoleInFlags(inmode), ok);

    outmode &= ~kNtDisableNewlineAutoReturn;
    outmode |= kNtEnableProcessedOutput;
    if (!(tio->c_oflag & ONLCR)) {
      outmode |= kNtDisableNewlineAutoReturn;
    }
    if (IsAtLeastWindows10()) {
      outmode |= kNtEnableVirtualTerminalProcessing;
    }
    ok = SetConsoleMode(hOutput, outmode);
    (void)ok;
    NTTRACE("SetConsoleMode(%p, %s) → %hhhd", hOutput,
            DescribeNtConsoleOutFlags(outmode), ok);

    return 0;
  } else {
    return enotty();
  }
}
