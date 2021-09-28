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
#include "libc/log/libfatal.internal.h"
#include "libc/nexgen32e/bsr.h"
#include "libc/nexgen32e/uart.internal.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/str/tpenc.h"
#include "libc/sysv/consts/nr.h"

/**
 * Low-level printf.
 *
 * This will work without any cosmopolitan runtime support once the
 * executable has been loaded into memory.
 */
privileged noasan noinstrument void __printf(const char *fmt, ...) {
  long d, ax;
  va_list va;
  uint16_t dx;
  const char *s;
  uint32_t wrote;
  unsigned long x;
  unsigned char al;
  const char16_t *S;
  int n, t, w, plus;
  char c, f, *p, *e, b[2048];
  w = 0;
  p = b;
  e = p + sizeof(b);
  va_start(va, fmt);
  do {
    switch ((c = *fmt++)) {
      default:
        if (p < e) {
          *p++ = c;
        }
        break;
      case '\0':
        break;
      case '%':
        w = 0;
        f = ' ';
        plus = 0;
      NeedMoar:
        switch ((c = *fmt++)) {
          case '\0':
            break;
          case '0':
            f = c;
            goto NeedMoar;
          case '+':
            plus = c;
            goto NeedMoar;
          case '*':
            w = va_arg(va, int);
            goto NeedMoar;
          case 'd':
            d = va_arg(va, long);
          ApiAbuse:
            if (p + 22 <= e) {
              if (d || !plus) {
                if (d > 0 && plus) {
                  *p++ = plus;
                }
                p = __intcpy(p, d);
              }
            }
            break;
          case 'p':
            w = 12;
            f = '0';
            /* fallthrough */
          case 'x':
            x = va_arg(va, unsigned long);
            if (x) {
              n = __builtin_clzl(x) ^ (sizeof(long) * 8 - 1);
              n >>= 2;
              n += 1;
            } else {
              n = 1;
            }
            while (w-- > n) {
              if (p < e) {
                *p++ = f;
              }
            }
            while (n--) {
              if (p < e) {
                *p++ = "0123456789abcdef"[(x >> (n << 2)) & 15];
              }
            }
            break;
          case 's':
            s = va_arg(va, const char *);
            if (!s) {
            EmitNullString:
              s = "NULL";
            }
            if ((uintptr_t)s < PAGESIZE) {
              d = (intptr_t)s;
              goto ApiAbuse;
            }
            for (n = 0; s[n];) ++n;
            while (w-- > n) {
              if (p < e) {
                *p++ = f;
              }
            }
            while ((t = *s++)) {
              if (p < e) {
                *p++ = t;
              }
            }
            break;
          case 'S':
            S = va_arg(va, const char16_t *);
            if (!S) goto EmitNullString;
            while ((t = *S++)) {
              if (p + 3 <= e && (t & 0xfc00) != 0xdc00) {
                if (t <= 0x7ff) {
                  p[0] = 0300 | t >> 6;
                  p[1] = 0200 | x << 8 | t & 077;
                  p += 2;
                } else {
                  if (t > 0xffff) t = 0xfffd;
                  p[0] = 0340 | t >> 12;
                  p[1] = 0200 | x << 8 | (t >> 6) & 077;
                  p[2] = 0200 | x << 8 | t & 077;
                  p += 3;
                }
              }
            }
            break;
          default:
            break;
        }
        break;
    }
  } while (c);
  va_end(va);
  if (p == e) {
    e[-4] = '.';
    e[-3] = '.';
    e[-2] = '.';
    e[-1] = '\n';
  }
  if (IsWindows()) {
    WriteFile(GetStdHandle(kNtStdErrorHandle), b, p - b, &wrote, 0);
  } else if (IsMetal()) {
    for (e = p, p = b; p < e; ++p) {
      for (;;) {
        dx = 0x3F8 + UART_LSR;
        asm("inb\t%1,%0" : "=a"(al) : "dN"(dx));
        if (al & UART_TTYTXR) break;
        asm("pause");
      }
      dx = 0x3F8;
      asm volatile("outb\t%0,%1"
                   : /* no inputs */
                   : "a"(*p), "dN"(dx));
    }
  } else {
    asm volatile("syscall"
                 : "=a"(ax)
                 : "0"(__NR_write), "D"(2), "S"(b), "d"(p - b)
                 : "rcx", "r11", "memory");
  }
}
