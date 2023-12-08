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
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"
#ifndef __aarch64__

#ifndef __chibicc__
typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(1)));
typedef long long xmm_a __attribute__((__vector_size__(16), __aligned__(16)));
static void *memset_sse(char *p, char c, size_t n) {
  xmm_t v = {c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c};
  if (n <= 32) {
    *(xmm_t *)(p + n - 16) = v;
    *(xmm_t *)p = v;
  } else {
    do {
      n -= 32;
      *(xmm_t *)(p + n) = v;
      *(xmm_t *)(p + n + 16) = v;
    } while (n > 32);
    *(xmm_t *)(p + 16) = v;
    *(xmm_t *)p = v;
  }
  return p;
}
#endif

#if defined(__x86_64__) && !defined(__chibicc__)
_Microarchitecture("avx") static void *memset_avx(char *p, char c, size_t n) {
  char *t;
  xmm_t v = {c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c};
  if (n <= 32) {
    *(xmm_t *)(p + n - 16) = v;
    *(xmm_t *)p = v;
  } else if (n >= 1024 && X86_HAVE(ERMS)) {
    asm("rep stosb" : "=D"(t), "+c"(n), "=m"(*(char(*)[n])p) : "0"(p), "a"(c));
  } else {
    if (n < kHalfCache3 || !kHalfCache3) {
      do {
        n -= 32;
        *(xmm_t *)(p + n) = v;
        *(xmm_t *)(p + n + 16) = v;
      } while (n > 32);
    } else {
      while ((uintptr_t)(p + n) & 15) {
        p[--n] = c;
      }
      do {
        n -= 32;
        __builtin_ia32_movntdq((xmm_a *)(p + n), (xmm_a)v);
        __builtin_ia32_movntdq((xmm_a *)(p + n + 16), (xmm_a)v);
      } while (n > 32);
      asm("sfence");
    }
    *(xmm_t *)(p + 16) = v;
    *(xmm_t *)p = v;
  }
  return p;
}
#endif /* __x86_64__ */

/**
 * Sets memory.
 *
 *     memset n=0                         992 picoseconds
 *     memset n=1                         992 ps/byte            984 mb/s
 *     memset n=2                         330 ps/byte          2,952 mb/s
 *     memset n=3                         330 ps/byte          2,952 mb/s
 *     memset n=4                         165 ps/byte          5,904 mb/s
 *     memset n=7                          94 ps/byte         10,333 mb/s
 *     memset n=8                         124 ps/byte          7,872 mb/s
 *     memset n=15                         66 ps/byte         14,761 mb/s
 *     memset n=16                         62 ps/byte         15,745 mb/s
 *     memset n=31                         32 ps/byte         30,506 mb/s
 *     memset n=32                         20 ps/byte         47,236 mb/s
 *     memset n=63                         26 ps/byte         37,198 mb/s
 *     memset n=64                         20 ps/byte         47,236 mb/s
 *     memset n=127                        23 ps/byte         41,660 mb/s
 *     memset n=128                        12 ps/byte         75,578 mb/s
 *     memset n=255                        18 ps/byte         53,773 mb/s
 *     memset n=256                        12 ps/byte         75,578 mb/s
 *     memset n=511                        17 ps/byte         55,874 mb/s
 *     memset n=512                        12 ps/byte         75,578 mb/s
 *     memset n=1023                       16 ps/byte         58,080 mb/s
 *     memset n=1024                       11 ps/byte         86,375 mb/s
 *     memset n=2047                        9 ps/byte            101 gb/s
 *     memset n=2048                        8 ps/byte            107 gb/s
 *     memset n=4095                        8 ps/byte            113 gb/s
 *     memset n=4096                        8 ps/byte            114 gb/s
 *     memset n=8191                        7 ps/byte            126 gb/s
 *     memset n=8192                        7 ps/byte            126 gb/s
 *     memset n=16383                       7 ps/byte            133 gb/s
 *     memset n=16384                       7 ps/byte            131 gb/s
 *     memset n=32767                      14 ps/byte         69,246 mb/s
 *     memset n=32768                       6 ps/byte            138 gb/s
 *     memset n=65535                      15 ps/byte         62,756 mb/s
 *     memset n=65536                      15 ps/byte         62,982 mb/s
 *     memset n=131071                     18 ps/byte         52,834 mb/s
 *     memset n=131072                     15 ps/byte         62,023 mb/s
 *     memset n=262143                     15 ps/byte         61,169 mb/s
 *     memset n=262144                     16 ps/byte         61,011 mb/s
 *     memset n=524287                     16 ps/byte         60,633 mb/s
 *     memset n=524288                     16 ps/byte         57,902 mb/s
 *     memset n=1048575                    16 ps/byte         60,405 mb/s
 *     memset n=1048576                    16 ps/byte         58,754 mb/s
 *     memset n=2097151                    16 ps/byte         59,329 mb/s
 *     memset n=2097152                    16 ps/byte         58,729 mb/s
 *     memset n=4194303                    16 ps/byte         59,329 mb/s
 *     memset n=4194304                    16 ps/byte         59,262 mb/s
 *     memset n=8388607                    16 ps/byte         59,530 mb/s
 *     memset n=8388608                    16 ps/byte         60,205 mb/s
 *
 * @param p is memory address
 * @param c is masked with 255 and used as repeated byte
 * @param n is byte length
 * @return p
 * @asyncsignalsafe
 */
void *memset(void *p, int c, size_t n) {
  char *b;
  uint32_t u;
  uint64_t x;
  b = p;
  if (n <= 16) {
    if (n >= 8) {
      x = 0x0101010101010101ul * (c & 255);
      __builtin_memcpy(b, &x, 8);
      __builtin_memcpy(b + n - 8, &x, 8);
    } else if (n >= 4) {
      u = 0x01010101u * (c & 255);
      __builtin_memcpy(b, &u, 4);
      __builtin_memcpy(b + n - 4, &u, 4);
    } else if (n) {
      do {
        asm volatile("" ::: "memory");
        b[--n] = c;
      } while (n);
    }
    return b;
#if defined(__x86_64__) && !defined(__chibicc__)
  } else if (IsTiny()) {
    asm("rep stosb" : "+D"(b), "+c"(n), "=m"(*(char(*)[n])b) : "a"(c));
    return p;
  } else if (X86_HAVE(AVX)) {
    return memset_avx(b, c, n);
#endif
  } else {
    return memset_sse(b, c, n);
  }
}

#endif /* __aarch64__ */
