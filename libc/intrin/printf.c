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
#include "libc/limits.h"
#include "libc/log/libfatal.internal.h"
#include "libc/nexgen32e/uart.internal.h"
#include "libc/nt/runtime.h"
#include "libc/runtime/runtime.h"
#include "libc/str/tpenc.h"
#include "libc/sysv/consts/nr.h"

/**
 * Privileged vprintf.
 *
 * This will work without any cosmopolitan runtime support once the
 * executable has been loaded into memory.
 */
privileged noasan noubsan noinstrument void __vprintf(const char *fmt,
                                                      va_list va) {
  short w[2];
  uint16_t dx;
  const void *s;
  uint32_t wrote;
  unsigned long x;
  unsigned char al;
  int i, j, t, cstr;
  long d, rax, rdi, rsi, rdx, dot;
  char c, *p, *e, pad, bits, base, sign, thou, z[28], b[2048];
  p = b;
  e = p + sizeof(b);
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
        dot = 0;
        pad = ' ';
        sign = 0;
        bits = 0;
        thou = 0;
        w[0] = 0;
        w[1] = SHRT_MAX;
      NeedMoar:
        switch ((c = *fmt++)) {
          case '\0':
            break;
          case 'l':
          case 'z':
            goto NeedMoar;
          case ' ':
          case '+':
            sign = c;
            goto NeedMoar;
          case 'e':
            dot = 1;
            goto NeedMoar;
          case ',':
            thou = c;
            goto NeedMoar;
          case 'h':
            bits = 16;
            goto NeedMoar;
          case '0':
            pad = c;
            /* fallthrough */
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            w[dot] *= 10;
            w[dot] += c - '0';
            goto NeedMoar;
          case '*':
            w[dot] = va_arg(va, int);
            goto NeedMoar;
          case 'd':
            d = va_arg(va, long);
          ApiAbuse:
            x = d;
            if (d < 0) {
              x = -x;
              sign = '-';
            }
            for (i = j = 0;;) {
              z[i++] = x % 10 + '0';
              if (!(x /= 10)) break;
              if (thou && !(++j % 3)) {
                z[i++] = thou;
              }
            }
            if (sign) {
              z[i++] = sign;
            }
          EmitNumber:
            while (w[0]-- > i) {
              if (p < e) *p++ = pad;
            }
            do {
              if (p < e) *p++ = z[--i];
            } while (i);
            break;
          case 'b':
            base = 1;
          BinaryNumber:
            i = 0;
            x = va_arg(va, unsigned long);
            do z[i++] = "0123456789abcdef"[x & ((1 << base) - 1)];
            while ((x >>= base) && i < w[1]);
            goto EmitNumber;
          case 'p':
            pad = '0';
            w[0] = 12;
            w[1] = 12;
            /* fallthrough */
          case 'x':
            base = 4;
            goto BinaryNumber;
          case 'o':
            base = 3;
            goto BinaryNumber;
          case 'c':
            cstr = va_arg(va, int);
            s = &cstr;
            goto EmitString;
          case 's':
            s = va_arg(va, const void *);
          EmitString:
            if (!s) {
              s = "NULL";
              bits = 0;
            } else if ((uintptr_t)s < PAGESIZE) {
              d = (intptr_t)s;
              goto ApiAbuse;
            }
            for (i = 0; i < w[1]; ++i) {
              if (!bits) {
                t = ((const char *)s)[i];
              EmitByte:
                if (t) {
                  if (p < e) {
                    *p++ = t;
                  }
                } else {
                  break;
                }
              } else {
                t = ((const char16_t *)s)[i];
                if (t <= 0x7f) {
                  goto EmitByte;
                } else if (t <= 0x7ff) {
                  if (p + 1 < e) {
                    p[0] = 0300 | t >> 6;
                    p[1] = 0200 | x << 8 | t & 077;
                    p += 2;
                  }
                } else if (p + 2 < e) {
                  p[0] = 0340 | t >> 12;
                  p[1] = 0200 | x << 8 | (t >> 6) & 077;
                  p[2] = 0200 | x << 8 | t & 077;
                  p += 3;
                }
              }
            }
            while (w[0]-- > i) {
              if (p < e) *p++ = pad;
            }
            break;
          default:
            break;
        }
        break;
    }
  } while (c);
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
                 : "=a"(rax), "=D"(rdi), "=S"(rsi), "=d"(rdx)
                 : "0"(__NR_write), "1"(2L), "2"(b), "3"(p - b)
                 : "rcx", "r8", "r9", "r10", "r11", "memory", "cc");
  }
}

/**
 * Privileged printf.
 *
 * This will work without any cosmopolitan runtime support once the
 * executable has been loaded into memory.
 */
privileged noasan noubsan noinstrument void __printf(const char *fmt, ...) {
  /* system call support runtime depends on this function */
  /* function tracing runtime depends on this function */
  /* asan runtime depends on this function */
  va_list va;
  va_start(va, fmt);
  __vprintf(fmt, va);
  va_end(va);
}
