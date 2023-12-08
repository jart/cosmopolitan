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
#include "libc/stdckdint.h"

/**
 * Returns pointer to ELF section file content.
 *
 * This function computes `elf + sh_offset` with safety checks.
 *
 * @param elf points to the start of the executable image data
 * @param mapsize is the number of bytes of `elf` we can access
 * @param shdr is from GetElfSectionHeaderAddress(), or null
 * @return pointer to section data within image, or null if
 *     1. `shdr` was null, or
 *     2. content wasn't contained within `[elf,elf+mapsize)`, or
 *     3. an arithmetic overflow occurred
 */
void *GetElfSectionAddress(const Elf64_Ehdr *elf,     // validated
                           size_t mapsize,            // validated
                           const Elf64_Shdr *shdr) {  // foreign
  Elf64_Off last;
  if (!shdr) return 0;
  if (!shdr->sh_size) return (void *)elf;
  if (shdr->sh_type == SHT_NOBITS) return 0;
  if (ckd_add(&last, shdr->sh_offset, shdr->sh_size)) return 0;
  if (last > mapsize) return 0;
  return (char *)elf + shdr->sh_offset;
}
