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
#include "libc/str/str.h"

/**
 * Returns `strtab + i` from elf string table.
 *
 * @param elf points to the start of the executable image data
 * @param mapsize is the number of bytes past `elf` we can access
 * @param strtab is double-nul string list from GetElfStringTable()
 *     which may be null, in which case only the `!i` name is valid
 * @param i is byte index into strtab where needed string starts or
 *     zero (no name) in which case empty string is always returned
 *     as a pointer to the read-only string literal, rather than in
 *     the elf image, since the elf spec permits an empty or absent
 *     string table section
 * @return a const nul-terminated string pointer, otherwise null if
 *     1. `i` was nonzero and `strtab` was null, or
 *     2. `strtab+i` wasn't inside `[elf,elf+mapsize)`, or
 *     3. a nul byte wasn't present within `[strtab+i,elf+mapsize)`, or
 *     4. an arithmetic overflow occurred
 */
char *GetElfString(const Elf64_Ehdr *elf,  // validated
                   size_t mapsize,         // validated
                   const char *strtab,     // validated
                   Elf64_Word i) {         // foreign
  const char *e;
  if (!i) return "";
  e = (const char *)elf;
  if (!strtab) return 0;
  if (i >= mapsize) return 0;
  if (strtab + i >= e + mapsize) return 0;
  if (!memchr(strtab + i, 0, (e + mapsize) - (strtab + i))) return 0;
  return (char *)strtab + i;
}
