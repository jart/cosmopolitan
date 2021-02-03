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
#include "libc/str/str.h"

typedef long long xmm_t __attribute__((__vector_size__(16), __aligned__(1)));

/**
 * Copies memory.
 *
 * DST and SRC may overlap.
 *
 * @param dst is destination
 * @param src is memory to copy
 * @param n is number of bytes to copy
 * @return dst
 * @asyncsignalsafe
 */
void *memmove$pure(void *dst, const void *src, size_t n) {
  size_t i;
  xmm_t v, w;
  char *d, *r;
  const char *s;
  uint64_t a, b;
  d = dst;
  s = src;
  switch (n) {
    case 9 ... 15:
      __builtin_memcpy(&a, s, 8);
      __builtin_memcpy(&b, s + n - 8, 8);
      __builtin_memcpy(d, &a, 8);
      __builtin_memcpy(d + n - 8, &b, 8);
      return d;
    case 5 ... 7:
      __builtin_memcpy(&a, s, 4);
      __builtin_memcpy(&b, s + n - 4, 4);
      __builtin_memcpy(d, &a, 4);
      __builtin_memcpy(d + n - 4, &b, 4);
      return d;
    case 17 ... 32:
      __builtin_memcpy(&v, s, 16);
      __builtin_memcpy(&w, s + n - 16, 16);
      __builtin_memcpy(d, &v, 16);
      __builtin_memcpy(d + n - 16, &w, 16);
      return d;
    case 16:
      __builtin_memcpy(&v, s, 16);
      __builtin_memcpy(d, &v, 16);
      return d;
    case 0:
      return d;
    case 1:
      *d = *s;
      return d;
    case 8:
      __builtin_memcpy(&a, s, 8);
      __builtin_memcpy(d, &a, 8);
      return d;
    case 4:
      __builtin_memcpy(&a, s, 4);
      __builtin_memcpy(d, &a, 4);
      return d;
    case 2:
      __builtin_memcpy(&a, s, 2);
      __builtin_memcpy(d, &a, 2);
      return d;
    case 3:
      __builtin_memcpy(&a, s, 2);
      __builtin_memcpy(&b, s + 1, 2);
      __builtin_memcpy(d, &a, 2);
      __builtin_memcpy(d + 1, &b, 2);
      return d;
    default:
      r = d;
      if (d > s) {
        do {
          n -= 32;
          __builtin_memcpy(&v, s + n, 16);
          __builtin_memcpy(&w, s + n + 16, 16);
          __builtin_memcpy(d + n, &v, 16);
          __builtin_memcpy(d + n + 16, &w, 16);
        } while (n >= 32);
      } else {
        i = 0;
        do {
          __builtin_memcpy(&v, s + i, 16);
          __builtin_memcpy(&w, s + i + 16, 16);
          __builtin_memcpy(d + i, &v, 16);
          __builtin_memcpy(d + i + 16, &w, 16);
        } while ((i += 32) + 32 <= n);
        d += i;
        s += i;
        n -= i;
      }
      switch (n) {
        case 0:
          return r;
        case 17 ... 31:
          __builtin_memcpy(&v, s, 16);
          __builtin_memcpy(&w, s + n - 16, 16);
          __builtin_memcpy(d, &v, 16);
          __builtin_memcpy(d + n - 16, &w, 16);
          return r;
        case 9 ... 15:
          __builtin_memcpy(&a, s, 8);
          __builtin_memcpy(&b, s + n - 8, 8);
          __builtin_memcpy(d, &a, 8);
          __builtin_memcpy(d + n - 8, &b, 8);
          return r;
        case 5 ... 7:
          __builtin_memcpy(&a, s, 4);
          __builtin_memcpy(&b, s + n - 4, 4);
          __builtin_memcpy(d, &a, 4);
          __builtin_memcpy(d + n - 4, &b, 4);
          return r;
        case 16:
          __builtin_memcpy(&v, s, 16);
          __builtin_memcpy(d, &v, 16);
          return r;
        case 8:
          __builtin_memcpy(&a, s, 8);
          __builtin_memcpy(d, &a, 8);
          return r;
        case 4:
          __builtin_memcpy(&a, s, 4);
          __builtin_memcpy(d, &a, 4);
          return r;
        case 1:
          *d = *s;
          return r;
        case 2:
          __builtin_memcpy(&a, s, 2);
          __builtin_memcpy(d, &a, 2);
          return r;
        case 3:
          __builtin_memcpy(&a, s, 2);
          __builtin_memcpy(&b, s + 1, 2);
          __builtin_memcpy(d, &a, 2);
          __builtin_memcpy(d + 1, &b, 2);
          return r;
        default:
          unreachable;
      }
  }
}
