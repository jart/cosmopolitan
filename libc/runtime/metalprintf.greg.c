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
#include "libc/nexgen32e/uart.internal.h"
#include "libc/runtime/pc.internal.h"
#include "libc/runtime/runtime.h"

#ifdef __x86_64__

#define PUTC(C)                                      \
  do {                                               \
    while (!(inb(0x3F8 + UART_LSR) & UART_TTYTXR)) { \
      __builtin_ia32_pause();                        \
    }                                                \
    outb(0x3F8, C);                                  \
  } while (0)

/**
 * Prints string to serial port.
 *
 * This only supports %d and %s. It'll will work even if .rodata hasn't
 * been loaded into memory yet.
 */
textreal void(MetalPrintf)(const char *fmt, ...) {
  int i;
  char c;
  unsigned u;
  va_list va;
  const char *s;
  unsigned long d;
  va_start(va, fmt);
  for (;;) {
    switch ((c = *fmt++)) {
      case '\0':
        va_end(va);
        return;
      case '%':
        switch ((c = *fmt++)) {
          case 's':
            for (s = va_arg(va, const char *); s && *s; ++s) {
              PUTC(*s);
            }
            break;
          case 'd':
            d = va_arg(va, unsigned long);
            for (i = 16; i--;) {
              u = (d >> (i * 4)) & 0xf;
              if (u < 10) {
                c = '0' + u;
              } else {
                u -= 10;
                c = 'a' + u;
              }
              PUTC(c);
            }
            break;
          default:
            PUTC(c);
            break;
        }
        break;
      default:
        PUTC(c);
        break;
    }
  }
}

#endif /* __x86_64__ */
