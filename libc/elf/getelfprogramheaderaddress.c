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
#include "libc/elf/struct/phdr.h"

/**
 * Returns program header at `elf.phdr[i]`.
 *
 * @param elf points to the start of the executable image
 * @param mapsize is the number of bytes past `elf` we can access
 * @param i is the program header index, starting at zero
 * @return program header pointer, or null on error
 */
Elf64_Phdr *GetElfProgramHeaderAddress(const Elf64_Ehdr *elf,  //
                                       size_t mapsize,         //
                                       Elf64_Half i) {         //
  Elf64_Off off;
  if (i >= elf->e_phnum) return 0;
  if (elf->e_phoff <= 0) return 0;
  if (elf->e_phoff >= mapsize) return 0;
  if (elf->e_phentsize < sizeof(Elf64_Phdr)) return 0;
  if ((off = elf->e_phoff + (unsigned)i * elf->e_phentsize) > mapsize) return 0;
  return (Elf64_Phdr *)((char *)elf + off);
}
