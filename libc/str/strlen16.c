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
#include "libc/str/str.h"

typedef char16_t xmm_t __attribute__((__vector_size__(16), __aligned__(16)));

/**
 * Returns length of NUL-terminated utf-16 string.
 *
 * @param s is non-null NUL-terminated string pointer
 * @return number of shorts (excluding NUL)
 * @asyncsignalsafe
 */
__vex size_t strlen16(const char16_t *s) {
#if defined(__x86_64__) && !defined(__chibicc__)
  size_t n;
  xmm_t z = {0};
  unsigned m, k = (uintptr_t)s & 15;
  const xmm_t *p = (const xmm_t *)((uintptr_t)s & -16);
  m = __builtin_ia32_pmovmskb128(*p == z) >> k << k;
  while (!m) m = __builtin_ia32_pmovmskb128(*++p == z);
  n = (const char16_t *)p + (__builtin_ctzl(m) >> 1) - s;
  return n;
#else
  size_t n = 0;
  while (*s++) ++n;
  return n;
#endif
}
