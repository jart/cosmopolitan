/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/math.h"
#include "libc/stdio/rand.h"
#include "libc/str/str.h"

/**
 * Returns Shannon entropy of array.
 *
 * This gives you an idea of the density of information. Cryptographic
 * random should be in the ballpark of 7.9 whereas plaintext will be
 * more like 4.5.
 *
 * @param p is treated as binary octets
 * @param n should be at least 1000
 * @return number between 0 and 8
 */
double MeasureEntropy(const char *p, size_t n) {
  size_t i;
  double e, x;
  long h[256];
  e = 0;
  if (n) {
    bzero(h, sizeof(h));
    for (i = 0; i < n; ++i) {
      ++h[p[i] & 255];
    }
    for (i = 0; i < 256; i++) {
      if (h[i]) {
        x = h[i];
        x /= n;
        e += x * log(x);
      }
    }
    e = -(e / M_LN2);
  }
  return e;
}
