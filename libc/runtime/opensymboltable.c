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
#include "libc/alg/alg.h"
#include "libc/calls/calls.h"
#include "libc/elf/def.h"
#include "libc/elf/elf.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

/**
 * Maps debuggable binary into memory and indexes symbol addresses.
 *
 * @return object freeable with closesymboltable(), or NULL w/ errno
 */
struct SymbolTable *opensymboltable(const char *filename) {
  struct SymbolTable *t = MAP_FAILED;
  const Elf64_Sym *symtab;
  if (filename && (t = mapanon(BIGPAGESIZE)) != MAP_FAILED &&
      mapelfread(filename, &t->mf) &&
      (t->name_base = getelfstringtable(t->elf, t->elfsize)) != NULL &&
      (symtab = getelfsymboltable(t->elf, t->elfsize, &t->count)) &&
      sizeof(struct SymbolTable) + sizeof(struct Symbol) * t->count <
          (t->scratch = BIGPAGESIZE)) {
    unsigned j = 0;
    getelfvirtualaddressrange(t->elf, t->elfsize, &t->addr_base, &t->addr_end);
    for (unsigned i = 0; i < t->count; ++i) {
      const Elf64_Sym *sym = &symtab[i];
      if (iselfsymbolcontent(sym) &&
          (sym->st_value >= t->addr_base && sym->st_value <= t->addr_end)) {
        t->symbols[j].addr_rva = (unsigned)(sym->st_value - t->addr_base);
        t->symbols[j].name_rva = sym->st_name;
        j++;
      }
    }
    t->count = j;
    heapsortcar((int32_t(*)[2])t->symbols, t->count);
  } else {
    closesymboltable(&t);
  }
  return t == MAP_FAILED ? NULL : t;
}
