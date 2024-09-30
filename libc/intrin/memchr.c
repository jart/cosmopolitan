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
#include "libc/dce.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"
#include "third_party/aarch64/arm_neon.internal.h"
#include "third_party/intel/immintrin.internal.h"
#ifndef __aarch64__

static inline const unsigned char *memchr_pure(const unsigned char *s,
                                               unsigned char c, size_t n) {
  size_t i;
  for (i = 0; i < n; ++i) {
    if (s[i] == c) {
      return s + i;
    }
  }
  return 0;
}

#if defined(__x86_64__) && !defined(__chibicc__)
static const char *memchr_sse(const char *s, char c, size_t n) {
  const char *e = s + n;
  __m128i t = _mm_set1_epi8(c);
  unsigned m, k = (uintptr_t)s & 15;
  m = _mm_movemask_epi8(
      _mm_cmpeq_epi8(_mm_load_si128((const __m128i *)((uintptr_t)s & -16)), t));
  m >>= k;
  if (m) {
    s += __builtin_ctz(m);
    if (s < e)
      return s;
    return 0;
  }
  for (s += 16 - k; s < e; s += 16) {
    m = _mm_movemask_epi8(
        _mm_cmpeq_epi8(_mm_load_si128((const __m128i *)s), t));
    if (m) {
      s += __builtin_ctz(m);
      if (s < e)
        return s;
      return 0;
    }
  }
  return 0;
}
#endif

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
#if defined(__x86_64__) && !defined(__chibicc__)
  return (void *)memchr_sse(s, c, n);
#else
  return (void *)memchr_pure(s, c, n);
#endif
}

#endif /* __aarch64__ */
