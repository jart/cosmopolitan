/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/immintrin.internal.h"

/**
 * Returns pointer to first instance of character in range.
 */
char32_t *rawmemchr32(const char32_t *s, char32_t c) {
#if defined(__x86_64__) && !defined(__chibicc__)
  __m128i nv = _mm_set1_epi32(c);
  __m128i *v = (__m128i *)((intptr_t)s & -16);
  int skew = (intptr_t)s & 15;
  unsigned m = _mm_movemask_epi8(_mm_cmpeq_epi32(_mm_load_si128(v), nv));
  m >>= skew;
  m <<= skew;
  while (!m)
    m = _mm_movemask_epi8(_mm_cmpeq_epi32(_mm_load_si128(++v), nv));
  s = (const char32_t *)v;
  s += __builtin_ctz(m) >> 2;
  return (char32_t *)s;
#elif defined(__aarch64__) && defined(__ARM_NEON)
  uint32x4_t nv = vdupq_n_u32(c);
  uint32_t *v = (uint32_t *)((intptr_t)s & -16);
  int skew = (intptr_t)s & 15;
  uint64_t m =
      vget_lane_u64(vreinterpret_u64_u8(vshrn_n_u16(
                        vreinterpretq_u16_u32(vceqq_u32(vld1q_u32(v), nv)), 4)),
                    0);
  m >>= skew * 4;
  m <<= skew * 4;
  while (!m) {
    v += 4;
    m = vget_lane_u64(
        vreinterpret_u64_u8(
            vshrn_n_u16(vreinterpretq_u16_u32(vceqq_u32(vld1q_u32(v), nv)), 4)),
        0);
  }
  s = (const char32_t *)v;
  s += __builtin_ctzll(m) >> 4;
  return (char32_t *)s;
#else
  for (;; ++s)
    if (*s == c)
      return (char32_t *)s;
#endif
}
