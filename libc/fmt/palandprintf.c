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
│                                                                              │
│ @brief Modified by Justine Tunney to support three different types of        │
│        UNICODE, 128-bit arithmetic, binary conversion, string escaping,      │
│        AVX2 character scanning, and possibly a tinier footprint too, so      │
│        long as extremely wild linker hacks aren't considered cheating.       │
└─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/bits/bits.h"
#include "libc/bits/weaken.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/paland.inc"
#include "libc/fmt/palandprintf.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"

static int ppatoi(const char **str) {
  int i;
  for (i = 0; '0' <= **str && **str <= '9'; ++*str) {
    i *= 10;
    i += **str - '0';
  }
  return i;
}

/**
 * Implements {,v}{,s{,n},{,{,x}as},f,d}printf domain-specific language.
 *
 * Type Specifiers
 *
 *   - `%s`   char * (thompson-pike unicode)
 *   - `%ls`  wchar_t * (32-bit unicode → thompson-pike unicode)
 *   - `%hs`  char16_t * (16-bit unicode → thompson-pike unicode)
 *   - `%b`   int (radix 2 binary)
 *   - `%o`   int (radix 8 octal)
 *   - `%d`   int (radix 10 decimal)
 *   - `%x`   int (radix 16 hexadecimal)
 *   - `%X`   int (radix 16 hexadecimal uppercase)
 *   - `%u`   unsigned
 *   - `%f`   double
 *   - `%Lf`  long double
 *   - `%p`   pointer (48-bit hexadecimal)
 *
 * Size Modifiers
 *
 *   - `%hhd` char (8-bit)
 *   - `%hd`  short (16-bit)
 *   - `%ld`  long (64-bit)
 *   - `%lu`  unsigned long (64-bit)
 *   - `%lx`  unsigned long (64-bit hexadecimal)
 *   - `%jd`  intmax_t (128-bit)
 *
 * Width Modifiers
 *
 *   - `%08d` fixed columns w/ zero leftpadding
 *   - `%8d`  fixed columns w/ space leftpadding
 *   - `%*s`  variable column string (thompson-pike)
 *
 * Precision Modifiers
 *
 *   - `%.8s`    supplied byte length (obeys nul terminator)
 *   - `%.*s`    supplied byte length argument (obeys nul terminator)
 *   - ``%`.*s`` supplied byte length argument c escaped (ignores nul term)
 *   - `%#.*s`   supplied byte length argument visualized (ignores nul term)
 *   - `%.*hs`   supplied char16_t length argument (obeys nul terminator)
 *   - `%.*ls`   supplied wchar_t length argument (obeys nul terminator)
 *
 * Formatting Modifiers
 *
 *   - `%,d`    thousands separators
 *   - `%'s`    escaped c string literal
 *   - ``%`c``  c escaped character
 *   - ``%`'c`` c escaped character quoted
 *   - ``%`s``  c escaped string
 *   - ``%`'s`` c escaped string quoted
 *   - ``%`s``  escaped double quoted c string literal
 *   - ``%`c``  escaped double quoted c character literal
 *   - `%+d`    plus leftpad if positive (aligns w/ negatives)
 *   - `% d`    space leftpad if positive (aligns w/ negatives)
 *   - `%#s`    datum (radix 256 null-terminated ibm cp437)
 *   - `%#x`    int (radix 16 hexadecimal w/ 0x prefix if not zero)
 *
 * @note implementation detail of printf(), snprintf(), etc.
 * @see printf() for wordier documentation
 */
