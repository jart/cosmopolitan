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
#include "libc/dce.h"
#include "libc/intrin/likely.h"
#include "libc/str/str.h"

typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(16)));

/**
 * Searches for fixed-length substring in memory region.
 *
 * @param haystack is the region of memory to be searched
 * @param haystacklen is its character count
 * @param needle contains the memory for which we're searching
 * @param needlelen is its character count
 * @return pointer to first result or NULL if not found
 */
__vex void *memmem(const void *haystack, size_t haystacklen, const void *needle,
                   size_t needlelen) {
#if defined(__x86_64__) && !defined(__chibicc__)
  char c;
  xmm_t n;
  const xmm_t *v;
  unsigned i, k, m;
  const char *p, *q, *e;
  if (!needlelen) return (void *)haystack;
  if (UNLIKELY(needlelen > haystacklen)) return 0;
  q = needle;
  c = *q;
  n = (xmm_t){c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c};
  p = haystack;
  e = p + haystacklen;
  k = (uintptr_t)p & 15;
  v = (const xmm_t *)((uintptr_t)p & -16);
  m = __builtin_ia32_pmovmskb128(*v == n);
  m >>= k;
  m <<= k;
  for (;;) {
    while (!m) {
      ++v;
      if ((const char *)v >= e) return 0;
      m = __builtin_ia32_pmovmskb128(*v == n);
    }
    do {
      k = __builtin_ctzl(m);
      p = (const char *)v + k;
      if (UNLIKELY(p + needlelen > e)) return 0;
      for (i = 1;; ++i) {
        if (i == needlelen) return (/*unconst*/ char *)p;
        if (p[i] != q[i]) break;
      }
      m &= ~(1 << k);
    } while (m);
  }
#else
  size_t i, j;
  if (!needlelen) return (void *)haystack;
  if (needlelen > haystacklen) return 0;
  for (i = 0; i < haystacklen; ++i) {
    for (j = 0;; ++j) {
      if (j == needlelen) return (/*unconst*/ char *)haystack + i;
      if (i + j == haystacklen) break;
      if (((char *)haystack)[i + j] != ((char *)needle)[j]) break;
    }
  }
  return 0;
#endif
}
