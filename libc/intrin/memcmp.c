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

#define PMOVMSKB(x) __builtin_ia32_pmovmskb128(x)

typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(1)));

static dontinline antiquity int memcmp_sse(const unsigned char *p,
                                           const unsigned char *q, size_t n) {
  unsigned u;
  if (n > 32) {
    while (n > 16 + 16) {
      if (!(u = PMOVMSKB(*(xmm_t *)p == *(xmm_t *)q) ^ 0xffff)) {
        n -= 16;
        p += 16;
        q += 16;
      } else {
        u = __builtin_ctzl(u);
        return p[u] - q[u];
      }
    }
  }
  if (!(u = PMOVMSKB(*(xmm_t *)p == *(xmm_t *)q) ^ 0xffff)) {
    if (!(u = PMOVMSKB(*(xmm_t *)(p + n - 16) == *(xmm_t *)(q + n - 16)) ^
              0xffff)) {
      return 0;
    } else {
      u = __builtin_ctzl(u);
      return p[n - 16 + u] - q[n - 16 + u];
    }
  } else {
    u = __builtin_ctzl(u);
    return p[u] - q[u];
  }
}

microarchitecture("avx") static int memcmp_avx(const unsigned char *p,
                                               const unsigned char *q,
                                               size_t n) {
  uint64_t w;
  unsigned u;
  if (n > 32) {
    while (n >= 16 + 64) {
      w = (uint64_t)PMOVMSKB(((xmm_t *)p)[0] == ((xmm_t *)q)[0]) << 000 |
          (uint64_t)PMOVMSKB(((xmm_t *)p)[1] == ((xmm_t *)q)[1]) << 020 |
          (uint64_t)PMOVMSKB(((xmm_t *)p)[2] == ((xmm_t *)q)[2]) << 040 |
          (uint64_t)PMOVMSKB(((xmm_t *)p)[3] == ((xmm_t *)q)[3]) << 060;
      if (w == -1) {
        n -= 64;
        p += 64;
        q += 64;
      } else {
        w = __builtin_ctzll(w ^ -1);
        return p[w] - q[w];
      }
    }
    while (n > 16 + 16) {
      if (!(u = PMOVMSKB(*(xmm_t *)p == *(xmm_t *)q) ^ 0xffff)) {
        n -= 16;
        p += 16;
        q += 16;
      } else {
        u = __builtin_ctzl(u);
        return p[u] - q[u];
      }
    }
  }
  if (!(u = PMOVMSKB(*(xmm_t *)p == *(xmm_t *)q) ^ 0xffff)) {
    if (!(u = PMOVMSKB(*(xmm_t *)(p + n - 16) == *(xmm_t *)(q + n - 16)) ^
              0xffff)) {
      return 0;
    } else {
      u = __builtin_ctzl(u);
      return p[n - 16 + u] - q[n - 16 + u];
    }
  } else {
    u = __builtin_ctzl(u);
    return p[u] - q[u];
  }
}

/**
 * Compares memory byte by byte.
 *
 *     memcmp n=0                         992 picoseconds
 *     memcmp n=1                           1 ns/byte            738 mb/s
 *     memcmp n=2                         661 ps/byte          1,476 mb/s
 *     memcmp n=3                         551 ps/byte          1,771 mb/s
 *     memcmp n=4                         248 ps/byte          3,936 mb/s
 *     memcmp n=5                         198 ps/byte          4,920 mb/s
 *     memcmp n=6                         165 ps/byte          5,904 mb/s
 *     memcmp n=7                         141 ps/byte          6,889 mb/s
 *     memcmp n=8                         124 ps/byte          7,873 mb/s
 *     memcmp n=9                         110 ps/byte          8,857 mb/s
 *     memcmp n=15                         44 ps/byte         22,143 mb/s
 *     memcmp n=16                         41 ps/byte         23,619 mb/s
 *     memcmp n=17                         77 ps/byte         12,547 mb/s
 *     memcmp n=31                         42 ps/byte         22,881 mb/s
 *     memcmp n=32                         41 ps/byte         23,619 mb/s
 *     memcmp n=33                         60 ps/byte         16,238 mb/s
 *     memcmp n=80                         53 ps/byte         18,169 mb/s
 *     memcmp n=128                        38 ps/byte         25,194 mb/s
 *     memcmp n=256                        32 ps/byte         30,233 mb/s
 *     memcmp n=16384                      27 ps/byte         35,885 mb/s
 *     memcmp n=32768                      29 ps/byte         32,851 mb/s
 *     memcmp n=131072                     33 ps/byte         28,983 mb/s
 *
 * @return unsigned char subtraction at stop index
 * @asyncsignalsafe
 */
