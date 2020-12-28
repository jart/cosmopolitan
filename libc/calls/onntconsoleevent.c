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
    case kNtCtrlLogoffEvent:    // only received by services so hack hack hack
    case kNtCtrlShutdownEvent:  // only received by services so hack hack hack
      sig = pushpop(SIGALRM);
      break;
    default:
      return false;
  }
  memset(&info, 0, sizeof(info));
  info.si_signo = sig;
  return __sigenter(sig, &info, NULL);
}
