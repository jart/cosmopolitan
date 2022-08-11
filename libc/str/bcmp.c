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
#include "libc/intrin/likely.h"
#include "libc/dce.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"

typedef uint64_t xmm_t __attribute__((__vector_size__(16), __aligned__(1)));

static dontinline antiquity int bcmp_sse(const char *p, const char *q,
                                         size_t n) {
  xmm_t a;
  while (n > 32) {
    a = *(const xmm_t *)p ^ *(const xmm_t *)q;
    if (a[0] | a[1]) return 1;
    p += 16;
    q += 16;
    n -= 16;
  }
  a = *(const xmm_t *)p ^ *(const xmm_t *)q |
      *(const xmm_t *)(p + n - 16) ^ *(const xmm_t *)(q + n - 16);
  return !!(a[0] | a[1]);
}

microarchitecture("avx") static int bcmp_avx(const char *p, const char *q,
                                             size_t n) {
  xmm_t a, b, c, d;
  if (n > 32) {
    if (n >= 16 + 64) {
      do {
        a = ((const xmm_t *)p)[0] ^ ((const xmm_t *)q)[0];
        b = ((const xmm_t *)p)[1] ^ ((const xmm_t *)q)[1];
        c = ((const xmm_t *)p)[2] ^ ((const xmm_t *)q)[2];
        d = ((const xmm_t *)p)[3] ^ ((const xmm_t *)q)[3];
        a = a | b | c | d;
        if (a[0] | a[1]) return 1;
        p += 64;
        q += 64;
        n -= 64;
      } while (n >= 16 + 64);
    }
    while (n > 16 + 16) {
      a = *(const xmm_t *)p ^ *(const xmm_t *)q;
      if (a[0] | a[1]) return 1;
      p += 16;
      q += 16;
      n -= 16;
    }
  }
  a = *(const xmm_t *)p ^ *(const xmm_t *)q |
      *(const xmm_t *)(p + n - 16) ^ *(const xmm_t *)(q + n - 16);
  return !!(a[0] | a[1]);
}

/**
 * Tests inequality of first 𝑛 bytes of 𝑝 and 𝑞.
 *
 *     bcmp n=0                           992 picoseconds
 *     bcmp n=1                           992 ps/byte            984 mb/s
 *     bcmp n=2                           661 ps/byte          1,476 mb/s
 *     bcmp n=3                           441 ps/byte          2,214 mb/s
 *     bcmp n=4                           330 ps/byte          2,952 mb/s
 *     bcmp n=5                           264 ps/byte          3,690 mb/s
 *     bcmp n=6                           165 ps/byte          5,905 mb/s
 *     bcmp n=7                           189 ps/byte          5,166 mb/s
 *     bcmp n=8                           124 ps/byte          7,873 mb/s
 *     bcmp n=9                           183 ps/byte          5,314 mb/s
 *     bcmp n=15                          110 ps/byte          8,857 mb/s
 *     bcmp n=16                           62 ps/byte         15,746 mb/s
 *     bcmp n=17                          175 ps/byte          5,577 mb/s
 *     bcmp n=31                           96 ps/byte         10,169 mb/s
 *     bcmp n=32                           93 ps/byte         10,497 mb/s
 *     bcmp n=33                           80 ps/byte         12,179 mb/s
 *     bcmp n=80                           37 ps/byte         26,244 mb/s
 *     bcmp n=128                          36 ps/byte         26,994 mb/s
 *     bcmp n=256                          27 ps/byte         35,992 mb/s
 *     bcmp n=16384                        19 ps/byte         49,411 mb/s
 *     bcmp n=32768                        27 ps/byte         34,914 mb/s
 *     bcmp n=131072                       30 ps/byte         32,303 mb/s
 *
 * @return 0 if a and b have equal contents, otherwise nonzero
 * @see timingsafe_bcmp()
 * @asyncsignalsafe
 */
int bcmp(const void *a, const void *b, size_t n) {
  int c;
  unsigned u;
  uint32_t i, j;
  uint64_t x, y;
  const char *p, *q;
  if ((p = a) == (q = b)) return 0;
  if (!IsTiny()) {
    if (n <= 16) {
      if (n >= 8) {
        __builtin_memcpy(&x, p, 8);
        __builtin_memcpy(&y, q, 8);
        if (x ^ y) return 1;
        __builtin_memcpy(&x, p + n - 8, 8);
        __builtin_memcpy(&y, q + n - 8, 8);
        return !!(x ^ y);
      } else if (n >= 4) {
        __builtin_memcpy(&i, p, 4);
        __builtin_memcpy(&j, q, 4);
        if (i ^ j) return 1;
        __builtin_memcpy(&i, p + n - 4, 4);
        __builtin_memcpy(&j, q + n - 4, 4);
        return !!(i ^ j);
      }
    } else if (LIKELY(X86_HAVE(AVX))) {
      return bcmp_avx(p, q, n);
    } else {
      return bcmp_sse(p, q, n);
    }
  }
  while (n--) {
    if ((c = p[n] ^ q[n])) {
      return c;
    }
  }
  return 0;
}
