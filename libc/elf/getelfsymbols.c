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
#include "libc/elf/elf.h"
#include "libc/elf/scalar.h"
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/sym.h"

/**
 * Returns pointer to array of elf symbols.
 *
 * This is a shortcut composing GetElfSymbolTable() and
 * GetElfSectionAddress(), that can be used as follows:
 *
 *     Elf64_Xword i, n;
 *     Elf64_Sym *st = GetElfSymbols(map, size, SHT_SYMTAB, &n);
 *     for (i = 0; st && i < n; ++i) {
 *       // st[i] holds a symbol
 *     }
 *
 * The above code will iterate over the relocatable and/or
 * statically-linked symbols defined by an ELF image.
 *
 * @param elf points to the start of the executable image data
 * @param mapsize is the number of bytes past `elf` we can access
 * @param section_type is usually `SHT_SYMTAB` or `SHT_DYNSYM`
 * @param out_count optionally receives number of symbols
 * @return pointer to array of elf symbol array, otherwise null
 */
Elf64_Sym *GetElfSymbols(const Elf64_Ehdr *elf,  //
                         size_t mapsize,         //
                         int section_type,       //
                         Elf64_Xword *out_count) {
  Elf64_Sym *syms;
  Elf64_Xword count;
  if ((syms = GetElfSectionAddress(
           elf, mapsize,
           GetElfSymbolTable(elf, mapsize, section_type, &count))) &&
      out_count) {
    *out_count = count;
  }
  return syms;
}
