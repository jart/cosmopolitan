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
#include "libc/elf/def.h"
#include "libc/elf/elf.h"
#include "libc/elf/scalar.h"
#include "libc/elf/struct/shdr.h"

/**
 * Returns section header object at `elf.section[i]`.
 *
 * @param elf points to the start of the executable image data
 * @param mapsize is the number of bytes past `elf` we can access
 * @param i is the index of the section header
 * @return pointer to section header within image, or null if
 *     1. `i` was a magic number, i.e. `i >= SHN_LORESERVE`, or
 *     2. `e_shoff` was zero (image has no section headers), or
 *     3. `e_shentsize` had fewer than the mandatory 60 bytes, or
 *     4. section header wasn't contained by `[elf,elf+mapsize)`, or
 *     5. an arithmetic overflow occurred
 */
Elf64_Shdr *GetElfSectionHeaderAddress(const Elf64_Ehdr *elf,  //
                                       size_t mapsize,         //
                                       Elf64_Half i) {         //
  Elf64_Off off;
  if (i >= SHN_LORESERVE) return 0;
  if (i >= elf->e_shnum) return 0;
  if (elf->e_shoff <= 0) return 0;
  if (elf->e_shoff >= mapsize) return 0;
  if (elf->e_shentsize < sizeof(Elf64_Shdr)) return 0;
  if ((off = elf->e_shoff + (unsigned)i * elf->e_shentsize) > mapsize) return 0;
  return (Elf64_Shdr *)((char *)elf + off);
}
