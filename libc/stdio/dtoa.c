/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
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
#include "libc/fmt/fmt.h"
#include "libc/fmt/fmt.internal.h"
#include "libc/fmt/internal.h"
#include "libc/macros.internal.h"
#include "libc/nexgen32e/bsr.h"
#include "third_party/gdtoa/gdtoa.h"

static const char kSpecialFloats[2][2][4] = {{"INF", "inf"}, {"NAN", "nan"}};

static void __fmt_free_dtoa(char **mem) {
  if (*mem) {
    freedtoa(*mem);
    *mem = 0;
  }
}

int __fmt_dtoa(int (*out)(const char *, void *, size_t), void *arg, int d,
               int flags, int prec, int sign, int width, bool longdouble,
               char qchar, unsigned char signbit, const char *alphabet,
               va_list va) {
  union {
    double d;
    uint32_t u[2];
    uint64_t q;
  } pun;
  int c, k, i1, ui, bw, bex, sgn, prec1, decpt;
  char *s, *p, *q, *se, *mem, special[8];
  mem = 0;
  switch (d) {
    case 'F':
    case 'f':
      if (!(flags & FLAGS_PRECISION)) prec = 6;
      if (longdouble) {
        pun.d = va_arg(va, long double);
      } else {
        pun.d = va_arg(va, double);
      }
    FormatDtoa:
      assert(!mem);
      s = mem = dtoa(pun.d, 3, prec, &decpt, &sgn, &se);
      if (decpt == 9999) {
      Format9999:
        bzero(special, sizeof(special));
        p = q = special;
        if (sgn) {
          *q++ = '-';
        } else if (flags & FLAGS_PLUS) {
          *q++ = '+';
        } else if (flags & FLAGS_SPACE) {
          *q++ = ' ';
        }
        memcpy(q, kSpecialFloats[*s == 'N'][d >= 'a'], 4);
      FormatThatThing:
        __fmt_free_dtoa(&mem);
        mem = 0;
        prec = 0;
        flags &= ~(FLAGS_PRECISION | FLAGS_PLUS | FLAGS_SPACE);
        goto FormatString;
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
          if (prec > 0) --width;
        }
      }
      if (width > 0 && !(flags & FLAGS_LEFT)) {
        if (flags & FLAGS_ZEROPAD) {
          if (sign) __FMT_PUT(sign);
          sign = 0;
          do
            __FMT_PUT('0');
          while (--width > 0);
        } else {
          do
            __FMT_PUT(' ');
          while (--width > 0);
        }
      }
      if (sign) __FMT_PUT(sign);
      if (decpt <= 0) {
        __FMT_PUT('0');
        if (prec > 0) __FMT_PUT('.');
        while (decpt < 0) {
          __FMT_PUT('0');
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
          __FMT_PUT(c);
        } while (--decpt > 0);
        if (prec > 0) __FMT_PUT('.');
      }
      while (--prec >= 0) {
        if ((c = *s)) {
          s++;
        } else {
          c = '0';
        }
        __FMT_PUT(c);
      }
      while (--width >= 0) {
        __FMT_PUT(' ');
      }
      __fmt_free_dtoa(&mem);
      break;

    case 'G':
    case 'g':
      if (!(flags & FLAGS_PRECISION)) prec = 6;
      if (longdouble) {
        pun.d = va_arg(va, long double);
      } else {
        pun.d = va_arg(va, double);
      }
      if (prec < 0) prec = 0;
      assert(!mem);
      s = mem = dtoa(pun.d, prec ? 2 : 0, prec, &decpt, &sgn, &se);
      if (decpt == 9999) goto Format9999;
      c = se - s;
      prec1 = prec;
      if (!prec) {
        prec = c;
        prec1 = c + (s[1] ? 5 : 4);
      }
      if (decpt > -4 && decpt <= prec1) {
        prec = c - decpt;
        if (prec < 0) prec = 0;
        goto FormatReal;
      }
      d -= 2;
      if (prec > c) prec = c;
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
      if (!dtoa) {
        p = "?";
        goto FormatThatThing;
      }
      assert(!mem);
      s = mem = dtoa(pun.d, 2, prec + 1, &decpt, &sgn, &se);
      if (decpt == 9999) goto Format9999;
    FormatExpo:
      if (sgn) sign = '-';
      if ((width -= prec + 5) > 0) {
        if (sign) --width;
        if (prec) --width;
      }
      if ((c = --decpt) < 0) c = -c;
      while (c >= 100) {
        --width;
        c /= 10;
      }
      if (width > 0 && !(flags & FLAGS_LEFT)) {
        if (flags & FLAGS_ZEROPAD) {
          if (sign) __FMT_PUT(sign);
          sign = 0;
          do
            __FMT_PUT('0');
          while (--width > 0);
        } else {
          do
            __FMT_PUT(' ');
          while (--width > 0);
        }
      }
      if (sign) __FMT_PUT(sign);
      __FMT_PUT(*s++);
      if (prec) __FMT_PUT('.');
      while (--prec >= 0) {
        if ((c = *s)) {
          s++;
        } else {
          c = '0';
        }
        __FMT_PUT(c);
      }
      __fmt_free_dtoa(&mem);
      __FMT_PUT(d);
      if (decpt < 0) {
        __FMT_PUT('-');
        decpt = -decpt;
      } else {
        __FMT_PUT('+');
      }
      for (c = 2, k = 10; 10 * k <= decpt; c++) k *= 10;
      for (;;) {
        i1 = decpt / k;
        __FMT_PUT(i1 + '0');
        if (--c <= 0) break;
        decpt -= i1 * k;
        decpt *= 10;
      }
      while (--width >= 0) {
        __FMT_PUT(' ');
      }
      break;

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
      if (pun.u[1] & 0x80000000) {
        sign = '-';
        pun.u[1] &= 0x7fffffff;
      }
      if (pun.d) {
        c = '1';
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
            ui = 1u << ((5 - prec) * 4 - 1);
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
            ui = 1u << (i1 - 1);
            if (pun.u[0] & ui && pun.u[0] & ((ui - 1) | (ui << 1))) {
              pun.u[0] += ui;
              if (!(pun.u[0] >> i1)) goto BumpIt;
            }
          }
        }
      } else {
        if ((ui = pun.u[0])) {
          ui = __builtin_ctz(ui);
          prec = 6 + ((32 - ROUNDDOWN(ui, 4)) >> 2) - 1;
        } else if ((ui = pun.u[1] & 0xfffff)) {
          ui = __builtin_ctz(ui);
          prec = (20 - ROUNDDOWN(ui, 4)) >> 2;
        } else {
          prec = 0;
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
      if (pun.u[1] & 0x80000000) {
        pun.u[1] &= 0x7fffffff;
        if (pun.d || sign) sign = '-';
      }
      if ((width -= bw + 5) > 0) {
        if (sign) --width;
        if (prec) --width;
      }
      if (pun.q && prec > 0) {
        width -= ROUNDUP(bsrl(pun.q) + 1, 4) >> 2;
      }
      if (width > 0 && !(flags & FLAGS_LEFT)) {
        if (flags & FLAGS_ZEROPAD) {
          if (sign) {
            __FMT_PUT(sign);
            sign = 0;
          }
          do
            __FMT_PUT('0');
          while (--width > 0);
        } else {
          do
            __FMT_PUT(' ');
          while (--width > 0);
        }
      }
      if (sign) __FMT_PUT(sign);
      __FMT_PUT('0');
      __FMT_PUT(alphabet[17]);
      __FMT_PUT(c);
      if (prec > 0) __FMT_PUT('.');
      while (prec-- > 0) {
        __FMT_PUT(alphabet[(pun.q >> 48) & 0xf]);
        pun.q <<= 4;
      }
      __FMT_PUT(alphabet[16]);
      if (bex < 0) {
        __FMT_PUT('-');
        bex = -bex;
      } else {
        __FMT_PUT('+');
      }
      for (c = 1; 10 * c <= bex;) c *= 10;
      for (;;) {
        i1 = bex / c;
        __FMT_PUT('0' + i1);
        if (!--bw) break;
        bex -= i1 * c;
        bex *= 10;
      }
      break;

    FormatString:
      if (__fmt_stoa(out, arg, p, flags, prec, width, signbit, qchar) == -1) {
        return -1;
      }
      break;
  }
  return 0;
}