int memcmp(const void *a, const void *b, size_t n) {
  int c;
  unsigned u;
  uint32_t k, i, j;
  uint64_t w, x, y;
  const unsigned char *p, *q;
  if ((p = a) == (q = b) || !n) return 0;
  if ((c = *p - *q)) return c;
  if (!IsTiny()) {
    if (n <= 16) {
      if (n >= 8) {
        if (!(w = (x = ((uint64_t)p[0] << 000 | (uint64_t)p[1] << 010 |
                        (uint64_t)p[2] << 020 | (uint64_t)p[3] << 030 |
                        (uint64_t)p[4] << 040 | (uint64_t)p[5] << 050 |
                        (uint64_t)p[6] << 060 | (uint64_t)p[7] << 070)) ^
                  (y = ((uint64_t)q[0] << 000 | (uint64_t)q[1] << 010 |
                        (uint64_t)q[2] << 020 | (uint64_t)q[3] << 030 |
                        (uint64_t)q[4] << 040 | (uint64_t)q[5] << 050 |
                        (uint64_t)q[6] << 060 | (uint64_t)q[7] << 070)))) {
          p += n - 8;
          q += n - 8;
          if (!(w = (x = ((uint64_t)p[0] << 000 | (uint64_t)p[1] << 010 |
                          (uint64_t)p[2] << 020 | (uint64_t)p[3] << 030 |
                          (uint64_t)p[4] << 040 | (uint64_t)p[5] << 050 |
                          (uint64_t)p[6] << 060 | (uint64_t)p[7] << 070)) ^
                    (y = ((uint64_t)q[0] << 000 | (uint64_t)q[1] << 010 |
                          (uint64_t)q[2] << 020 | (uint64_t)q[3] << 030 |
                          (uint64_t)q[4] << 040 | (uint64_t)q[5] << 050 |
                          (uint64_t)q[6] << 060 | (uint64_t)q[7] << 070)))) {
            return 0;
          }
        }
        u = __builtin_ctzll(w);
        u = u & -8;
        return ((x >> u) & 255) - ((y >> u) & 255);
      } else if (n >= 4) {
        if (!(k = (i = ((uint32_t)p[0] << 000 | (uint32_t)p[1] << 010 |
                        (uint32_t)p[2] << 020 | (uint32_t)p[3] << 030)) ^
                  (j = ((uint32_t)q[0] << 000 | (uint32_t)q[1] << 010 |
                        (uint32_t)q[2] << 020 | (uint32_t)q[3] << 030)))) {
          p += n - 4;
          q += n - 4;
          if (!(k = (i = ((uint32_t)p[0] << 000 | (uint32_t)p[1] << 010 |
                          (uint32_t)p[2] << 020 | (uint32_t)p[3] << 030)) ^
                    (j = ((uint32_t)q[0] << 000 | (uint32_t)q[1] << 010 |
                          (uint32_t)q[2] << 020 | (uint32_t)q[3] << 030)))) {
            return 0;
          }
        }
        u = __builtin_ctzl(k);
        u = u & -8;
        return ((i >> u) & 255) - ((j >> u) & 255);
      }
    } else if (LIKELY(X86_HAVE(AVX))) {
      return memcmp_avx(p, q, n);
    } else {
      return memcmp_sse(p, q, n);
    }
  }
  for (; n; ++p, ++q, --n) {
    if ((c = *p - *q)) {
      return c;
    }
  }
  return 0;
}
