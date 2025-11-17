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
#include "libc/str/str.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/immintrin.internal.h"

/**
 * Returns pointer to first instance of character.
 *
 * @param m is memory to search
 * @param c is search byte which is masked with 255
 * @return is pointer to first instance of c
 */
void *rawmemchr(const void *s, int c) {
#if defined(__AVX2__)
  __m256i nv = _mm256_set1_epi8(c);
  __m256i *v = (__m256i *)((intptr_t)s & -32);
  int skew = (intptr_t)s & 31;
  unsigned m =
      _mm256_movemask_epi8(_mm256_cmpeq_epi8(_mm256_load_si256(v), nv));
  m >>= skew;
  m <<= skew;
  while (!m)
    m = _mm256_movemask_epi8(_mm256_cmpeq_epi8(_mm256_load_si256(++v), nv));
  s = (const char *)v;
  s += __builtin_ctz(m);
  return (void *)s;
#elif defined(__x86_64__) && !defined(__chibicc__)
  __m128i nv = _mm_set1_epi8(c);
  __m128i *v = (__m128i *)((intptr_t)s & -16);
  int skew = (intptr_t)s & 15;
  unsigned m = _mm_movemask_epi8(_mm_cmpeq_epi8(_mm_load_si128(v), nv));
  m >>= skew;
  m <<= skew;
  while (!m)
    m = _mm_movemask_epi8(_mm_cmpeq_epi8(_mm_load_si128(++v), nv));
  s = (const char *)v;
  s += __builtin_ctz(m);
  return (void *)s;
#elif defined(__aarch64__) && defined(__ARM_NEON)
  uint8x16_t nv = vdupq_n_u8(c);
  uint8_t *v = (uint8_t *)((intptr_t)s & -16);
  int skew = (intptr_t)s & 15;
  uint64_t m =
      vget_lane_u64(vreinterpret_u64_u8(vshrn_n_u16(
                        vreinterpretq_u16_u8(vceqq_u8(vld1q_u8(v), nv)), 4)),
                    0);
  m >>= skew * 4;
  m <<= skew * 4;
  while (!m) {
    v += 16;
    m = vget_lane_u64(vreinterpret_u64_u8(vshrn_n_u16(
                          vreinterpretq_u16_u8(vceqq_u8(vld1q_u8(v), nv)), 4)),
                      0);
  }
  s = (const char *)v;
  s += __builtin_ctzll(m) >> 2;
  return (void *)s;
#else
  uint64_t v, w;
  const unsigned char *p;
  p = s;
  c &= 255;
  v = 0x0101010101010101ul * c;
  for (; (uintptr_t)p & 7; ++p)
    if (*p == c)
      return (void *)p;
  for (;; p += 8) {
    w = (uint64_t)p[7] << 070 | (uint64_t)p[6] << 060 | (uint64_t)p[5] << 050 |
        (uint64_t)p[4] << 040 | (uint64_t)p[3] << 030 | (uint64_t)p[2] << 020 |
        (uint64_t)p[1] << 010 | (uint64_t)p[0] << 000;
    if ((w = ~(w ^ v) & ((w ^ v) - 0x0101010101010101) & 0x8080808080808080)) {
      p += (unsigned)__builtin_ctzll(w) >> 3;
      break;
    }
  }
  return (void *)p;
#endif
}
