/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/internal.h"
#include "libc/calls/struct/metatermios.h"
#include "libc/calls/termios-internal.h"
#include "libc/nt/console.h"
#include "libc/nt/enum/version.h"
#include "libc/nt/struct/teb.h"
#include "libc/sysv/consts/o.h"
#include "libc/sysv/consts/termios.h"
#include "libc/sysv/errfuns.h"

textwindows int ioctl$tcsets$nt(int ignored, uint64_t request,
                                const struct termios *tio) {
  int64_t in, out;
  bool32 inok, outok;
  uint32_t inmode, outmode;
  inok = GetConsoleMode((in = g_fds.p[0].handle), &inmode);
  outok = GetConsoleMode((out = g_fds.p[1].handle), &outmode);
  if (inok | outok) {
    if (inok) {
      if (request == TCSETSF) {
        FlushConsoleInputBuffer(in);
      }
      inmode &=
          ~(kNtEnableLineInput | kNtEnableEchoInput | kNtEnableProcessedInput);
      if (tio->c_lflag & ECHO) inmode |= kNtEnableEchoInput;
      if (tio->c_lflag & (IEXTEN | ISIG)) inmode |= kNtEnableProcessedInput;
      SetConsoleMode(in, inmode);
    }
    if (outok) {
      SetConsoleMode(out, outmode | kNtEnableProcessedOutput |
                              (NtGetVersion() >= kNtVersionWindows10
                                   ? kNtEnableVirtualTerminalProcessing
                                   : 0));
    }
    return 0;
  } else {
    return enotty();
  }
}
