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
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"

#define PMOVMSKB(x) __builtin_ia32_pmovmskb128(x)

typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(1)));

#if defined(__x86_64__) && !defined(__chibicc__)

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

_Microarchitecture("avx") static int memcmp_avx(const unsigned char *p,
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

#endif /* __x86_64__ */

/**
 * Compares memory byte by byte.
 *
 *     memcmp n=0                           2 nanoseconds
 *     memcmp n=1                           2 ns/byte            357 mb/s
 *     memcmp n=2                           1 ns/byte            530 mb/s
 *     memcmp n=3                           1 ns/byte            631 mb/s
 *     𝗺𝗲𝗺𝗰𝗺𝗽 n=4                           1 ns/byte            849 mb/s
 *     memcmp n=5                         816 ps/byte          1,195 mb/s
 *     memcmp n=6                         888 ps/byte          1,098 mb/s
 *     memcmp n=7                         829 ps/byte          1,176 mb/s
 *     𝗺𝗲𝗺𝗰𝗺𝗽 n=8                         773 ps/byte          1,261 mb/s
 *     memcmp n=9                         629 ps/byte          1,551 mb/s
 *     memcmp n=15                        540 ps/byte          1,805 mb/s
 *     𝗺𝗲𝗺𝗰𝗺𝗽 n=16                        211 ps/byte          4,623 mb/s
 *     memcmp n=17                        268 ps/byte          3,633 mb/s
 *     memcmp n=31                        277 ps/byte          3,524 mb/s
 *     memcmp n=32                        153 ps/byte          6,351 mb/s
 *     memcmp n=33                        179 ps/byte          5,431 mb/s
 *     memcmp n=79                        148 ps/byte          6,576 mb/s
 *     𝗺𝗲𝗺𝗰𝗺𝗽 n=80                         81 ps/byte             11 GB/s
 *     memcmp n=128                        76 ps/byte             12 GB/s
 *     memcmp n=256                        60 ps/byte             15 GB/s
 *     memcmp n=16384                      51 ps/byte             18 GB/s
 *     memcmp n=32768                      51 ps/byte             18 GB/s
 *     memcmp n=131072                     52 ps/byte             18 GB/s
 *
 * @return an integer that's (1) equal to zero if `a` is equal to `b`,
 *     (2) less than zero if `a` is less than `b`, or (3) greater than
 *     zero if `a` is greater than `b`
 * @asyncsignalsafe
 */
int memcmp(const void *a, const void *b, size_t n) {
  int c;
#if defined(__x86_64__) && !defined(__chibicc__)
  unsigned u;
  uint32_t k, i, j;
  uint64_t w, x, y;
#endif
  const unsigned char *p, *q;
  if ((p = a) == (q = b) || !n) return 0;
  if ((c = *p - *q)) return c;
#if defined(__x86_64__) && !defined(__chibicc__)
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
#endif /* __x86_64__ */
  for (; n; ++p, ++q, --n) {
    if ((c = *p - *q)) {
      return c;
    }
  }
  return 0;
}
