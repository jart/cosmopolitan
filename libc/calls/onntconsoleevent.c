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
#include "libc/bits/pushpop.h"
#include "libc/calls/internal.h"
#include "libc/calls/struct/siginfo.h"
#include "libc/nt/enum/ctrlevent.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sig.h"

textwindows bool32 onntconsoleevent(uint32_t CtrlType) {
  int sig;
  siginfo_t info;
  switch (CtrlType) {
    case kNtCtrlCEvent:
      sig = pushpop(SIGINT);
      break;
    case kNtCtrlBreakEvent:
      sig = pushpop(SIGQUIT);
      break;
    case kNtCtrlCloseEvent:
      sig = pushpop(SIGHUP);
      break;
    case kNtCtrlLogoffEvent:
    case kNtCtrlShutdownEvent:
      sig = pushpop(SIGTERM);
      break;
    default:
      return false;
  }
  memset(&info, 0, sizeof(info));
  info.si_signo = sig;
  return __sigenter(sig, &info, NULL);
}
