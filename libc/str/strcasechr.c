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
#include "libc/str/tab.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/immintrin.internal.h"

/**
 * Returns pointer to first instance of character.
 */
char *strcasechr(const char *s, int c) {
#if defined(__AVX2__)
  __m256i nz = _mm256_setzero_si256();
  __m256i nvl = _mm256_set1_epi8(kToLower[c & 255]);
  __m256i nvu = _mm256_set1_epi8(kToUpper[c & 255]);
  __m256i *v = (__m256i *)((intptr_t)s & -32);
  int skew = (intptr_t)s & 31;
  unsigned m = _mm256_movemask_epi8(_mm256_or_si256(
      _mm256_cmpeq_epi8(_mm256_load_si256(v), nz),
      _mm256_or_si256(_mm256_cmpeq_epi8(_mm256_load_si256(v), nvl),
                      _mm256_cmpeq_epi8(_mm256_load_si256(v), nvu))));
  m >>= skew;
  m <<= skew;
  while (!m) {
    ++v;
    m = _mm256_movemask_epi8(_mm256_or_si256(
        _mm256_cmpeq_epi8(_mm256_load_si256(v), nz),
        _mm256_or_si256(_mm256_cmpeq_epi8(_mm256_load_si256(v), nvl),
                        _mm256_cmpeq_epi8(_mm256_load_si256(v), nvu))));
  }
  s = (const char *)v;
  s += __builtin_ctz(m);
  return (void *)(*s ? s : 0);
#elif defined(__x86_64__) && !defined(__chibicc__)
  __m128i nz = _mm_setzero_si128();
  __m128i nvl = _mm_set1_epi8(kToLower[c & 255]);
  __m128i nvu = _mm_set1_epi8(kToUpper[c & 255]);
  __m128i *v = (__m128i *)((intptr_t)s & -16);
  int skew = (intptr_t)s & 15;
  unsigned m = _mm_movemask_epi8(
      _mm_or_si128(_mm_cmpeq_epi8(_mm_load_si128(v), nz),
                   _mm_or_si128(_mm_cmpeq_epi8(_mm_load_si128(v), nvl),
                                _mm_cmpeq_epi8(_mm_load_si128(v), nvu))));
  m >>= skew;
  m <<= skew;
  while (!m) {
    ++v;
    m = _mm_movemask_epi8(
        _mm_or_si128(_mm_cmpeq_epi8(_mm_load_si128(v), nz),
                     _mm_or_si128(_mm_cmpeq_epi8(_mm_load_si128(v), nvl),
                                  _mm_cmpeq_epi8(_mm_load_si128(v), nvu))));
  }
  s = (const char *)v;
  s += __builtin_ctz(m);
  return (void *)(*s ? s : 0);
#elif defined(__aarch64__) && defined(__ARM_NEON)
  uint8x16_t nz = vdupq_n_u8(0);
  uint8x16_t nvl = vdupq_n_u8(kToLower[c & 255]);
  uint8x16_t nvu = vdupq_n_u8(kToUpper[c & 255]);
  uint8_t *v = (uint8_t *)((intptr_t)s & -16);
  int skew = (intptr_t)s & 15;
  uint64_t m = vget_lane_u64(
      vreinterpret_u64_u8(vshrn_n_u16(
          vreinterpretq_u16_u8(vorrq_u8(vceqq_u8(vld1q_u8(v), nz),
                                        vorrq_u8(vceqq_u8(vld1q_u8(v), nvl),
                                                 vceqq_u8(vld1q_u8(v), nvu)))),
          4)),
      0);
  m >>= skew * 4;
  m <<= skew * 4;
  while (!m) {
    v += 16;
    m = vget_lane_u64(vreinterpret_u64_u8(vshrn_n_u16(
                          vreinterpretq_u16_u8(
                              vorrq_u8(vceqq_u8(vld1q_u8(v), nz),
                                       vorrq_u8(vceqq_u8(vld1q_u8(v), nvl),
                                                vceqq_u8(vld1q_u8(v), nvu)))),
                          4)),
                      0);
  }
  s = (const char *)v;
  s += __builtin_ctzll(m) >> 2;
  return (void *)(*s ? s : 0);
#else
  for (;; ++s) {
    if (kToLower[*s & 255] == kToLower[c & 255])
      return (char *)s;
    if (!*s)
      return 0;
  }
#endif
}
