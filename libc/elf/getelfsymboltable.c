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
#include "libc/elf/def.h"
#include "libc/elf/elf.h"
#include "libc/elf/scalar.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/shdr.h"
#include "libc/elf/struct/sym.h"

/**
 * Returns pointer to the elf section header for a symbol table.
 *
 * The easiest way to get the symbol table is:
 *
 *     Elf64_Xword i, n;
 *     Elf64_Sym *st = GetElfSymbols(map, size, SHT_SYMTAB, &n);
 *     for (i = 0; st && i < n; ++i) {
 *       // st[i] holds a symbol
 *     }
 *
 * This API is more verbose than the GetElfSymbols() shortcut, however
 * calling this the long way makes tricks like the following possible:
 *
 *     Elf64_Xword i, n;
 *     Elf64_Shdr *sh = GetElfSymbolTable(map, size, SHT_SYMTAB, &n);
 *     Elf64_Sym *st = GetElfSectionAddress(map, size, sh);
 *     if (st) {
 *       for (i = sh->sh_info; i < n; ++i) {
 *         // st[i] holds a non-local symbol
 *       }
 *     }
 *
 * Our code here only cares about `STB_GLOBAL` and `STB_WEAK` symbols
 * however `SHT_SYMTAB` usually has countless `STB_LOCAL` entries too
 * that must be skipped over. The trick is that the ELF spec requires
 * local symbols be ordered before global symbols, and that the index
 * dividing the two be stored to `sh_info`. So, if we start iterating
 * there, then we've cleverly avoided possibly dozens of page faults!
 *
 * @param elf points to the start of the executable image data
 * @param mapsize is the number of bytes past `elf` we can access
 * @param section_type is usually `SHT_SYMTAB` or `SHT_DYNSYM`
 * @param out_count optionally receives number of symbols
 * @return pointer to symbol table section header, otherwise null
 */
Elf64_Shdr *GetElfSymbolTable(const Elf64_Ehdr *elf,  //
                              size_t mapsize,         //
                              int section_type,       //
                              Elf64_Xword *out_count) {
  int i;
  Elf64_Shdr *shdr;
  for (i = elf->e_shnum; i > 0; --i) {
    if ((shdr = GetElfSectionHeaderAddress(elf, mapsize, i - 1)) &&  //
        shdr->sh_entsize == sizeof(Elf64_Sym) &&                     //
        shdr->sh_type == section_type) {
      if (out_count) {
        *out_count = shdr->sh_size / sizeof(Elf64_Sym);
      }
      return shdr;
    }
  }
  return 0;
}
