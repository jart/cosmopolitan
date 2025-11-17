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
 * Returns prefix length, consisting of chars not in reject.
 * a.k.a. Return index of first byte that's in charset.
 *
 * This algorithm is O(len(s)) when len(reject)≤512, otherwise an
 * O(len(s)*len(reject)) algorithm is used.
 *
 * @param reject is nul-terminated character set
 * @see strspn(), strtok_r()
 * @asyncsignalsafe
 */
size_t wcscspn(const wchar_t *s, const wchar_t *reject) {

  // handle simple cases
  if (!reject[0])
    return wcslen(s);
  if (!reject[1])
    return wcschrnul(s, reject[0]) - s;

  // allocate double two power stack memory
  // go quadratic if we need more than a page
  int p = 2 + bsrl(wcslen(reject) - 1);
  size_t n = 1ul << p;
  size_t need = sizeof(wchar_t) * n;
  if (need > 4096) {
    size_t i;
    for (i = 0; s[i]; ++i)
      if (wcschr(reject, s[i]))
        break;
    return i;
  }
  wchar_t *table = (wchar_t *)alloca(need);
  bzero(table, need);

  // populate hash set of characters in reject
  while (*reject) {
    wchar_t c = *reject++;
    unsigned h = hash(c, p);
    size_t k = 0;
    size_t step = 0;
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
      return i;
    if (!table[h])
      continue;
    size_t k, step = 1;
    do {
      k = (h + step * (step + 1) / 2) & (n - 1);
      if (table[k] == c)
        return i;
      step++;
    } while (table[k]);
  }
}
