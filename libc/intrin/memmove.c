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
#include "libc/intrin/asan.internal.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/str/str.h"
#ifndef __aarch64__

typedef long long xmm_t __attribute__((__vector_size__(16), __aligned__(1)));
typedef long long xmm_a __attribute__((__vector_size__(16), __aligned__(16)));

/**
 * Copies memory.
 *
 *     memmove n=0                        661 picoseconds
 *     memmove n=1                        661 ps/byte          1,476 mb/s
 *     memmove n=2                        330 ps/byte          2,952 mb/s
 *     memmove n=3                        330 ps/byte          2,952 mb/s
 *     memmove n=4                        165 ps/byte          5,904 mb/s
 *     memmove n=7                        141 ps/byte          6,888 mb/s
 *     memmove n=8                         82 ps/byte             11 GB/s
 *     memmove n=15                        44 ps/byte             21 GB/s
 *     memmove n=16                        41 ps/byte             23 GB/s
 *     memmove n=31                        32 ps/byte             29 GB/s
 *     memmove n=32                        31 ps/byte             30 GB/s
 *     memmove n=63                        21 ps/byte             45 GB/s
 *     memmove n=64                        15 ps/byte             61 GB/s
 *     memmove n=127                       13 ps/byte             73 GB/s
 *     memmove n=128                       31 ps/byte             30 GB/s
 *     memmove n=255                       20 ps/byte             45 GB/s
 *     memmove n=256                       19 ps/byte             49 GB/s
 *     memmove n=511                       16 ps/byte             56 GB/s
 *     memmove n=512                       17 ps/byte             54 GB/s
 *     memmove n=1023                      18 ps/byte             52 GB/s
 *     memmove n=1024                      13 ps/byte             72 GB/s
 *     memmove n=2047                       9 ps/byte             96 GB/s
 *     memmove n=2048                       9 ps/byte             98 GB/s
 *     memmove n=4095                       8 ps/byte            112 GB/s
 *     memmove n=4096                       8 ps/byte            109 GB/s
 *     memmove n=8191                       7 ps/byte            124 GB/s
 *     memmove n=8192                       7 ps/byte            125 GB/s
 *     memmove n=16383                      7 ps/byte            134 GB/s
 *     memmove n=16384                      7 ps/byte            134 GB/s
 *     memmove n=32767                     13 ps/byte             72 GB/s
 *     memmove n=32768                     13 ps/byte             72 GB/s
 *     memmove n=65535                     13 ps/byte             68 GB/s
 *     memmove n=65536                     14 ps/byte             67 GB/s
 *     memmove n=131071                    14 ps/byte             65 GB/s
 *     memmove n=131072                    14 ps/byte             64 GB/s
 *     memmove n=262143                    15 ps/byte             63 GB/s
 *     memmove n=262144                    15 ps/byte             63 GB/s
 *     memmove n=524287                    15 ps/byte             61 GB/s
 *     memmove n=524288                    15 ps/byte             61 GB/s
 *     memmove n=1048575                   15 ps/byte             61 GB/s
 *     memmove n=1048576                   15 ps/byte             61 GB/s
 *     memmove n=2097151                   19 ps/byte             48 GB/s
 *     memmove n=2097152                   27 ps/byte             35 GB/s
 *     memmove n=4194303                   28 ps/byte             33 GB/s
 *     memmove n=4194304                   28 ps/byte             33 GB/s
 *     memmove n=8388607                   28 ps/byte             33 GB/s
 *     memmove n=8388608                   28 ps/byte             33 GB/s
 *
 * DST and SRC may overlap.
 *
 * @param dst is destination
 * @param src is memory to copy
 * @param n is number of bytes to copy
 * @return dst
 * @asyncsignalsafe
 */
