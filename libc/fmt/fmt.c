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
#include "libc/assert.h"
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/fmt.internal.h"
#include "libc/fmt/internal.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/bits.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/intrin/weaken.h"
#include "libc/macros.internal.h"
#include "libc/mem/mem.h"
#include "libc/runtime/internal.h"
#include "libc/str/str.h"
#include "libc/sysv/errfuns.h"
#include "third_party/gdtoa/gdtoa.h"

static int __fmt_atoi(const char **str) {
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
 *   - `%p`   pointer (48-bit hexadecimal)
 *   - `%u`   unsigned
 *   - `%g`   double (smart formatting)
 *   - `%e`   double (expo formatting)
 *   - `%f`   double (ugly formatting)
 *   - `%a`   double (hex formatting)
 *   - `%Lg`  long double
 *
 * Size Modifiers
 *
 *   - `%hhd` char (8-bit)
 *   - `%hd`  short (16-bit)
 *   - `%ld`  long (64-bit)
 *   - `%lu`  unsigned long (64-bit)
 *   - `%lx`  unsigned long (64-bit hexadecimal)
 *   - `%jd`  intmax_t (64-bit)
 *   - `%jjd` int128_t (128-bit)
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
 * @note netlib.org is so helpful
 * @asyncsignalsafe if floating point isn't used
 * @vforksafe if floating point isn't used
 */
_Hide int __fmt(void *fn, void *arg, const char *format, va_list va) {
  long ld;
  void *p;
  unsigned u;
  char ibuf[21];
  bool longdouble;
  unsigned long lu;
  wchar_t charbuf[1];
  char *s, *q, qchar;
  const char *alphabet;
  unsigned char signbit, log2base;
  int (*out)(const char *, void *, size_t);
  int d, w, n, sign, prec, flags, width, lasterr;

  lasterr = errno;
  out = fn ? fn : (void *)_missingno;

  while (*format) {
    if (*format != '%') {
      for (n = 1; format[n]; ++n) {
        if (format[n] == '%') break;
      }
      if (out(format, arg, n) == -1) return -1;
      format += n;
      continue;
    }

    if (!IsTiny()) {
      if (format[1] == 's') {  // FAST PATH: PLAIN STRING
        s = va_arg(va, char *);
        if (!s) s = "(null)";
        if (out(s, arg, strlen(s)) == -1) return -1;
        format += 2;
        continue;
      } else if (format[1] == 'd') {  // FAST PATH: PLAIN INTEGER
        d = va_arg(va, int);
        if (out(ibuf, arg, FormatInt32(ibuf, d) - ibuf) == -1) return -1;
        format += 2;
        continue;
      } else if (format[1] == 'u') {  // FAST PATH: PLAIN UNSIGNED
        u = va_arg(va, unsigned);
        if (out(ibuf, arg, FormatUint32(ibuf, u) - ibuf) == -1) return -1;
        format += 2;
        continue;
      } else if (format[1] == 'x') {  // FAST PATH: PLAIN HEX
        u = va_arg(va, unsigned);
        if (out(ibuf, arg, uint64toarray_radix16(u, ibuf)) == -1) return -1;
        format += 2;
        continue;
      } else if (format[1] == 'l' && format[2] == 'x') {
        lu = va_arg(va, unsigned long);  // FAST PATH: PLAIN LONG HEX
        if (out(ibuf, arg, uint64toarray_radix16(lu, ibuf)) == -1) return -1;
        format += 3;
        continue;
      } else if (format[1] == 'l' && format[2] == 'd') {
        ld = va_arg(va, long);  // FAST PATH: PLAIN LONG
        if (out(ibuf, arg, FormatInt64(ibuf, ld) - ibuf) == -1) return -1;
        format += 3;
        continue;
      } else if (format[1] == 'l' && format[2] == 'u') {
        lu = va_arg(va, unsigned long);  // FAST PATH: PLAIN UNSIGNED LONG
        if (out(ibuf, arg, FormatUint64(ibuf, lu) - ibuf) == -1) return -1;
        format += 3;
        continue;
      } else if (format[1] == '.' && format[2] == '*' && format[3] == 's') {
        n = va_arg(va, unsigned);  // FAST PATH: PRECISION STRING
        s = va_arg(va, const char *);
        if (s) {
          n = strnlen(s, n);
        } else {
          s = "(null)";
          n = MIN(6, n);
        }
        if (out(s, arg, n) == -1) return -1;
        format += 4;
        continue;
      }
    }

    // GENERAL PATH
    format++;
    sign = 0;
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
        sign = '+';
        flags |= FLAGS_PLUS;
        goto getflag;
      case ' ':
        sign = ' ';
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
        // fallthrough
      case '\'':
        flags |= FLAGS_QUOTE;
        goto getflag;
      default:
        format--;
        break;
    }

    // evaluate width field
    width = 0;
    if (isdigit(*format)) {
      width = __fmt_atoi(&format);
    } else if (*format == '*') {
      w = va_arg(va, int);
      if (w < 0) {
        flags |= FLAGS_LEFT;  // reverse padding
        width = -w;
        sign = '-';
      } else {
        width = w;
      }
      format++;
    }

    // evaluate prec field
    prec = 0;
    if (*format == '.') {
      flags |= FLAGS_PRECISION;
      format++;
      if (isdigit(*format)) {
        prec = __fmt_atoi(&format);
      } else if (*format == '*') {
        prec = va_arg(va, int);
        format++;
      }
    }
    if (prec < 0) {
      prec = 0;
    }

    // evaluate length field
    signbit = 31;
    longdouble = false;
    switch (*format) {
      case 'j':  // intmax_t
        format++;
        signbit = sizeof(intmax_t) * 8 - 1;
        if (*format == 'j') {
          format++;
          signbit = sizeof(int128_t) * 8 - 1;
        }
        break;
      case 'l':
        if (format[1] == 'f' || format[1] == 'F') {
          format++;
          break;
        }
        if (format[1] == 'l') format++;
        // fallthrough
      case 't':  // ptrdiff_t
      case 'z':  // size_t
      case 'Z':  // size_t
        format++;
        signbit = 63;
        break;
      case 'L':  // long double
        format++;
        longdouble = true;
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

    // evaluate specifier
    qchar = '"';
    log2base = 0;
    alphabet = "0123456789abcdefpx";
    switch ((d = *format++)) {
      case 'p':
        flags |= FLAGS_HASH;
        log2base = 4;
        signbit = 63;
        goto FormatNumber;
      case 'X':
        alphabet = "0123456789ABCDEFPX";
        // fallthrough
      case 'x':
        log2base = 4;
        goto FormatNumber;
      case 'b':
        log2base = 1;
        goto FormatNumber;
      case 'o':
        log2base = 3;
        goto FormatNumber;
      case 'd':
      case 'i':
        flags |= FLAGS_ISSIGNED;
        // fallthrough
      case 'u': {
        flags &= ~FLAGS_HASH;  // no hash for dec format
      FormatNumber:
        if (__fmt_ntoa(out, arg, va, signbit, log2base, prec, width, flags,
                       alphabet) == -1) {
          return -1;
        }
        break;
      }
      case 'c':
        if ((charbuf[0] = va_arg(va, int))) {
          p = charbuf;
          qchar = '\'';
          flags |= FLAGS_PRECISION;
          prec = 1;
          goto FormatString;
        } else {
          __FMT_PUT('\0');
          break;
        }
      case 'm':
        p = _weaken(strerror) ? _weaken(strerror)(lasterr) : "?";
        signbit = 0;
        goto FormatString;
      case 'r':
        // undocumented %r specifier
        // used for good carriage return
        // helps integrate loggers with repls
        if (!__replstderr) {
          break;
        } else {
          p = "\r\e[K";
          goto FormatString;
        }
      case 'q':
        flags |= FLAGS_QUOTE;
        // fallthrough
      case 's':
        p = va_arg(va, void *);
      FormatString:
        if (__fmt_stoa(out, arg, p, flags, prec, width, signbit, qchar) == -1) {
          return -1;
        }
        break;
      case 'n':
        __FMT_PUT('\n');
        break;
      case 'F':
      case 'f':
      case 'G':
      case 'g':
      case 'e':
      case 'E':
      case 'a':
      case 'A':
        if (!_weaken(__fmt_dtoa)) {
          p = "?";
          prec = 0;
          flags &= ~(FLAGS_PRECISION | FLAGS_PLUS | FLAGS_SPACE);
          goto FormatString;
        }
        if (_weaken(__fmt_dtoa)(out, arg, d, flags, prec, sign, width,
                                longdouble, qchar, signbit, alphabet,
                                va) == -1) {
          return -1;
        }
        break;
      case '%':
        __FMT_PUT('%');
        break;
      default:
        __FMT_PUT(format[-1]);
        break;
    }
  }

  return 0;
}
