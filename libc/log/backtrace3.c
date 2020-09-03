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
#include "libc/alg/bisectcarleft.h"
#include "libc/bits/weaken.h"
#include "libc/fmt/fmt.h"
#include "libc/log/backtrace.h"
#include "libc/macros.h"
#include "libc/nexgen32e/gc.h"
#include "libc/nexgen32e/stackframe.h"
#include "libc/runtime/symbols.h"
#include "libc/stdio/stdio.h"

static char *FormatAddress(FILE *f, const struct SymbolTable *st, intptr_t addr,
                           char *out, unsigned size, bool symbolic) {
  int64_t addend;
  const char *name;
  const struct Symbol *symbol;
  if (st->count && ((intptr_t)addr >= (intptr_t)&_base &&
                    (intptr_t)addr <= (intptr_t)&_end && symbolic)) {
    symbol = &st->symbols[bisectcarleft((const int32_t(*)[2])st->symbols,
                                        st->count, addr - st->addr_base - 1)];
    addend = addr - st->addr_base - symbol->addr_rva;
    name = &st->name_base[symbol->name_rva];
    snprintf(out, size, "%s%c%#x", name, addend >= 0 ? '+' : '-', ABS(addend));
  } else {
    snprintf(out, size, "%p", addr);
  }
  return out;
}

int PrintBacktraceUsingSymbols(FILE *f, const struct StackFrame *bp,
                               struct SymbolTable *symbols) {
  size_t gi;
  char buf[256];
  intptr_t addr;
  struct Garbages *garbage;
  const struct StackFrame *frame;
  if (!symbols) return -1;
  garbage = weaken(g_garbage);
  gi = garbage ? garbage->i : 0;
  for (frame = bp; frame; frame = frame->next) {
    addr = frame->addr;
    if (addr == weakaddr("CollectGarbage")) {
      do {
        --gi;
      } while ((addr = garbage->p[gi].ret) == weakaddr("CollectGarbage"));
    }
    fprintf(f, "%p %p %s\n", frame, addr,
            FormatAddress(f, symbols, addr, buf, sizeof(buf), true));
  }
  return 0;
}
