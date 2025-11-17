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
#ifndef __aarch64__

/**
 * Returns pointer to first instance of character.
 *
 * @param s is memory to search
 * @param c is search byte which is masked with 255
 * @param n is byte length of p
 * @return is pointer to first instance of c or NULL if not found
 * @asyncsignalsafe
 */
void *memchr(const void *s, int c, size_t n) {
  if (!n)
    return 0;
  char *p = (char *)s;
#if defined(__AVX2__)
  __m256i nv = _mm256_set1_epi8(c);
  long skew = (intptr_t)p & 31;
  unsigned m = _mm256_movemask_epi8(
      _mm256_cmpeq_epi8(_mm256_load_si256((__m256i *)((intptr_t)p & -32)), nv));
  m >>= skew;
  m <<= skew;
  ssize_t i = -skew;
  while (!m) {
    i += 32;
    if (i >= n)
      return 0;
    m = _mm256_movemask_epi8(
        _mm256_cmpeq_epi8(_mm256_load_si256((__m256i *)(p + i)), nv));
  }
  i += __builtin_ctz(m);
  return i < n ? p + i : 0;
#elif defined(__x86_64__) && !defined(__chibicc__)
  __m128i nv = _mm_set1_epi8(c);
  long skew = (intptr_t)p & 15;
  unsigned m = _mm_movemask_epi8(
      _mm_cmpeq_epi8(_mm_load_si128((__m128i *)((intptr_t)p & -16)), nv));
  m >>= skew;
  m <<= skew;
  ssize_t i = -skew;
  while (!m) {
    i += 16;
    if (i >= n)
      return 0;
    m = _mm_movemask_epi8(
        _mm_cmpeq_epi8(_mm_load_si128((__m128i *)(p + i)), nv));
  }
  i += __builtin_ctz(m);
  return i < n ? p + i : 0;
#else
  for (size_t i = 0; i < n; ++i)
    if ((p[i] & 255) == (c & 255))
      return p + i;
  return 0;
#endif
}

#endif /* __aarch64__ */
