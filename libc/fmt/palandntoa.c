/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=8 sts=2 sw=2 fenc=utf-8                                :vi│
╚══════════════════════════════════════════════════════════════════════════════╝
│ @author (c) Marco Paland (info@paland.com)                                   │
│             2014-2019, PALANDesign Hannover, Germany                         │
│                                                                              │
│ @license The MIT License (MIT)                                               │
│                                                                              │
│ Permission is hereby granted, free of charge, to any person obtaining a copy │
│ of this software and associated documentation files (the "Software"), to deal│
│ in the Software without restriction, including without limitation the rights │
│ to use, copy, modify, merge, publish, distribute, sublicense, and/or sell    │
│ copies of the Software, and to permit persons to whom the Software is        │
│ furnished to do so, subject to the following conditions:                     │
│                                                                              │
│ The above copyright notice and this permission notice shall be included in   │
│ all copies or substantial portions of the Software.                          │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR   │
│ IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,     │
│ FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  │
│ AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER       │
│ LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,│
│ OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN    │
│ THE SOFTWARE.                                                                │
└─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/paland.inc"
#include "libc/fmt/palandprintf.internal.h"

uintmax_t __udivmodti4(uintmax_t, uintmax_t, uintmax_t *);

static int ntoaformat(int out(long, void *), void *arg, char *buf, unsigned len,
                      bool negative, unsigned log2base, unsigned prec,
                      unsigned width, unsigned char flags) {
  unsigned i, idx;
  idx = 0;

  /* pad leading zeros */
  if (!(flags & FLAGS_LEFT)) {
    if (width && (flags & FLAGS_ZEROPAD) &&
        (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
      width--;
    }
    while ((len < prec) && (len < PRINTF_NTOA_BUFFER_SIZE)) {
      buf[len++] = '0';
    }
    while ((flags & FLAGS_ZEROPAD) && (len < width) &&
           (len < PRINTF_NTOA_BUFFER_SIZE)) {
      buf[len++] = '0';
    }
  }

  /* handle hash */
  if (flags & FLAGS_HASH) {
    if (!(flags & FLAGS_PRECISION) && len &&
        ((len == prec) || (len == width)) && buf[len - 1] == '0') {
      len--;
      if (len && (log2base == 4 || log2base == 1) && buf[len - 1] == '0') {
        len--;
      }
    }
    if (log2base == 4 && len < PRINTF_NTOA_BUFFER_SIZE) {
      buf[len++] = 'x';
    } else if (log2base == 1 && len < PRINTF_NTOA_BUFFER_SIZE) {
      buf[len++] = 'b';
    }
    if (len < PRINTF_NTOA_BUFFER_SIZE) {
      buf[len++] = '0';
    }
  }

  if (len < PRINTF_NTOA_BUFFER_SIZE) {
    if (negative) {
      buf[len++] = '-';
    } else if (flags & FLAGS_PLUS) {
      buf[len++] = '+'; /* ignore the space if the '+' exists */
    } else if (flags & FLAGS_SPACE) {
      buf[len++] = ' ';
    }
  }

  /* pad spaces up to given width */
  if (!(flags & FLAGS_LEFT) && !(flags & FLAGS_ZEROPAD)) {
    if (len < width) {
      if (spacepad(out, arg, width - len) == -1) return -1;
    }
  }

  /* reverse string */
  for (i = 0U; i < len; i++) {
    if (out(buf[len - i - 1], arg) == -1) return -1;
    idx++;
  }

  /* append pad spaces up to given width */
  if (flags & FLAGS_LEFT) {
    if (idx < width) {
      if (spacepad(out, arg, width - idx) == -1) return -1;
    }
  }
  return 0;
}

int ntoa2(int out(long, void *), void *arg, uintmax_t value, bool neg,
          unsigned log2base, unsigned prec, unsigned width, unsigned flags,
          const char *alphabet) {
  uintmax_t remainder;
  unsigned len, count, digit;
  char buf[PRINTF_NTOA_BUFFER_SIZE];
  len = 0;
  if (!value) flags &= ~FLAGS_HASH;
  if (value || !(flags & FLAGS_PRECISION)) {
    count = 0;
    do {
      assert(len < PRINTF_NTOA_BUFFER_SIZE);
      if (!log2base) {
        value = __udivmodti4(value, 10, &remainder);
        digit = remainder;
      } else {
        digit = value;
        digit &= (1u << log2base) - 1;
        value >>= log2base;
      }
      if ((flags & FLAGS_GROUPING) && count == 3) {
        buf[len++] = ',';
        count = 1;
      } else {
        count++;
      }
      buf[len++] = alphabet[digit];
    } while (value);
  }
  return ntoaformat(out, arg, buf, len, neg, log2base, prec, width, flags);
}

int ntoa(int out(long, void *), void *arg, va_list va, unsigned char signbit,
         unsigned long log2base, unsigned long prec, unsigned long width,
         unsigned char flags, const char *lang) {
  bool neg;
  uintmax_t value, sign;

  /* ignore '0' flag when prec is given */
  if (flags & FLAGS_PRECISION) {
    flags &= ~FLAGS_ZEROPAD;
  }

  /* no plus / space flag for u, x, X, o, b */
  if (!(flags & FLAGS_ISSIGNED)) {
    flags &= ~(FLAGS_PLUS | FLAGS_SPACE);
  }

  if (signbit > 63) {
    value = va_arg(va, uint128_t);
  } else {
    value = va_arg(va, uint64_t);
  }

  neg = 0;
  sign = 1;
  sign <<= signbit;
  value &= sign | (sign - 1);
  if (flags & FLAGS_ISSIGNED) {
    if (value != sign) {
      if (value & sign) {
        value = ~value + 1;
        value &= sign | (sign - 1);
        neg = 1;
      }
      value &= sign - 1;
    } else {
      neg = 1;
    }
  }

  return ntoa2(out, arg, value, neg, log2base, prec, width, flags, lang);
}
