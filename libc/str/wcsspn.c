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
#include "libc/intrin/bsr.h"
#include "libc/limits.h"
#include "libc/mem/alloca.h"
#include "libc/str/str.h"

static inline unsigned hash(unsigned x, int p) {
  /* Knuth multiplicative hashing. (Sorting & Searching, p. 510).
     0x9e3779b1 is the closest prime number to (2**32)*phi, where
     phi is defined as (sqrt(5)-1)/2. */
  return (x * 2654435761u) >> (sizeof(unsigned) * CHAR_BIT - p);
}

/**
 * Returns prefix length, consisting of chars in accept.
 *
 * This algorithm is O(len(s)) when len(accept)≤512, otherwise an
 * O(len(s)*len(accept)) algorithm is used.
 *
 * @param accept is nul-terminated character set
 * @see strcspn(), strtok_r()
 * @asyncsignalsafe
 */
size_t wcsspn(const wchar_t *s, const wchar_t *accept) {

  // handle simple cases
  if (!accept[0])
    return 0;
  if (!accept[1])
    for (size_t i = 0;; i++)
      if (s[i] != accept[0])
        return i;

  // allocate double two power stack memory
  // go quadratic if we need more than a page
  int p = 2 + bsrl(wcslen(accept) - 1);
  size_t n = 1ul << p;
  size_t need = sizeof(wchar_t) * n;
  if (need > 4096) {
    size_t i;
    for (i = 0; s[i]; ++i)
      if (!wcschr(accept, s[i]))
        break;
    return i;
  }
  wchar_t *table = (wchar_t *)alloca(need);
  bzero(table, need);

  // populate hash set of characters in accept
  while (*accept) {
    wchar_t c = *accept++;
    unsigned h = hash(c, p);
    size_t k, step = 0;
    do {
      k = (h + step * (step + 1) / 2) & (n - 1);
      step++;
    } while (table[k] && table[k] != c);
    table[k] = c;
  }

  // find prefix length
  for (size_t i = 0;; i++) {
    wchar_t c = s[i];
    if (!c)
      return i;
    unsigned h = hash(c, p);
    if (table[h] == c)
      continue;
    if (!table[h])
      return i;
    size_t k, step = 1;
    do {
      k = (h + step * (step + 1) / 2) & (n - 1);
      if (!table[k])
        return i;
      step++;
    } while (table[k] != c);
  }
}
