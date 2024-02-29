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
#include "libc/limits.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/stdckdint.h"
#include "libc/str/str.h"

typedef wchar_t xmm_t __attribute__((__vector_size__(16), __aligned__(4)));

static inline const wchar_t *wmemrchr_pure(const wchar_t *s, wchar_t c,
                                           size_t n) {
  size_t i;
  for (i = n; i--;) {
    if (s[i] == c) {
      return s + i;
    }
  }
  return 0;
}

#if defined(__x86_64__) && !defined(__chibicc__)
static inline const wchar_t *wmemrchr_sse(const wchar_t *s, wchar_t c,
                                          size_t n) {
  size_t i;
  unsigned m;
  xmm_t v, t = {c, c, c, c};
  for (i = n; i >= 4;) {
    v = *(const xmm_t *)(s + (i -= 4));
    m = __builtin_ia32_pmovmskb128(v == t);
    if (m) {
      m = __builtin_clzl(m) ^ (sizeof(long) * CHAR_BIT - 1);
      return s + i + m / 4;
    }
  }
  while (i--) {
    if (s[i] == c) {
      return s + i;
    }
  }
  return 0;
}
#endif

/**
 * Returns pointer to first instance of character.
 *
 * @param s is memory to search
 * @param c is search word
 * @param n is number of wchar_t elements in `s`
 * @return is pointer to first instance of c or NULL if not found
 * @asyncsignalsafe
 */
__vex void *wmemrchr(const wchar_t *s, wchar_t c, size_t n) {
#if defined(__x86_64__) && !defined(__chibicc__)
  return (void *)wmemrchr_sse(s, c, n);
#else
  return (void *)wmemrchr_pure(s, c, n);
#endif
}
