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
 * Returns length of NUL-terminated utf-16 string.
 *
 * @param s is non-null NUL-terminated string pointer
 * @return number of shorts (excluding NUL)
 * @asyncsignalsafe
 */
size_t strlen16(const char16_t *s) {
#if defined(__x86_64__) && !defined(__chibicc__)
  __m128i zv = _mm_setzero_si128();
  __m128i *v = (__m128i *)((intptr_t)s & -16);
  int skew = (intptr_t)s & 15;
  unsigned m = _mm_movemask_epi8(_mm_cmpeq_epi16(_mm_load_si128(v), zv));
  m >>= skew;
  m <<= skew;
  while (!m)
    m = _mm_movemask_epi8(_mm_cmpeq_epi16(_mm_load_si128(++v), zv));
  const char16_t *e = (const char16_t *)v;
  e += __builtin_ctz(m) >> 1;
  return e - s;
#elif defined(__aarch64__) && defined(__ARM_NEON)
  uint16x8_t zv = vdupq_n_u16(0);
  uint16_t *v = (uint16_t *)((intptr_t)s & -16);
  int skew = (intptr_t)s & 15;
  uint64_t m = vget_lane_u64(
      vreinterpret_u64_u8(vshrn_n_u16(vceqq_u16(vld1q_u16(v), zv), 4)), 0);
  m >>= skew * 4;
  m <<= skew * 4;
  while (!m) {
    v += 8;
    m = vget_lane_u64(
        vreinterpret_u64_u8(vshrn_n_u16(vceqq_u16(vld1q_u16(v), zv), 4)), 0);
  }
  const char16_t *e = (const char16_t *)v;
  e += __builtin_ctzll(m) >> 3;
  return e - s;
#else
  size_t n = 0;
  while (*s++)
    ++n;
  return n;
#endif
}
