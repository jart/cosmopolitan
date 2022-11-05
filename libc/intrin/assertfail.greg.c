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
#include "libc/assert.h"
#include "libc/atomic.h"
#include "libc/calls/state.internal.h"
#include "libc/calls/syscall-sysv.internal.h"
#include "libc/intrin/atomic.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/weaken.h"
#include "libc/log/backtrace.internal.h"
#include "libc/log/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/thread/thread.h"

relegated void __assert_fail(const char *expr, const char *file, int line) {
  int me, owner;
  static atomic_int once;
  if (!__assert_disable) {
    strace_enabled(-1);
    ftrace_enabled(-1);
    owner = 0;
    me = sys_gettid();
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, 0);
    kprintf("%s:%d: assert(%s) failed (tid %d)\n", file, line, expr, me);
    if (__vforked ||
        atomic_compare_exchange_strong_explicit(
            &once, &owner, me, memory_order_relaxed, memory_order_relaxed)) {
      __restore_tty();
      if (_weaken(ShowBacktrace)) {
        _weaken(ShowBacktrace)(2, __builtin_frame_address(0));
      } else if (_weaken(PrintBacktraceUsingSymbols) &&
                 _weaken(GetSymbolTable)) {
        _weaken(PrintBacktraceUsingSymbols)(2, __builtin_frame_address(0),
                                            _weaken(GetSymbolTable)());
      } else {
        kprintf("can't backtrace b/c `ShowCrashReports` not linked\n");
      }
      _Exitr(23);
    } else if (owner == me) {
      kprintf("assert failed while failing\n");
      _Exitr(24);
    } else {
      _Exit1(25);
    }
  }
}
