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
