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
#include "third_party/intel/immintrin.internal.h"
#ifndef __aarch64__

static __vex size_t __strlen(const char *s) {
#if defined(__AVX2__)
  __m256i zv = _mm256_setzero_si256();
  __m256i *v = (__m256i *)((intptr_t)s & -32);
  int skew = (intptr_t)s & 31;
  unsigned m =
      _mm256_movemask_epi8(_mm256_cmpeq_epi8(_mm256_loadu_si256(v), zv));
  m >>= skew;
  m <<= skew;
  while (!m)
    m = _mm256_movemask_epi8(_mm256_cmpeq_epi8(_mm256_loadu_si256(++v), zv));
  const char *e = (const char *)v;
  e += __builtin_ctz(m);
  return e - s;
#elif defined(__x86_64__) && !defined(__chibicc__)
  __m128i zv = _mm_setzero_si128();
  __m128i *v = (__m128i *)((intptr_t)s & -16);
  int skew = (intptr_t)s & 15;
  unsigned m = _mm_movemask_epi8(_mm_cmpeq_epi8(_mm_loadu_si128(v), zv));
  m >>= skew;
  m <<= skew;
  while (!m)
    m = _mm_movemask_epi8(_mm_cmpeq_epi8(_mm_loadu_si128(++v), zv));
  const char *e = (const char *)v;
  e += __builtin_ctz(m);
  return e - s;
#else
#define ONES ((word) - 1 / 255)
#define BANE (ONES * (255 / 2 + 1))
  typedef unsigned long mayalias word;
  word w;
  unsigned k;
  const word *p;
  k = (uintptr_t)s & (sizeof(word) - 1);
  p = (const word *)((uintptr_t)s & -sizeof(word));
  w = *p;
  w = ~w & (w - ONES) & BANE;
  w >>= k << 3;
  w <<= k << 3;
  while (!w) {
    w = *++p;
    w = ~w & (w - ONES) & BANE;
  }
  return (const char *)p + (__builtin_ctzl(w) >> 3) - s;
#endif
}

/**
 * Returns length of NUL-terminated string.
 *
 * @param s is non-null NUL-terminated string pointer
 * @return number of bytes (excluding NUL)
 * @asyncsignalsafe
 */
size_t strlen(const char *s) {
  return __strlen(s);
}

#endif /* __aarch64__ */
