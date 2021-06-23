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
#include "libc/str/str.h"

/**
 * Searches for fixed-length substring in memory region.
 *
 * @param haystack is the region of memory to be searched
 * @param haystacklen is its character count
 * @param needle contains the memory for which we're searching
 * @param needlelen is its character count
 * @return pointer to first result or NULL if not found
 */
void *memmem(const void *haystack, size_t haystacklen, const void *needle,
             size_t needlelen) {
  size_t i, j;
  const char *p;
  if (!needlelen) return haystack;
  if (needlelen <= haystacklen) {
    p = memchr(haystack, *(const char *)needle, haystacklen);
    if (needlelen == 1) return p;
    if (p) {
      haystacklen -= p - (const char *)haystack;
      haystack = p;
    }
    /* TODO: make not quadratic */
    for (i = 0; i < haystacklen; ++i) {
      for (j = 0;; ++j) {
        if (j == needlelen) return (/*unconst*/ char *)haystack + i;
        if (i + j == haystacklen) break;
        if (((char *)needle)[j] != ((char *)haystack)[i + j]) break;
      }
    }
  }
  return NULL;
}
