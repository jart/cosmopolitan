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
│                                                                              │
│ @brief Tiny printf, sprintf and (v)snprintf implementation, optimized for    │
│        embedded systems with a very limited resources. These routines are    │
│        thread safe and reentrant! Use this instead of the bloated            │
│        standard/newlib printf cause these use malloc for printf (and may not │
│        be thread safe).                                                      │
└─────────────────────────────────────────────────────────────────────────────*/
#include "libc/fmt/paland.inc"
#include "libc/fmt/palandprintf.internal.h"
#include "libc/math.h"

/**
 * Formats floating point number.
 *
 * @see xdtoa() for higher precision at the cost of bloat
 * @see palandprintf() which is intended caller
 */
int ftoa(int out(long, void *), void *arg, long double value, int prec,
         unsigned long width, unsigned long flags) {
  long whole, frac;
  long double tmp, diff;
  unsigned i, len, count, idx;
  char buf[PRINTF_FTOA_BUFFER_SIZE];

  len = 0;
  diff = 0;

  if (isnan(value)) {
    buf[0] = 'n';
    buf[1] = 'a';
    buf[2] = 'n';
    buf[3] = '\0';
    len += 3;
  } else if (isinf(value) || (value && ilogbl(fabsl(value)) > 63)) {
    buf[0] = 'f';
    buf[1] = 'n';
    buf[2] = 'i';
    buf[3] = '\0';
    len += 3;
  } else {

    /* set default precision to 6, if not set explicitly */
    if (!(flags & FLAGS_PRECISION)) {
      prec = 6;
    }

    while (len < PRINTF_FTOA_BUFFER_SIZE && prec > 14) {
      buf[len++] = '0';
      prec--;
    }

    whole = truncl(fabsl(value));
    tmp = (fabsl(value) - whole) * exp10l(prec);
    frac = tmp;
    diff = tmp - frac;

    if (diff > .5) {
      ++frac; /* handle rollover, e.g. case 0.99 with prec 1 is 1.0 */
      if (frac >= exp10l(prec)) {
        frac = 0;
        ++whole;
      }
    } else if (diff < .5) {
    } else if (!frac || (frac & 1)) {
      ++frac; /* if halfway, round up if odd OR if last digit is 0 */
    }

    if (!prec) {
      diff = fabsl(value) - whole;
      if ((!(diff < .5) || (diff > .5)) && (whole & 1)) {
        /* exactly .5 and ODD, then round up */
        /* 1.5 -> 2, but 2.5 -> 2 */
        ++whole;
      }
    } else {
      count = prec;
      /* now do fractional part, as an unsigned number */
      while (len < PRINTF_FTOA_BUFFER_SIZE) {
        --count;
        buf[len++] = 48 + (frac % 10);
        if (!(frac /= 10)) {
          break;
        }
      }
      /* add extra 0s */
      while ((len < PRINTF_FTOA_BUFFER_SIZE) && (count-- > 0)) {
        buf[len++] = '0';
      }
      if (len < PRINTF_FTOA_BUFFER_SIZE) {
        /* add decimal */
        buf[len++] = '.';
      }
    }

    /* do whole part, number is reversed */
    while (len < PRINTF_FTOA_BUFFER_SIZE) {
      buf[len++] = (char)(48 + (whole % 10));
      if (!(whole /= 10)) {
        break;
      }
    }

    /* pad leading zeros */
    if (!(flags & FLAGS_LEFT) && (flags & FLAGS_ZEROPAD)) {
      if (width && (signbit(value) || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
        width--;
      }
      while ((len < width) && (len < PRINTF_FTOA_BUFFER_SIZE)) {
        buf[len++] = '0';
      }
    }
  }

  if (len < PRINTF_FTOA_BUFFER_SIZE) {
    if (signbit(value)) {
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
  for (idx = i = 0; i < len; i++) {
    if (out(buf[len - i - 1U], arg) == -1) return -1;
    idx++;
  }

  /* append pad spaces up to given width */
  if (flags & FLAGS_LEFT) {
    if (len < width) {
      if (spacepad(out, arg, width - len) == -1) return -1;
    }
  }

  return 0;
}
