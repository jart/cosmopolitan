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
#include "libc/calls/sig.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/strace.internal.h"
#include "libc/nexgen32e/nt2sysv.h"
#include "libc/nt/enum/ctrlevent.h"
#include "libc/nt/thread.h"
#include "libc/str/str.h"
#include "libc/sysv/consts/sicode.h"
#include "libc/sysv/consts/sig.h"
#include "libc/thread/tls.h"
#include "libc/thread/tls2.h"

textwindows bool32 __onntconsoleevent(uint32_t dwCtrlType) {
  struct CosmoTib tib;
  struct StackFrame *fr;

  // win32 spawns a thread on its own just to deliver sigint
  // TODO(jart): make signal code lockless so we can delete!
  if (__tls_enabled && !__get_tls_win32()) {
    bzero(&tib, sizeof(tib));
    tib.tib_self = &tib;
    tib.tib_self2 = &tib;
    atomic_store_explicit(&tib.tib_tid, GetCurrentThreadId(),
                          memory_order_relaxed);
    __set_tls_win32(&tib);
  }

  STRACE("__onntconsoleevent(%u)", dwCtrlType);
  switch (dwCtrlType) {
    case kNtCtrlCEvent:
      __sig_add(0, SIGINT, SI_KERNEL);
      return true;
    case kNtCtrlBreakEvent:
      __sig_add(0, SIGQUIT, SI_KERNEL);
      return true;
    case kNtCtrlCloseEvent:
    case kNtCtrlLogoffEvent:    // only received by services
    case kNtCtrlShutdownEvent:  // only received by services
      __sig_add(0, SIGHUP, SI_KERNEL);
      return true;
    default:
      return false;
  }
}