__vex void *memmove(void *dst, const void *src, size_t n) {
  char *d;
  size_t i;
  const char *s;
  uint64_t a, b;
  xmm_t v, w, x, y, V, W, X, Y;
  d = dst;
  s = src;

#if defined(__x86_64__) && !defined(__chibicc__)
  if (IsTiny()) {
    uint16_t w1, w2;
    uint32_t l1, l2;
    uint64_t q1, q2;
    if (n <= 16) {
      if (n >= 8) {
        __builtin_memcpy(&q1, s, 8);
        __builtin_memcpy(&q2, s + n - 8, 8);
        __builtin_memcpy(d, &q1, 8);
        __builtin_memcpy(d + n - 8, &q2, 8);
      } else if (n >= 4) {
        __builtin_memcpy(&l1, s, 4);
        __builtin_memcpy(&l2, s + n - 4, 4);
        __builtin_memcpy(d, &l1, 4);
        __builtin_memcpy(d + n - 4, &l2, 4);
      } else if (n >= 2) {
        __builtin_memcpy(&w1, s, 2);
        __builtin_memcpy(&w2, s + n - 2, 2);
        __builtin_memcpy(d, &w1, 2);
        __builtin_memcpy(d + n - 2, &w2, 2);
      } else if (n) {
        *d = *s;
      }
    } else {
      if (d <= s) {
        asm("rep movsb"
            : "+D"(d), "+S"(s), "+c"(n), "=m"(*(char(*)[n])dst)
            : "m"(*(char(*)[n])src));
      } else {
        d += n - 1;
        s += n - 1;
        asm("std\n\t"
            "rep movsb\n\t"
            "cld"
            : "+D"(d), "+S"(s), "+c"(n), "=m"(*(char(*)[n])dst)
            : "m"(*(char(*)[n])src));
      }
    }
    return dst;
  }
#endif

  switch (n) {
    case 0:
      return d;
    case 1:
      *d = *s;
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
    case 4:
      __builtin_memcpy(&a, s, 4);
      __builtin_memcpy(d, &a, 4);
      return d;
    case 5 ... 7:
      __builtin_memcpy(&a, s, 4);
      __builtin_memcpy(&b, s + n - 4, 4);
      __builtin_memcpy(d, &a, 4);
      __builtin_memcpy(d + n - 4, &b, 4);
      return d;
    case 8:
      __builtin_memcpy(&a, s, 8);
      __builtin_memcpy(d, &a, 8);
      return d;
    case 9 ... 15:
      __builtin_memcpy(&a, s, 8);
      __builtin_memcpy(&b, s + n - 8, 8);
      __builtin_memcpy(d, &a, 8);
      __builtin_memcpy(d + n - 8, &b, 8);
      return d;
    case 16:
      *(xmm_t *)d = *(xmm_t *)s;
      return d;
    case 17 ... 32:
      v = *(xmm_t *)s;
      w = *(xmm_t *)(s + n - 16);
      *(xmm_t *)d = v;
      *(xmm_t *)(d + n - 16) = w;
      return d;
    case 33 ... 64:
      v = *(xmm_t *)s;
      w = *(xmm_t *)(s + 16);
      x = *(xmm_t *)(s + n - 32);
      y = *(xmm_t *)(s + n - 16);
      *(xmm_t *)d = v;
      *(xmm_t *)(d + 16) = w;
      *(xmm_t *)(d + n - 32) = x;
      *(xmm_t *)(d + n - 16) = y;
      return d;
    case 65 ... 127:
      v = *(xmm_t *)s;
      w = *(xmm_t *)(s + 16);
      x = *(xmm_t *)(s + 32);
      y = *(xmm_t *)(s + 48);
      V = *(xmm_t *)(s + n - 64);
      W = *(xmm_t *)(s + n - 48);
      X = *(xmm_t *)(s + n - 32);
      Y = *(xmm_t *)(s + n - 16);
      *(xmm_t *)d = v;
      *(xmm_t *)(d + 16) = w;
      *(xmm_t *)(d + 32) = x;
      *(xmm_t *)(d + 48) = y;
      *(xmm_t *)(d + n - 64) = V;
      *(xmm_t *)(d + n - 48) = W;
      *(xmm_t *)(d + n - 32) = X;
      *(xmm_t *)(d + n - 16) = Y;
      return d;
    default:
      if (d == s) return d;

#if defined(__x86_64__) && !defined(__chibicc__)
      if (n < kHalfCache3 || !kHalfCache3) {
        if (d > s) {
          if (IsAsan() || n < 900 || !X86_HAVE(ERMS)) {
            do {
              n -= 32;
              v = *(const xmm_t *)(s + n);
              w = *(const xmm_t *)(s + n + 16);
              *(xmm_t *)(d + n) = v;
              *(xmm_t *)(d + n + 16) = w;
            } while (n >= 32);
          } else {
            asm("std\n\t"
                "rep movsb\n\t"
                "cld"
                : "=D"(d), "=S"(s), "+c"(n), "=m"(*(char(*)[n])d)
                : "0"(d + n - 1), "1"(s + n - 1), "m"(*(char(*)[n])s));
            return dst;
          }
        } else {
          if (IsAsan() || n < 900 || !X86_HAVE(ERMS)) {
            i = 0;
            do {
              v = *(const xmm_t *)(s + i);
              w = *(const xmm_t *)(s + i + 16);
              *(xmm_t *)(d + i) = v;
              *(xmm_t *)(d + i + 16) = w;
            } while ((i += 32) + 32 <= n);
            d += i;
            s += i;
            n -= i;
          } else {
            asm("rep movsb"
                : "+D"(d), "+S"(s), "+c"(n), "=m"(*(char(*)[n])d)
                : "m"(*(char(*)[n])s));
            return dst;
          }
        }
      } else {
        if (d > s) {
          while ((uintptr_t)(d + n) & 15) {
            --n;
            d[n] = s[n];
          }
          do {
            n -= 32;
            v = *(const xmm_t *)(s + n);
            w = *(const xmm_t *)(s + n + 16);
            __builtin_ia32_movntdq((xmm_a *)(d + n), v);
            __builtin_ia32_movntdq((xmm_a *)(d + n + 16), w);
          } while (n >= 32);
        } else {
          i = 0;
          while ((uintptr_t)(d + i) & 15) {
            d[i] = s[i];
            ++i;
          }
          do {
            v = *(const xmm_t *)(s + i);
            w = *(const xmm_t *)(s + i + 16);
            __builtin_ia32_movntdq((xmm_a *)(d + i), v);
            __builtin_ia32_movntdq((xmm_a *)(d + i + 16), w);
          } while ((i += 32) + 32 <= n);
          d += i;
          s += i;
          n -= i;
        }
        asm("sfence");
      }
#else

      if (d > s) {
        do {
          n -= 32;
          v = *(const xmm_t *)(s + n);
          w = *(const xmm_t *)(s + n + 16);
          *(xmm_t *)(d + n) = v;
          *(xmm_t *)(d + n + 16) = w;
        } while (n >= 32);
      } else {
        i = 0;
        do {
          v = *(const xmm_t *)(s + i);
          w = *(const xmm_t *)(s + i + 16);
          *(xmm_t *)(d + i) = v;
          *(xmm_t *)(d + i + 16) = w;
        } while ((i += 32) + 32 <= n);
        d += i;
        s += i;
        n -= i;
      }

#endif

      if (n) {
        if (n >= 16) {
          v = *(const xmm_t *)s;
          w = *(const xmm_t *)(s + n - 16);
          *(xmm_t *)d = v;
          *(xmm_t *)(d + n - 16) = w;
        } else if (n >= 8) {
          __builtin_memcpy(&a, s, 8);
          __builtin_memcpy(&b, s + n - 8, 8);
          __builtin_memcpy(d, &a, 8);
          __builtin_memcpy(d + n - 8, &b, 8);
        } else if (n >= 4) {
          __builtin_memcpy(&a, s, 4);
          __builtin_memcpy(&b, s + n - 4, 4);
          __builtin_memcpy(d, &a, 4);
          __builtin_memcpy(d + n - 4, &b, 4);
        } else if (n >= 2) {
          __builtin_memcpy(&a, s, 2);
          __builtin_memcpy(&b, s + n - 2, 2);
          __builtin_memcpy(d, &a, 2);
          __builtin_memcpy(d + n - 2, &b, 2);
        } else {
          *d = *s;
        }
      }

      return dst;
  }
}

__weak_reference(memmove, memcpy);

#endif /* __aarch64__ */
