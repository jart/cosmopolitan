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
 * Returns pointer to first instance of character.
 *
 * @param p is memory to search
 * @param c is search char which is masked with 255
 * @param n is length of p
 * @return is pointer to first instance of c or NULL if not found
 * @asyncsignalsafe
 */
wchar_t *wmemchr(const wchar_t *p, wchar_t c, size_t n) {
#if defined(__x86_64__) && !defined(__chibicc__)
#if defined(__AVX2__)
  if (n >= 8) {
    __m256i vc = _mm256_set1_epi32(c);
    do {
      unsigned m;
      if ((m = _mm256_movemask_epi8(
               _mm256_cmpeq_epi32(_mm256_loadu_si256((const __m256i *)p), vc))))
        return (void *)(p + (__builtin_ctz(m) >> 2));
      p += 8;
      n -= 8;
    } while (n >= 8);
  }
#endif
  if (n >= 4) {
    __m128i vc = _mm_set1_epi32(c);
    do {
      unsigned m;
      if ((m = _mm_movemask_epi8(
               _mm_cmpeq_epi32(_mm_loadu_si128((const __m128i *)p), vc))))
        return (wchar_t *)(p + (__builtin_ctz(m) >> 2));
      p += 4;
      n -= 4;
    } while (n >= 4);
  }
#elif defined(__aarch64__) && defined(__ARM_NEON)
  if (n >= 4) {
    uint32x4_t vc = vdupq_n_u32(c);
    do {
      uint64_t m;
      if ((m = vget_lane_u64(vreinterpret_u64_u8(vshrn_n_u16(
                                 vreinterpretq_u16_u32(vceqq_u32(
                                     vld1q_u32((const uint32_t *)p), vc)),
                                 4)),
                             0)))
        return (wchar_t *)(p + (__builtin_ctzll(m) >> 4));
      p += 4;
      n -= 4;
    } while (n >= 8);
  }
#endif
  for (size_t i = 0; i < n; ++i)
    if (p[i] == c)
      return (wchar_t *)(p + i);
  return 0;
}
