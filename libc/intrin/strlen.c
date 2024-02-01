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
#ifndef __aarch64__

/**
 * Returns length of NUL-terminated string.
 *
 * @param s is non-null NUL-terminated string pointer
 * @return number of bytes (excluding NUL)
 * @asyncsignalsafe
 */
size_t strlen(const char *s) {
#if defined(__x86_64__) && !defined(__chibicc__)
  typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(16)));
  xmm_t z = {0};
  unsigned m, k = (uintptr_t)s & 15;
  const xmm_t *p = (const xmm_t *)((uintptr_t)s & -16);
  m = __builtin_ia32_pmovmskb128(*p == z) >> k << k;
  while (!m) m = __builtin_ia32_pmovmskb128(*++p == z);
  return (const char *)p + __builtin_ctzl(m) - s;
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
#endif /* __aarch64__ */
