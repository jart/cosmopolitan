/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/calls/blockcancel.internal.h"
#include "libc/calls/cp.internal.h"
#include "libc/intrin/describebacktrace.h"
#include "libc/intrin/kprintf.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/thread/posixthread.internal.h"
#include "libc/thread/tls.h"
#ifdef MODE_DBG

int begin_cancelation_point(void) {
  int state = 0;
  if (__tls_enabled) {
    struct PosixThread *pt;
    if ((pt = _pthread_self())) {
      state = pt->pt_flags & PT_INCANCEL;
      pt->pt_flags |= PT_INCANCEL;
    }
  }
  return state;
}

void end_cancelation_point(int state) {
  if (__tls_enabled) {
    struct PosixThread *pt;
    if ((pt = _pthread_self())) {
      pt->pt_flags &= ~PT_INCANCEL;
      pt->pt_flags |= state;
    }
  }
}

void report_cancelation_point(int sysv_ordinal, int xnu_ordinal) {
  char bt[160];
  struct StackFrame *bp = __builtin_frame_address(0);
  kprintf("error: report_cancelation_point(%#x, %#x) %s\n", sysv_ordinal,
          xnu_ordinal, _DescribeBacktrace(bt, bp));
  __builtin_trap();
}

#endif /* MODE_DBG */
