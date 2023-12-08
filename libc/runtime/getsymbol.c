/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"

extern struct SymbolTable *__symtab;

/**
 * Returns low index into symbol table for address.
 *
 * @param t if null will be auto-populated only if already open
 * @return index or -1 if nothing found
 */
privileged int __get_symbol(struct SymbolTable *t, intptr_t a) {
  // we need privileged because:
  //   kprintf is privileged and it depends on this
  // we don't want function tracing because:
  //   function tracing depends on this function via kprintf
  unsigned l, m, r, n, k;
  if (!t && __symtab) {
    t = __symtab;
  }
  if (t) {
    l = 0;
    r = n = t->count;
    k = a - t->addr_base;
    while (l < r) {
      m = (l & r) + ((l ^ r) >> 1);  // floor((a+b)/2)
      if (t->symbols[m].y < k) {
        l = m + 1;
      } else {
        r = m;
      }
    }
    if (l < n && t->symbols[l].x <= k && k <= t->symbols[l].y) {
      return l;
    }
  }
  return -1;
}
