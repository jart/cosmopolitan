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
#include "libc/stdckdint.h"
#include "libc/str/str.h"
#include "third_party/intel/immintrin.internal.h"
#ifndef __aarch64__

/**
 * Returns length of NUL-terminated string w/ limit.
 *
 * @param s is string
 * @param n is max length
 * @return byte length
 * @asyncsignalsafe
 */
size_t strnlen(const char *s, size_t n) {
#if defined(__x86_64__) && !defined(__chibicc__)
  if (!n)
    return 0;
  long skew = (intptr_t)s & 15;
  unsigned m = _mm_movemask_epi8(_mm_cmpeq_epi8(
      _mm_load_si128((__m128i *)((intptr_t)s & -16)), _mm_set1_epi8(0)));
  m >>= skew;
  m <<= skew;
  ssize_t i = -skew;
  while (!m) {
    i += 16;
    if (i >= n)
      return n;
    m = _mm_movemask_epi8(
        _mm_cmpeq_epi8(_mm_load_si128((__m128i *)(s + i)), _mm_set1_epi8(0)));
  }
  i += __builtin_ctz(m);
  return i < n ? i : n;
#else
  size_t i;
  for (i = 0; i < n; ++i)
    if (!s[i])
      break;
  return i;
#endif
}

#endif /* __aarch64__ */
