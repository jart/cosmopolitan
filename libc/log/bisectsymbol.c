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
#include "libc/assert.h"
#include "libc/runtime/symbols.h"

/**
 * Finds symbol associated with address.
 * @param symbol table pointer (null propagating)
 * @return symbol address or NULL if not found
 */
const struct Symbol *bisectsymbol(struct SymbolTable *t,
                                  intptr_t virtual_address,
                                  int64_t *opt_out_addend) {
  const struct Symbol *symbol = NULL;
  int64_t addend = (intptr_t)virtual_address;
  if (t && t->count) {
    unsigned key = (unsigned)((intptr_t)virtual_address - t->addr_base - 1);
    unsigned i = bisectcarleft((const int32_t(*)[2])t->symbols, t->count, key);
    assert(0 <= i && i < t->count);
    symbol = &t->symbols[i];
    addend = (int64_t)key - (int64_t)symbol->addr_rva;
  }
  if (opt_out_addend) *opt_out_addend = addend;
  return symbol;
}
