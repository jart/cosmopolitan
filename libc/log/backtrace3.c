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
#include "libc/alg/bisectcarleft.internal.h"
#include "libc/assert.h"
#include "libc/bits/weaken.h"
#include "libc/calls/calls.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/itoa.h"
#include "libc/log/backtrace.internal.h"
#include "libc/macros.h"
#include "libc/nexgen32e/gc.internal.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/str/str.h"

/**
 * Prints stack frames with symbols.
 *
 *   PrintBacktraceUsingSymbols(STDOUT_FILENO, NULL, GetSymbolTable());
 *
 * @param f is output stream
 * @param bp is rbp which can be NULL to detect automatically
 * @param st is open symbol table for current executable
 * @return -1 w/ errno if error happened
 */
int PrintBacktraceUsingSymbols(int fd, const struct StackFrame *bp,
                               struct SymbolTable *st) {
  char *p;
  size_t gi;
  intptr_t addr;
  int64_t addend;
  struct Garbages *garbage;
  char buf[256], ibuf[21];
  const struct Symbol *symbol;
  const struct StackFrame *frame;
  if (!st) return -1;
  if (!bp) bp = __builtin_frame_address(0);
  garbage = weaken(__garbage);
  gi = garbage ? garbage->i : 0;
  for (frame = bp; frame; frame = frame->next) {
    addr = frame->addr;
    if (addr == weakaddr("__gc")) {
      do {
        --gi;
      } while ((addr = garbage->p[gi].ret) == weakaddr("__gc"));
    }
    p = buf;
    p = mempcpy(p, ibuf, uint64toarray_fixed16((intptr_t)frame, ibuf, 48));
    *p++ = ' ';
    p = mempcpy(p, ibuf, uint64toarray_fixed16(addr, ibuf, 48));
    *p++ = ' ';
    if (st->count && ((intptr_t)addr >= (intptr_t)&_base &&
                      (intptr_t)addr <= (intptr_t)&_end)) {
      symbol = &st->symbols[bisectcarleft((const int32_t(*)[2])st->symbols,
                                          st->count, addr - st->addr_base - 1)];
      p = stpcpy(p, &st->name_base[symbol->name_rva]);
      addend = addr - st->addr_base - symbol->addr_rva;
      *p++ = addend >= 0 ? '+' : '-';
      if (addend) *p++ = '0', *p++ = 'x';
      p = mempcpy(p, ibuf, uint64toarray_radix16(ABS(addend), ibuf));
    } else {
      p = stpcpy(p, "UNKNOWN");
    }
    *p++ = '\r';
    *p++ = '\n';
    if (write(fd, buf, p - buf) == -1) {
      return -1;
    }
  }
  return 0;
}
