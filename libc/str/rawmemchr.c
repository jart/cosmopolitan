/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
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
#include "libc/assert.h"
#include "libc/dce.h"
#include "libc/intrin/asan.internal.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"

typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(16)));

static inline const unsigned char *rawmemchr_pure(const unsigned char *s,
                                                  unsigned char c) {
  for (;; ++s) {
    if (*s == c) {
      return s;
    }
  }
}

noasan static inline const char *rawmemchr_sse(const char *s, unsigned char c) {
  unsigned k;
  unsigned m;
  xmm_t v, *p;
  xmm_t n = {c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c};
  k = (uintptr_t)s & 15;
  p = (const xmm_t *)((uintptr_t)s & -16);
  v = *p;
  m = __builtin_ia32_pmovmskb128(v == n);
  m >>= k;
  m <<= k;
  while (!m) {
    v = *++p;
    m = __builtin_ia32_pmovmskb128(v == n);
  }
  m = __builtin_ctzll(m);
  return (const char *)p + m;
}

/**
 * Returns pointer to first instance of character.
 *
 * @param m is memory to search
 * @param c is search byte which is masked with 255
 * @return is pointer to first instance of c
 */
void *rawmemchr(const void *s, int c) {
  const void *r;
  if (X86_HAVE(SSE)) {
    if (IsAsan()) __asan_verify(s, 1);
    r = rawmemchr_sse(s, c);
  } else {
    r = rawmemchr_pure(s, c);
  }
  return (void *)r;
}
