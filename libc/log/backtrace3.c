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
#include "libc/calls/calls.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/kprintf.h"
#include "libc/intrin/weaken.h"
#include "libc/log/backtrace.internal.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/gc.internal.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/memtrack.internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"
#include "libc/thread/tls.h"

#define LIMIT 100

/**
 * Prints stack frames with symbols.
 *
 *     PrintBacktraceUsingSymbols(STDOUT_FILENO, NULL, GetSymbolTable());
 *
 * @param f is output stream
 * @param bp is rbp which can be NULL to detect automatically
 * @param st is open symbol table for current executable
 * @return -1 w/ errno if error happened
 */
dontinstrument dontasan int PrintBacktraceUsingSymbols(
    int fd, const struct StackFrame *bp, struct SymbolTable *st) {
  size_t gi;
  intptr_t addr;
  int i, symbol, addend;
  struct Garbages *garbage;
  const struct StackFrame *frame;
  (void)gi;
  if (!bp) bp = __builtin_frame_address(0);
  garbage = __tls_enabled ? __get_tls()->tib_garbages : 0;
  gi = garbage ? garbage->i : 0;
  for (i = 0, frame = bp; frame; frame = frame->next) {
    if (kisdangerous(frame)) {
      kprintf("<dangerous frame>\n");
      break;
    }
    if (++i == LIMIT) {
      kprintf("<truncated backtrace>\n");
      break;
    }
    addr = frame->addr;
#ifdef __x86_64__
    if (gi && addr == (intptr_t)_weaken(__gc)) {
      do {
        --gi;
      } while ((addr = garbage->p[gi].ret) == (intptr_t)_weaken(__gc));
    }
#endif
    if (addr) {
      if ((symbol = __get_symbol(st, addr)) != -1) {
        addend = addr - st->addr_base;
        addend -= st->symbols[symbol].x;
      } else {
        addend = 0;
      }
    } else {
      symbol = 0;
      addend = 0;
    }
    kprintf("%012lx %lx %s%+d\n", frame, addr, __get_symbol_name(st, symbol),
            addend);
  }
  return 0;
}
