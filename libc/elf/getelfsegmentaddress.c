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
#include "libc/elf/struct/phdr.h"
#include "libc/stdckdint.h"

/**
 * Returns pointer to ELF segment file content.
 *
 * This function computes `elf + p_offset` with safety checks.
 *
 * @param elf points to the start of the executable image data
 * @param mapsize is the number of bytes of `elf` we can access
 * @param phdr is from GetElfProgramHeaderAddress(), or null
 * @return pointer to segment data within image, or null if
 *     1. `phdr` was null, or
 *     2. `p_filesz` was zero, or
 *     3. content wasn't contained within `[elf,elf+mapsize)`, or
 *     4. an arithmetic overflow occurred
 */
void *GetElfSegmentAddress(const Elf64_Ehdr *elf,     // validated
                           size_t mapsize,            // validated
                           const Elf64_Phdr *phdr) {  // foreign
  Elf64_Off last;
  if (!phdr) return 0;
  if (phdr->p_filesz <= 0) return 0;
  if (ckd_add(&last, phdr->p_offset, phdr->p_filesz)) return 0;
  if (last > mapsize) return 0;
  return (char *)elf + phdr->p_offset;
}
