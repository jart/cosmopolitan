/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/intrin/likely.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"

typedef uint64_t xmm_t __attribute__((__vector_size__(16), __aligned__(1)));

noasan static dontinline antiquity unsigned timingsafe_bcmp_sse(const char *p,
                                                              const char *q,
                                                              size_t n) {
  uint64_t w;
  xmm_t a = {0};
  while (n > 16 + 16) {
    a |= *(const xmm_t *)p ^ *(const xmm_t *)q;
    p += 16;
    q += 16;
    n -= 16;
  }
  a |= *(const xmm_t *)p ^ *(const xmm_t *)q;
  a |= *(const xmm_t *)(p + n - 16) ^ *(const xmm_t *)(q + n - 16);
  w = a[0] | a[1];
  return w | w >> 32;
}

noasan static microarchitecture("avx") int timingsafe_bcmp_avx(const char *p,
                                                               const char *q,
                                                               size_t n) {
  uint64_t w;
  xmm_t a = {0};
  if (n > 32) {
    if (n >= 16 + 64) {
      xmm_t b = {0};
      xmm_t c = {0};
      xmm_t d = {0};
      do {
        a |= ((const xmm_t *)p)[0] ^ ((const xmm_t *)q)[0];
        b |= ((const xmm_t *)p)[1] ^ ((const xmm_t *)q)[1];
        c |= ((const xmm_t *)p)[2] ^ ((const xmm_t *)q)[2];
        d |= ((const xmm_t *)p)[3] ^ ((const xmm_t *)q)[3];
        p += 64;
        q += 64;
        n -= 64;
      } while (n >= 16 + 64);
      a = a | b | c | d;
    }
    while (n > 32) {
      a |= *(const xmm_t *)p ^ *(const xmm_t *)q;
      p += 16;
      q += 16;
      n -= 16;
    }
  }
  a |= *(const xmm_t *)p ^ *(const xmm_t *)q;
  a |= *(const xmm_t *)(p + n - 16) ^ *(const xmm_t *)(q + n - 16);
  w = a[0] | a[1];
  return w | w >> 32;
}

/**
 * Tests inequality of first 𝑛 bytes of 𝑝 and 𝑞.
 *
 * The following expression:
 *
 *     !!timingsafe_bcmp(p, q, n)
 *
 * Is functionally equivalent to:
 *
 *     !!memcmp(p, q, n)
 *
 * This function is faster than memcmp() and bcmp() when byte sequences
 * are assumed to always be the same; that makes it best for assertions
 * or hash table lookups, assuming 𝑛 is variable (since no gcc builtin)
 *
 *     timingsafe_bcmp n=0                992 picoseconds
 *     timingsafe_bcmp n=1                  1 ns/byte            738 mb/s
 *     timingsafe_bcmp n=2                826 ps/byte          1,181 mb/s
 *     timingsafe_bcmp n=3                661 ps/byte          1,476 mb/s
 *     timingsafe_bcmp n=4                330 ps/byte          2,952 mb/s
 *     timingsafe_bcmp n=5                264 ps/byte          3,690 mb/s
 *     timingsafe_bcmp n=6                220 ps/byte          4,428 mb/s
 *     timingsafe_bcmp n=7                189 ps/byte          5,166 mb/s
 *     timingsafe_bcmp n=8                124 ps/byte          7,873 mb/s
 *     timingsafe_bcmp n=9                147 ps/byte          6,643 mb/s
 *     timingsafe_bcmp n=15                88 ps/byte         11,072 mb/s
 *     timingsafe_bcmp n=16                62 ps/byte         15,746 mb/s
 *     timingsafe_bcmp n=17               136 ps/byte          7,170 mb/s
 *     timingsafe_bcmp n=31                74 ps/byte         13,075 mb/s
 *     timingsafe_bcmp n=32                72 ps/byte         13,497 mb/s
 *     timingsafe_bcmp n=33                80 ps/byte         12,179 mb/s
 *     timingsafe_bcmp n=80                57 ps/byte         16,871 mb/s
 *     timingsafe_bcmp n=128               49 ps/byte         19,890 mb/s
 *     timingsafe_bcmp n=256               31 ps/byte         31,493 mb/s
 *     timingsafe_bcmp n=16384             14 ps/byte         67,941 mb/s
 *     timingsafe_bcmp n=32768             29 ps/byte         33,121 mb/s
 *     timingsafe_bcmp n=131072            29 ps/byte         32,949 mb/s
 *
 * Running time is independent of the byte sequences compared, making
 * this safe to use for comparing secret values such as cryptographic
 * MACs. In contrast, memcmp() may short-circuit after finding the first
 * differing byte.
 *
 * @return nonzero if unequal, otherwise zero
 * @see timingsafe_memcmp()
 * @asyncsignalsafe
 */
int timingsafe_bcmp(const void *a, const void *b, size_t n) {
  const char *p = a, *q = b;
  uint32_t u, u0, u1, u2, u3;
  uint64_t w, w0, w1, w2, w3;
  if (!IsTiny()) {
    if (n >= 8) {
      if (n <= 16) {
        __builtin_memcpy(&w0, p, 8);
        __builtin_memcpy(&w1, q, 8);
        __builtin_memcpy(&w2, p + n - 8, 8);
        __builtin_memcpy(&w3, q + n - 8, 8);
        w = (w0 ^ w1) | (w2 ^ w3);
        return w | w >> 32;
      } else {
        if (IsAsan()) {
          __asan_verify(a, n);
          __asan_verify(b, n);
        }
        if (X86_HAVE(AVX)) {
          return timingsafe_bcmp_avx(p, q, n);
        } else {
          return timingsafe_bcmp_sse(p, q, n);
        }
      }
    } else if (n >= 4) {
      __builtin_memcpy(&u0, p, 4);
      __builtin_memcpy(&u1, q, 4);
      __builtin_memcpy(&u2, p + n - 4, 4);
      __builtin_memcpy(&u3, q + n - 4, 4);
      return (u0 ^ u1) | (u2 ^ u3);
    }
  }
  for (u = 0; n--;) {
    u |= p[n] ^ q[n];
  }
  return u;
}
