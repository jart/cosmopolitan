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
#include "libc/calls/sig.internal.h"
#include "libc/calls/struct/metatermios.internal.h"
#include "libc/calls/syscall-nt.internal.h"
#include "libc/calls/termios.h"
#include "libc/calls/termios.internal.h"
#include "libc/calls/ttydefaults.h"
#include "libc/dce.h"
#include "libc/errno.h"
#include "libc/intrin/describeflags.internal.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/intrin/strace.internal.h"
#include "libc/macros.internal.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/consolemodeflags.h"
#include "libc/nt/enum/version.h"
#include "libc/nt/runtime.h"
#include "libc/nt/version.h"
#include "libc/runtime/runtime.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/fileno.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

#ifdef __x86_64__

textwindows int tcsetattr_nt(int fd, int opt, const struct termios *tio) {
  bool32 ok;
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

  if (opt == TCSAFLUSH) {
    FlushConsoleInputBytes(hInput);
  }
  inmode &= ~(kNtEnableLineInput | kNtEnableEchoInput |
              kNtEnableProcessedInput | kNtEnableVirtualTerminalInput);
  inmode |= kNtEnableWindowInput;
  __ttyconf.magic = 0;
  if (tio->c_lflag & ICANON) {
    inmode |= kNtEnableLineInput | kNtEnableQuickEditMode;
  } else {
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
  if ((tio->c_lflag & ISIG) && __ttyconf.vintr == CTRL('C')) {
    // allows ctrl-c to be delivered asynchronously via win32
    // TODO(jart): Fix up sig.c more.
    // inmode |= kNtEnableProcessedInput;
  }
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
}

#endif /* __x86_64__ */
