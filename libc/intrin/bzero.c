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

typedef char xmm_t __attribute__((__vector_size__(16), __aligned__(1)));
typedef long long xmm_a __attribute__((__vector_size__(16), __aligned__(16)));

static void bzero128(char *p, size_t n) {
  xmm_t v = {0};
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
}

#if defined(__x86_64__) && !defined(__chibicc__)
_Microarchitecture("avx") static void bzero_avx(char *p, size_t n) {
  xmm_t v = {0};
  if (n <= 32) {
    *(xmm_t *)(p + n - 16) = v;
    *(xmm_t *)p = v;
  } else if (n >= 1024 && X86_HAVE(ERMS)) {
    asm("rep stosb" : "+D"(p), "+c"(n), "=m"(*(char(*)[n])p) : "a"(0));
  } else {
    if (n < kHalfCache3 || !kHalfCache3) {
      do {
        n -= 32;
        *(xmm_t *)(p + n) = v;
        *(xmm_t *)(p + n + 16) = v;
      } while (n > 32);
    } else {
      while ((uintptr_t)(p + n) & 15) {
        p[--n] = 0;
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
}
#endif

/**
 * Sets memory to zero.
 *
 *     bzero n=0                          661 picoseconds
 *     bzero n=1                          661 ps/byte          1,476 mb/s
 *     bzero n=2                          330 ps/byte          2,952 mb/s
 *     bzero n=3                          220 ps/byte          4,428 mb/s
 *     bzero n=4                          165 ps/byte          5,904 mb/s
 *     bzero n=7                           94 ps/byte         10,333 mb/s
 *     bzero n=8                           41 ps/byte         23,618 mb/s
 *     bzero n=15                          44 ps/byte         22,142 mb/s
 *     bzero n=16                          20 ps/byte         47,236 mb/s
 *     bzero n=31                          21 ps/byte         45,760 mb/s
 *     bzero n=32                          20 ps/byte         47,236 mb/s
 *     bzero n=63                          10 ps/byte         92,997 mb/s
 *     bzero n=64                          15 ps/byte         62,982 mb/s
 *     bzero n=127                         15 ps/byte         62,490 mb/s
 *     bzero n=128                         10 ps/byte         94,473 mb/s
 *     bzero n=255                         14 ps/byte         68,439 mb/s
 *     bzero n=256                          9 ps/byte            105 gb/s
 *     bzero n=511                         15 ps/byte         62,859 mb/s
 *     bzero n=512                         11 ps/byte         83,976 mb/s
 *     bzero n=1023                        15 ps/byte         61,636 mb/s
 *     bzero n=1024                        10 ps/byte         88,916 mb/s
 *     bzero n=2047                         9 ps/byte            105 gb/s
 *     bzero n=2048                         8 ps/byte            109 gb/s
 *     bzero n=4095                         8 ps/byte            115 gb/s
 *     bzero n=4096                         8 ps/byte            118 gb/s
 *     bzero n=8191                         7 ps/byte            129 gb/s
 *     bzero n=8192                         7 ps/byte            130 gb/s
 *     bzero n=16383                        6 ps/byte            136 gb/s
 *     bzero n=16384                        6 ps/byte            137 gb/s
 *     bzero n=32767                        6 ps/byte            140 gb/s
 *     bzero n=32768                        6 ps/byte            141 gb/s
 *     bzero n=65535                       15 ps/byte         64,257 mb/s
 *     bzero n=65536                       15 ps/byte         64,279 mb/s
 *     bzero n=131071                      15 ps/byte         63,166 mb/s
 *     bzero n=131072                      15 ps/byte         63,115 mb/s
 *     bzero n=262143                      15 ps/byte         62,052 mb/s
 *     bzero n=262144                      15 ps/byte         62,097 mb/s
 *     bzero n=524287                      15 ps/byte         61,699 mb/s
 *     bzero n=524288                      15 ps/byte         61,674 mb/s
 *     bzero n=1048575                     16 ps/byte         60,179 mb/s
 *     bzero n=1048576                     15 ps/byte         61,330 mb/s
 *     bzero n=2097151                     15 ps/byte         61,071 mb/s
 *     bzero n=2097152                     15 ps/byte         61,065 mb/s
 *     bzero n=4194303                     16 ps/byte         60,942 mb/s
 *     bzero n=4194304                     16 ps/byte         60,947 mb/s
 *     bzero n=8388607                     16 ps/byte         60,872 mb/s
 *     bzero n=8388608                     16 ps/byte         60,879 mb/s
 *
 * @param p is memory address
 * @param n is byte length
 * @return p
 * @asyncsignalsafe
 */
void bzero(void *p, size_t n) {
  char *b;
  uint64_t x;
  b = p;
#ifdef __x86_64__
  asm("xorl\t%k0,%k0" : "=r"(x));
#else
  if (1) {
    memset(p, 0, n);
    return;
  }
  x = 0;
#endif
  if (n <= 16) {
    if (n >= 8) {
      __builtin_memcpy(b, &x, 8);
      __builtin_memcpy(b + n - 8, &x, 8);
    } else if (n >= 4) {
      __builtin_memcpy(b, &x, 4);
      __builtin_memcpy(b + n - 4, &x, 4);
    } else if (n) {
      do {
        asm volatile("" ::: "memory");
        b[--n] = x;
      } while (n);
    }
#if defined(__x86_64__) && !defined(__chibicc__)
  } else if (IsTiny()) {
    asm("rep stosb" : "+D"(b), "+c"(n), "=m"(*(char(*)[n])b) : "a"(0));
    return;
  } else if (X86_HAVE(AVX)) {
    bzero_avx(b, n);
#endif
  } else {
    bzero128(b, n);
  }
}

__weak_reference(bzero, explicit_bzero);
