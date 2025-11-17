/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2025 Justine Alexandra Roberts Tunney                              │
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
#include "libc/assert.h"
#include "libc/str/str.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/immintrin.internal.h"

/**
 * Compares memory byte by byte.
 *
 * @return an integer that's (1) equal to zero if `a` is equal to `b`,
 *     (2) less than zero if `a` is less than `b`, or (3) greater than
 *     zero if `a` is greater than `b`
 * @asyncsignalsafe
 */
int memcmp(const void *a, const void *b, size_t n) {
  const unsigned char *p = a;
  const unsigned char *q = b;

  // perform shortcuts
  int c;
  if (p == q || !n)
    return 0;
  if ((c = *p - *q))
    return c;

  // perform optimized implementation
  size_t i = 0;
#if defined(__x86_64__) && !defined(__chibicc__)
  for (; n - i >= 16; i += 16) {
    unsigned m;
    if ((m = 0xffff ^ _mm_movemask_epi8(_mm_cmpeq_epi8(
                          _mm_loadu_si128((const __m128i *)(p + i)),
                          _mm_loadu_si128((const __m128i *)(q + i))))))
      return p[i + __builtin_ctz(m)] - q[i + __builtin_ctz(m)];
  }
#elif defined(__aarch64__) && defined(__ARM_NEON)
  for (; n - i >= 16; i += 16) {
    uint64_t m;
    if ((m = vget_lane_u64(vreinterpret_u64_u8(vshrn_n_u16(
                               vreinterpretq_u16_u8(vmvnq_u8(
                                   vceqq_u8(vld1q_u8(p + i), vld1q_u8(q + i)))),
                               4)),
                           0)))
      return p[i + (__builtin_ctzll(m) >> 2)] -
             q[i + (__builtin_ctzll(m) >> 2)];
  }
#endif

  // perform pure implementation
  for (; i < n; ++i)
    if ((c = p[i] - q[i]))
      return c;
  return 0;
}
