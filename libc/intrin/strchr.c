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
#include "libc/assert.h"
#include "libc/dce.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"
#ifndef __aarch64__

static inline const char *strchr_pure(const char *s, int c) {
  for (;; ++s) {
    if ((*s & 255) == (c & 255)) return s;
    if (!*s) return 0;
  }
}

#if defined(__x86_64__) && !defined(__chibicc__)
typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(16)));
static inline const char *strchr_sse(const char *s, unsigned char c) {
  unsigned k;
  unsigned m;
  const xmm_t *p;
  xmm_t v;
  xmm_t z = {0};
  xmm_t n = {c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c};
  k = (uintptr_t)s & 15;
  p = (const xmm_t *)((uintptr_t)s & -16);
  v = *p;
  m = __builtin_ia32_pmovmskb128((v == z) | (v == n));
  m >>= k;
  m <<= k;
  while (!m) {
    v = *++p;
    m = __builtin_ia32_pmovmskb128((v == z) | (v == n));
  }
  m = __builtin_ctzl(m);
  s = (const char *)p + m;
  if (c && !*s) s = 0;
  return s;
}
#endif

static inline const char *strchr_x64(const char *p, uint64_t c) {
  unsigned a, b;
  uint64_t w, x, y;
  for (c *= 0x0101010101010101;; p += 8) {
    w = (uint64_t)(255 & p[7]) << 070 | (uint64_t)(255 & p[6]) << 060 |
        (uint64_t)(255 & p[5]) << 050 | (uint64_t)(255 & p[4]) << 040 |
        (uint64_t)(255 & p[3]) << 030 | (uint64_t)(255 & p[2]) << 020 |
        (uint64_t)(255 & p[1]) << 010 | (uint64_t)(255 & p[0]) << 000;
    if ((x = ~(w ^ c) & ((w ^ c) - 0x0101010101010101) & 0x8080808080808080) |
        (y = ~w & (w - 0x0101010101010101) & 0x8080808080808080)) {
      if (x) {
        a = __builtin_ctzll(x);
        if (y) {
          b = __builtin_ctzll(y);
          if (a <= b) {
            return p + (a >> 3);
          } else {
            return 0;
          }
        } else {
          return p + (a >> 3);
        }
      } else {
        return 0;
      }
    }
  }
}

/**
 * Returns pointer to first instance of character.
 *
 * @param s is a NUL-terminated string
 * @param c is masked with 255 as byte to search for
 * @return pointer to first instance of c or NULL if not found
 *     noting that if c is NUL we return pointer to terminator
 * @asyncsignalsafe
 * @vforksafe
 */
__vex char *strchr(const char *s, int c) {
#if defined(__x86_64__) && !defined(__chibicc__)
  const char *r;
  if (X86_HAVE(SSE)) {
    r = strchr_sse(s, c);
  } else {
    r = strchr_pure(s, c);
  }
  unassert(!r || *r || !(c & 255));
  return (char *)r;
#else
  const char *r;
  for (c &= 255; (uintptr_t)s & 7; ++s) {
    if ((*s & 255) == c) return (char *)s;
    if (!*s) return NULL;
  }
  r = strchr_x64(s, c);
  unassert(!r || *r || !c);
  return (char *)r;
#endif
}

__strong_reference(strchr, index);

#endif /* __aarch64__ */
