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
#include "libc/bits/bits.h"
#include "libc/bits/weaken.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/fmts.h"
#include "libc/fmt/internal.h"
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
 * @note netlib.org is so helpful
 * @asyncsignalsafe
 * @vforksafe
 */
hidden int __fmt(void *fn, void *arg, const char *format, va_list va) {
  union {
    double d;
    unsigned int u[2];
  } pun;
  void *p;
  char qchar;
  char *s, *se;
  bool longdouble;
  long double ldbl;
  wchar_t charbuf[1];
  const char *alphabet;
  int (*out)(long, void *);
  unsigned char signbit, log2base;
  int c, d, k, w, i1, ui, bw, bex;
  int sgn, alt, sign, prec, prec1, flags, width, decpt, lasterr;

  lasterr = errno;
  out = fn ? fn : (void *)missingno;

  while (*format) {
    /* %[flags][width][.prec][length] */
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
      width = __fmt_atoi(&format);
    } else if (*format == '*') {
      w = va_arg(va, int);
      if (w < 0) {
        flags |= FLAGS_LEFT; /* reverse padding */
        width = -w;
        sign = '-';
      } else {
        width = w;
      }
      format++;
    }

    /* evaluate prec field */
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

    /* evaluate length field */
    signbit = 31;
    longdouble = false;
    switch (*format) {
      case 'j': /* intmax_t */
        format++;
        signbit = sizeof(intmax_t) * 8 - 1;
        break;
      case 'l':
        if (format[1] == 'f' || format[1] == 'F') {
          format++;
          break;
        }
        if (format[1] == 'l') format++;
        /* fallthrough */
      case 't': /* ptrdiff_t */
      case 'z': /* size_t */
      case 'Z': /* size_t */
        format++;
        signbit = 63;
        break;
      case 'L': /* long double */
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

    /* evaluate specifier */
    qchar = '"';
    log2base = 0;
    alphabet = "0123456789abcdef";
    switch ((d = *format++)) {
      case 'p':
        flags |= FLAGS_ZEROPAD;
        width = POINTER_XDIGITS;
        log2base = 4;
        signbit = 47;
        goto FormatNumber;
      case 'X':
        alphabet = "0123456789ABCDEF";
        /* fallthrough */
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
        /* fallthrough */
      case 'u': {
        flags &= ~FLAGS_HASH; /* no hash for dec format */
      FormatNumber:
        if (__fmt_ntoa(out, arg, va, signbit, log2base, prec, width, flags,
                       alphabet) == -1) {
          return -1;
        }
        break;
      }

      case 'c':
        prec = 1;
        flags |= FLAGS_PRECISION;
        qchar = '\'';
        p = charbuf;
        charbuf[0] = va_arg(va, int);
        goto FormatString;

      case 'm':
        p = weaken(strerror) ? weaken(strerror)(lasterr) : "?";
        signbit = 0;
        goto FormatString;

      case 'r':
        flags |= FLAGS_REPR;
        /* fallthrough */

      case 'q':
        flags |= FLAGS_QUOTE;
        /* fallthrough */

      case 's':
        p = va_arg(va, void *);
      FormatString:
        if (__fmt_stoa(out, arg, p, flags, prec, width, signbit, qchar) == -1) {
          return -1;
        }
        break;

      case 'f':
        if (!(flags & FLAGS_PRECISION)) prec = 6;
        if (longdouble) {
          pun.d = va_arg(va, long double);
        } else {
          pun.d = va_arg(va, double);
        }
      FormatDtoa:
        if (!weaken(__fmt_dtoa)) {
          p = "nan";
          goto FormatString;
        }
        s = weaken(__fmt_dtoa)(pun.d, 3, prec, &decpt, &sgn, &se);
        if (decpt == 9999) {
        Format9999:
          prec = alt = 0;
          flags &= ~FLAGS_PRECISION;
          if (*s == 'N') {
            p = s;
            goto FormatString;
          }
          decpt = strlen(s);
        }
      FormatReal:
        if (sgn) sign = '-';
        if (prec > 0) width -= prec;
        if (width > 0) {
          if (sign) --width;
          if (decpt <= 0) {
            --width;
            if (prec > 0) --width;
          } else {
            if (s == se) decpt = 1;
            width -= decpt;
            if (prec > 0 || alt) --width;
          }
        }
        if (width > 0 && !(flags & FLAGS_LEFT)) {
          if (flags & FLAGS_ZEROPAD) {
            if (sign) out(sign, arg);
            sign = 0;
            do out('0', arg);
            while (--width > 0);
          } else
            do out(' ', arg);
            while (--width > 0);
        }
        if (sign) out(sign, arg);
        if (decpt <= 0) {
          out('0', arg);
          if (prec > 0 || alt) out('.', arg);
          while (decpt < 0) {
            out('0', arg);
            prec--;
            decpt++;
          }
        } else {
          do {
            if ((c = *s)) {
              s++;
            } else {
              c = '0';
            }
            out(c, arg);
          } while (--decpt > 0);
          if (prec > 0 || alt) out('.', arg);
        }
        while (--prec >= 0) {
          if ((c = *s)) {
            s++;
          } else {
            c = '0';
          }
          out(c, arg);
        }
        while (--width >= 0) {
          out(' ', arg);
        }
        continue;

      case 'G':
      case 'g':
        if (!(flags & FLAGS_PRECISION)) prec = 6;
        if (longdouble) {
          pun.d = va_arg(va, long double);
        } else {
          pun.d = va_arg(va, double);
        }
        if (prec < 0) prec = 0;
        if (!weaken(__fmt_dtoa)) {
          p = "nan";
          goto FormatString;
        }
        s = weaken(__fmt_dtoa)(pun.d, prec ? 2 : 0, prec, &decpt, &sgn, &se);
        if (decpt == 9999) goto Format9999;
        c = se - s;
        prec1 = prec;
        if (!prec) {
          prec = c;
          prec1 = c + (s[1] || alt ? 5 : 4);
          /* %.0g gives 10 rather than 1e1 */
        }
        if (decpt > -4 && decpt <= prec1) {
          if (alt) {
            prec -= decpt;
          } else {
            prec = c - decpt;
          }
          if (prec < 0) prec = 0;
          goto FormatReal;
        }
        d -= 2;
        if (!alt && prec > c) prec = c;
        --prec;
        goto FormatExpo;

      case 'e':
      case 'E':
        if (!(flags & FLAGS_PRECISION)) prec = 6;
        if (longdouble) {
          pun.d = va_arg(va, long double);
        } else {
          pun.d = va_arg(va, double);
        }
        if (prec < 0) prec = 0;
        if (!weaken(__fmt_dtoa)) {
          p = "nan";
          goto FormatString;
        }
        s = weaken(__fmt_dtoa)(pun.d, 2, prec + 1, &decpt, &sgn, &se);
        if (decpt == 9999) goto Format9999;
      FormatExpo:
        if (sgn) sign = '-';
        if ((width -= prec + 5) > 0) {
          if (sign) --width;
          if (prec || alt) --width;
        }
        if ((c = --decpt) < 0) c = -c;
        while (c >= 100) {
          --width;
          c /= 10;
        }
        if (width > 0 && !(flags & FLAGS_LEFT)) {
          if (flags & FLAGS_ZEROPAD) {
            if (sign) out(sign, arg);
            sign = 0;
            do out('0', arg);
            while (--width > 0);
          } else {
            do out(' ', arg);
            while (--width > 0);
          }
        }
        if (sign) out(sign, arg);
        out(*s++, arg);
        if (prec || alt) out('.', arg);
        while (--prec >= 0) {
          if ((c = *s)) {
            s++;
          } else {
            c = '0';
          }
          out(c, arg);
        }
        out(d, arg);
        if (decpt < 0) {
          out('-', arg);
          decpt = -decpt;
        } else {
          out('+', arg);
        }
        for (c = 2, k = 10; 10 * k <= decpt; c++, k *= 10) {
        }
        for (;;) {
          i1 = decpt / k;
          out(i1 + '0', arg);
          if (--c <= 0) break;
          decpt -= i1 * k;
          decpt *= 10;
        }
        while (--width >= 0) {
          out(' ', arg);
        }
        continue;

      case 'a':
        alphabet = "0123456789abcdefpx";
        goto FormatBinary;
      case 'A':
        alphabet = "0123456789ABCDEFPX";
      FormatBinary:
        if (longdouble) {
          pun.d = va_arg(va, long double);
        } else {
          pun.d = va_arg(va, double);
        }
        if ((pun.u[1] & 0x7ff00000) == 0x7ff00000) {
          goto FormatDtoa;
        }
        if (pun.d) {
          c = '1';
          if (pun.u[1] & 0x80000000) {
            sign = '-';
            pun.u[1] &= 0x7fffffff;
          }
          bex = (pun.u[1] >> 20) - 1023;
          pun.u[1] &= 0xfffff;
          if (bex == -1023) {
            ++bex;
            if (pun.u[1]) {
              do {
                --bex;
                pun.u[1] <<= 1;
                if (pun.u[0] & 0x80000000) pun.u[1] |= 1;
                pun.u[0] <<= 1;
              } while (pun.u[1] < 0x100000);
            } else {
              while (!(pun.u[0] & 0x80000000)) {
                --bex;
                pun.u[0] <<= 1;
              }
              bex -= 21;
              pun.u[1] = pun.u[0] >> 11;
              pun.u[0] <<= 21;
            }
          }
        } else {
          c = '0';
          bex = 0;
        }
        if (flags & FLAGS_PRECISION) {
          if (prec > 13) prec = 13;
          if (pun.d && prec < 13) {
            pun.u[1] |= 0x100000;
            if (prec < 5) {
              ui = 1 << ((5 - prec) * 4 - 1);
              if (pun.u[1] & ui) {
                if (pun.u[1] & ((ui - 1) | (ui << 1)) || pun.u[0]) {
                  pun.u[1] += ui;
                BexCheck:
                  if (pun.u[1] & 0x200000) {
                    ++bex;
                    pun.u[1] >>= 1;
                  }
                }
              }
            } else if (prec == 5) {
              if (pun.u[0] & 0x80000000) {
              BumpIt:
                ++pun.u[1];
                goto BexCheck;
              }
            } else {
              i1 = (13 - prec) * 4;
              ui = 1 << (i1 - 1);
              if (pun.u[0] & ui && pun.u[0] & ((ui - 1) | (ui << 1))) {
                pun.u[0] += ui;
                if (!(pun.u[0] >> i1)) goto BumpIt;
              }
            }
          }
        } else {
          if ((ui = pun.u[0])) {
            for (prec = 6; (ui = (ui << 4) & 0xffffffff); ++prec) {
            }
          } else {
            for (prec = 0, ui = pun.u[1] & 0xfffff; ui;
                 ++prec, ui = (ui << 4) & 0xfffff) {
            }
          }
        }
        bw = 1;
        if (bex) {
          if ((i1 = bex) < 0) i1 = -i1;
          while (i1 >= 10) {
            ++bw;
            i1 /= 10;
          }
        }
        if ((sgn = pun.u[1] & 0x80000000)) {
          pun.u[1] &= 0x7fffffff;
          if (pun.d || sign) sign = '-';
        }
        if ((width -= bw + 5) > 0) {
          if (sign) --width;
          if (prec || alt) --width;
        }
        if (width > 0 && !(flags & FLAGS_LEFT)) {
          if (flags & FLAGS_ZEROPAD) {
            if (sign) {
              out(sign, arg);
              sign = 0;
            }
            do out('0', arg);
            while (--width > 0);
          } else {
            do out(' ', arg);
            while (--width > 0);
          }
        }
        if (sign) out(sign, arg);
        out('0', arg);
        out(alphabet[17], arg);
        out(c, arg);
        if (prec > 0 || alt) out('.', arg);
        if (prec > 0) {
          if ((i1 = prec) > 5) i1 = 5;
          prec -= i1;
          do {
            out(alphabet[(pun.u[1] >> 16) & 0xf], arg);
            pun.u[1] <<= 4;
          } while (--i1 > 0);
          while (prec > 0) {
            --prec;
            out(alphabet[(pun.u[0] >> 28) & 0xf], arg);
            pun.u[0] <<= 4;
          }
        }
        out(alphabet[16], arg);
        if (bex < 0) {
          out('-', arg);
          bex = -bex;
        } else {
          out('+', arg);
        }
        for (c = 1; 10 * c <= bex; c *= 10) {
        }
        for (;;) {
          i1 = bex / c;
          out('0' + i1, arg);
          if (!--bw) break;
          bex -= i1 * c;
          bex *= 10;
        }
        continue;

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