hidden int palandprintf(void *fn, void *arg, const char *format, va_list va) {
  void *p;
  char qchar;
  long double ldbl;
  wchar_t charbuf[1];
  const char *alphabet;
  int (*out)(long, void *);
  unsigned char signbit, log2base;
  int w, flags, width, lasterr, precision;

  lasterr = errno;
  out = fn ? fn : (int (*)(int, void *))missingno;

  while (*format) {
    /* %[flags][width][.precision][length] */
    if (*format != '%') {
      /* no */
      if (out(*format, arg) == -1) return -1;
      format++;
      continue;
    } else {
      /* yes, evaluate it */
      format++;
    }

    /* evaluate flags */
    flags = 0;
  getflag:
    switch (*format++) {
      case '0':
        flags |= FLAGS_ZEROPAD;
        goto getflag;
      case '-':
        flags |= FLAGS_LEFT;
        goto getflag;
      case '+':
        flags |= FLAGS_PLUS;
        goto getflag;
      case ' ':
        flags |= FLAGS_SPACE;
        goto getflag;
      case '#':
        flags |= FLAGS_HASH;
        goto getflag;
      case ',':
        flags |= FLAGS_GROUPING;
        goto getflag;
      case '`':
        flags |= FLAGS_REPR;
        /* fallthrough */
      case '\'':
        flags |= FLAGS_QUOTE;
        goto getflag;
      default:
        format--;
        break;
    }

    /* evaluate width field */
    width = 0;
    if (isdigit(*format)) {
      width = ppatoi(&format);
    } else if (*format == '*') {
      w = va_arg(va, int);
      if (w < 0) {
        flags |= FLAGS_LEFT; /* reverse padding */
        width = -w;
      } else {
        width = w;
      }
      format++;
    }

    /* evaluate precision field */
    precision = 0;
    if (*format == '.') {
      flags |= FLAGS_PRECISION;
      format++;
      if (isdigit(*format)) {
        precision = ppatoi(&format);
      } else if (*format == '*') {
        precision = va_arg(va, int);
        format++;
      }
    }
    if (precision < 0) {
      precision = 0;
    }

    /* evaluate length field */
    signbit = 31;
    switch (*format) {
      case 'j': /* intmax_t */
        format++;
        signbit = sizeof(intmax_t) * 8 - 1;
        break;
      case 'l':
        if (format[1] == 'l') format++;
        /* fallthrough */
      case 't': /* ptrdiff_t */
      case 'z': /* size_t */
      case 'Z': /* size_t */
      case 'L': /* long double */
        format++;
        signbit = 63;
        break;
      case 'h':
        format++;
        if (*format == 'h') {
          format++;
          signbit = 7;
        } else {
          signbit = 15;
        }
        break;
      default:
        break;
    }

    /* evaluate specifier */
    alphabet = "0123456789abcdef";
    log2base = 0;
    qchar = '"';
    switch (*format++) {
      case 'p':
        flags |= FLAGS_ZEROPAD;
        width = POINTER_XDIGITS;
        log2base = 4;
        signbit = 47;
        goto DoNumber;
      case 'X':
        alphabet = "0123456789ABCDEF";
        /* fallthrough */
      case 'x':
        log2base = 4;
        goto DoNumber;
      case 'b':
        log2base = 1;
        goto DoNumber;
      case 'o':
        log2base = 3;
        goto DoNumber;
      case 'd':
      case 'i':
        flags |= FLAGS_ISSIGNED;
        /* fallthrough */
      case 'u': {
        flags &= ~FLAGS_HASH; /* no hash for dec format */
      DoNumber:
        if (!weaken(ntoa) ||
            weaken(ntoa)(out, arg, va, signbit, log2base, precision, width,
                         flags, alphabet) == -1) {
          return -1;
        }
        break;
      }

      case 'f':
      case 'F':
        if (signbit == 63) {
          ldbl = va_arg(va, long double);
        } else {
          ldbl = va_arg(va, double);
        }
        if (!weaken(ftoa) ||
            weaken(ftoa)(out, arg, ldbl, precision, width, flags) == -1) {
          return -1;
        }
        break;

      case 'c':
        precision = 1;
        flags |= FLAGS_PRECISION;
        qchar = '\'';
        p = charbuf;
        charbuf[0] = va_arg(va, int); /* assume little endian */
        goto showstr;

      case 'm':
        p = weaken(strerror) ? weaken(strerror)(lasterr) : "?";
        signbit = 0;
        goto showstr;

      case 'r':
        flags |= FLAGS_REPR;
        /* fallthrough */

      case 'q':
        flags |= FLAGS_QUOTE;
        /* fallthrough */

      case 's':
        p = va_arg(va, void *);
      showstr:
        if (!weaken(stoa) || weaken(stoa)(out, arg, p, flags, precision, width,
                                          signbit, qchar) == -1) {
          return -1;
        }
        break;

      case '%':
        if (out('%', arg) == -1) return -1;
        break;

      default:
        if (out(format[-1], arg) == -1) return -1;
        break;
    }
  }
  return 0;
}
