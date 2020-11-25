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
#include "libc/runtime/carsort.h"
#include "libc/runtime/internal.h"
#include "libc/runtime/runtime.h"
#include "libc/runtime/symbols.internal.h"
#include "libc/sysv/consts/map.h"
#include "libc/sysv/consts/prot.h"

/**
 * Maps debuggable binary into memory and indexes symbol addresses.
 *
 * @return object freeable with CloseSymbolTable(), or NULL w/ errno
 */
struct SymbolTable *OpenSymbolTable(const char *filename) {
  unsigned i, j;
  struct SymbolTable *t;
  const Elf64_Sym *symtab, *sym;
  t = MAP_FAILED;
  if (filename && (t = mapanon(BIGPAGESIZE)) != MAP_FAILED &&
      MapElfRead(filename, &t->mf) &&
      (t->name_base = GetElfStringTable(t->elf, t->elfsize)) != NULL &&
      (symtab = GetElfSymbolTable(t->elf, t->elfsize, &t->count)) &&
      sizeof(struct SymbolTable) + sizeof(struct Symbol) * t->count <
          (t->scratch = BIGPAGESIZE)) {
    GetElfVirtualAddressRange(t->elf, t->elfsize, &t->addr_base, &t->addr_end);
    for (j = i = 0; i < t->count; ++i) {
      sym = &symtab[i];
      if (IsElfSymbolContent(sym) &&
          (sym->st_value >= t->addr_base && sym->st_value <= t->addr_end)) {
        t->symbols[j].addr_rva = (unsigned)(sym->st_value - t->addr_base);
        t->symbols[j].name_rva = sym->st_name;
        j++;
      }
    }
    t->count = j;
    carsort1000(t->count, (void *)t->symbols);
  } else {
    CloseSymbolTable(&t);
  }
  return t == MAP_FAILED ? NULL : t;
}
