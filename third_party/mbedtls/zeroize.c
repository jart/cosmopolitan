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
#include "third_party/mbedtls/platform.h"

void mbedtls_platform_zeroize(void *p, size_t n) {
  MBEDTLS_INTERNAL_VALIDATE(!n || p);
  char *b;
  uint64_t x;
  x = 0;
  b = p;
  switch (n) {
    case 0:
      return;
    case 1:
      __builtin_memcpy(b, &x, 1);
      return;
    case 2:
      __builtin_memcpy(b, &x, 2);
      return;
    case 3:
      __builtin_memcpy(b, &x, 2);
      __builtin_memcpy(b + 1, &x, 2);
      return;
    case 4:
      __builtin_memcpy(b, &x, 4);
      return;
    case 5 ... 7:
      __builtin_memcpy(b, &x, 4);
      __builtin_memcpy(b + n - 4, &x, 4);
      return;
    case 8:
      __builtin_memcpy(b, &x, 8);
      return;
    case 9 ... 16:
      __builtin_memcpy(b, &x, 8);
      __builtin_memcpy(b + n - 8, &x, 8);
      return;
    default:
      do {
        n -= 16;
        __builtin_memcpy(b + n, &x, 8);
        asm volatile("" ::: "memory");
        __builtin_memcpy(b + n + 8, &x, 8);
      } while (n >= 16);
      switch (n) {
        case 0:
          return;
        case 1:
          __builtin_memcpy(b, &x, 1);
          return;
        case 2:
          __builtin_memcpy(b, &x, 2);
          return;
        case 3:
          __builtin_memcpy(b, &x, 2);
          __builtin_memcpy(b + 1, &x, 2);
          return;
        case 4:
          __builtin_memcpy(b, &x, 4);
          return;
        case 5 ... 7:
          __builtin_memcpy(b, &x, 4);
          __builtin_memcpy(b + n - 4, &x, 4);
          return;
        case 8:
          __builtin_memcpy(b, &x, 8);
          return;
        case 9 ... 15:
          __builtin_memcpy(b, &x, 8);
          __builtin_memcpy(b + n - 8, &x, 8);
          return;
        default:
          unreachable;
      }
  }
}
