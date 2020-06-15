/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
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
#ifndef COSMOPOLITAN_LIBC_SYMBOLS_H_
#define COSMOPOLITAN_LIBC_SYMBOLS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
#include "libc/elf/elf.h"
#include "libc/runtime/ezmap.h"
COSMOPOLITAN_C_START_

struct Symbol {
  unsigned addr_rva;
  unsigned name_rva;
};

struct SymbolTable {
  union {
    struct MappedFile mf;
    struct {
      int64_t fd;
      struct Elf64_Ehdr *elf;
      size_t elfsize;
    };
  };
  size_t scratch;
  size_t count;
  intptr_t addr_base;
  intptr_t addr_end;
  const char *name_base;
  struct Symbol symbols[];
};

struct SymbolTable *getsymboltable(void);
const char *findcombinary(void);
const char *finddebugbinary(void);
struct SymbolTable *opensymboltable(const char *) nodiscard;
int closesymboltable(struct SymbolTable **);
const struct Symbol *bisectsymbol(struct SymbolTable *, intptr_t, int64_t *);
const char *getsymbolname(struct SymbolTable *, const struct Symbol *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYMBOLS_H_ */
