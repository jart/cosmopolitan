/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright (C) 1997, 1999, 2001 Lucent Technologies                           │
│ All Rights Reserved                                                          │
│                                                                              │
│ Permission to use, copy, modify, and distribute this software and            │
│ its documentation for any purpose and without fee is hereby                  │
│ granted, provided that the above copyright notice appear in all              │
│ copies and that both that the copyright notice and this                      │
│ permission notice and warranty disclaimer appear in supporting               │
│ documentation, and that the name of Lucent or any of its entities            │
│ not be used in advertising or publicity pertaining to                        │
│ distribution of the software without specific, written prior                 │
│ permission.                                                                  │
│                                                                              │
│ LUCENT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,                │
│ INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.             │
│ IN NO EVENT SHALL LUCENT OR ANY OF ITS ENTITIES BE LIABLE FOR ANY            │
│ SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES                    │
│ WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER              │
│ IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,               │
│ ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF               │
│ THIS SOFTWARE.                                                               │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/assert.h"
#include "libc/fmt/fmt.h"
#include "libc/fmt/fmt.internal.h"
#include "libc/fmt/internal.h"
#include "libc/macros.internal.h"
#include "libc/intrin/bsr.h"
#include "libc/str/str.h"
#include "third_party/gdtoa/gdtoa.h"

/**
 * @fileoverview Floating-Point Formatting
 *
 * This implements most of ANSI C's printf floating-point directives.
 * omitting L, with %.0g and %.0G giving the shortest decimal string
 * that rounds to the number being converted, and with negative
 * precisions allowed for %f.
 */

struct FPBits {
  uint32_t bits[4];
  const FPI *fpi;
  int sign;
  int ex;  // exponent
  int kind;
};

union U {
  double d;
  uint64_t q;
  long double ld;
  unsigned int ui[4];
  unsigned short us[5];
};

static const FPI kFpiDbl = {
    .nbits = 53,
    .emin = 1 - 1023 - 53 + 1,
    .emax = 2046 - 1023 - 53 + 1,
    .rounding = FPI_Round_near,
    .sudden_underflow = 0,
};

static const FPI kFpiLdbl = {
    .nbits = 64,
    .emin = 1 - 16383 - 64 + 1,
    .emax = 32766 - 16383 - 64 + 1,
    .rounding = FPI_Round_near,
    .sudden_underflow = 0,
};

static const char kSpecialFloats[2][2][4] = {
    {"INF", "inf"},
    {"NAN", "nan"},
};

static void dfpbits(union U *u, struct FPBits *b) {
  int ex, i;
  uint32_t *bits;
  b->fpi = &kFpiDbl;
  b->sign = u->ui[1] & 0x80000000L;
  bits = b->bits;
  bits[1] = u->ui[1] & 0xfffff;
  bits[0] = u->ui[0];
  if ((ex = (u->ui[1] & 0x7ff00000L) >> 20) != 0) {
    if (ex == 0x7ff) {
      // Infinity or NaN
      i = bits[0] | bits[1] ? STRTOG_NaN : STRTOG_Infinite;
    } else {
      i = STRTOG_Normal;
      bits[1] |= 0x100000;
    }
  } else if (bits[0] | bits[1]) {
    i = STRTOG_Denormal;
    ex = 1;
  } else {
    i = STRTOG_Zero;
  }
  b->kind = i;
  b->ex = ex - (0x3ff + 52);
}

static void xfpbits(union U *u, struct FPBits *b) {
  uint32_t *bits;
  int ex, i;
  b->fpi = &kFpiLdbl;
  b->sign = u->us[4] & 0x8000;
  bits = b->bits;
  bits[1] = ((unsigned)u->us[3] << 16) | u->us[2];
  bits[0] = ((unsigned)u->us[1] << 16) | u->us[0];
  if ((ex = u->us[4] & 0x7fff) != 0) {
    i = STRTOG_Normal;
    if (ex == 0x7fff)  // Infinity or NaN
      i = bits[0] | bits[1] ? STRTOG_NaN : STRTOG_Infinite;
  } else if (bits[0] | bits[1]) {
    i = STRTOG_Denormal;
    ex = 1;
  } else {
    i = STRTOG_Zero;
  }
  b->kind = i;
  b->ex = ex - (0x3fff + 63);
}

// returns number of hex digits minus 1, or 0 for zero
static int fpiprec(struct FPBits *b) {
  FPI *fpi;
  int i, j, k, m;
  uint32_t *bits;
  if (b->kind == STRTOG_Zero) return (b->ex = 0);
  fpi = b->fpi;
  bits = b->bits;
  for (k = (fpi->nbits - 1) >> 2; k > 0; --k) {
    if ((bits[k >> 3] >> 4 * (k & 7)) & 0xf) {
      m = k >> 3;
      for (i = 0; i <= m; ++i)
        if (bits[i]) {
          if (i > 0) {
            k -= 8 * i;
            b->ex += 32 * i;
            for (j = i; j <= m; ++j) {
              bits[j - i] = bits[j];
            }
          }
          break;
        }
      for (i = 0; i < 28 && !((bits[0] >> i) & 0xf); i += 4) donothing;
      if (i) {
        b->ex += i;
        m = k >> 3;
        k -= (i >> 2);
        for (j = 0;; ++j) {
          bits[j] >>= i;
          if (j == m) break;
          bits[j] |= bits[j + 1] << (32 - i);
        }
      }
      break;
    }
  }
  return k;
}

// round to prec hex digits after the "."
// prec1 = incoming precision (after ".")
static int bround(struct FPBits *b, int prec, int prec1) {
  uint32_t *bits, t;
  int i, inc, j, k, m, n;
  m = prec1 - prec;
  bits = b->bits;
  inc = 0;
  k = m - 1;
  if ((t = bits[k >> 3] >> (j = (k & 7) * 4)) & 8) {
    if (t & 7) goto inc1;
    if (j && bits[k >> 3] << (32 - j)) goto inc1;
    while (k >= 8) {
      k -= 8;
      if (bits[k >> 3]) {
      inc1:
        inc = 1;
        goto haveinc;
      }
    }
  }
haveinc:
  b->ex += m * 4;
  i = m >> 3;
  k = prec1 >> 3;
  j = i;
  if ((n = 4 * (m & 7)))
    for (;; ++j) {
      bits[j - i] = bits[j] >> n;
      if (j == k) break;
      bits[j - i] |= bits[j + 1] << (32 - n);
    }
  else
    for (;; ++j) {
      bits[j - i] = bits[j];
      if (j == k) break;
    }
  k = prec >> 3;
  if (inc) {
    for (j = 0; !(++bits[j] & 0xffffffff); ++j) donothing;
    if (j > k) {
    onebit:
      bits[0] = 1;
      b->ex += 4 * prec;
      return 1;
    }
    if ((j = prec & 7) < 7 && bits[k] >> (j + 1) * 4) goto onebit;
  }
  for (i = 0; !(bits[i >> 3] & (0xf << 4 * (i & 7))); ++i) donothing;
  if (i) {
    b->ex += 4 * i;
    prec -= i;
    j = i >> 3;
    i &= 7;
    i *= 4;
    for (m = j;; ++m) {
      bits[m - j] = bits[m] >> i;
      if (m == k) break;
      bits[m - j] |= bits[m + 1] << (32 - i);
    }
  }
  return prec;
}

int __fmt_dtoa(int (*out)(const char *, void *, size_t), void *arg, int d,
               int flags, int prec, int sign, int width, bool longdouble,
               char qchar, unsigned char signbit, const char *alphabet,
               va_list va) {
  double x;
  union U u;
  struct FPBits fpb;
  char *s, *q, *se, *s0, special[8];
  int c, k, i1, ui, bw, bex, sgn, prec1, decpt;
  x = 0;
  switch (d) {
    case 'F':
    case 'f':
      if (!(flags & FLAGS_PRECISION)) prec = 6;
      if (!longdouble) {
        x = va_arg(va, double);
        s = s0 = dtoa(x, 3, prec, &decpt, &fpb.sign, &se);
        if (decpt == 9999) {
          if (s && s[0] == 'N') {
            fpb.kind = STRTOG_NaN;
          } else {
            fpb.kind = STRTOG_Infinite;
          }
        }
      } else {
        u.ld = va_arg(va, long double);
        xfpbits(&u, &fpb);
        s = s0 =
            gdtoa(fpb.fpi, fpb.ex, fpb.bits, &fpb.kind, 3, prec, &decpt, &se);
      }
      if (decpt == 9999) {
      Format9999:
        if (s0) freedtoa(s0);
        bzero(special, sizeof(special));
        s = q = special;
        if (fpb.sign) {
          *q++ = '-';
        } else if (flags & FLAGS_PLUS) {
          *q++ = '+';
        } else if (flags & FLAGS_SPACE) {
          *q++ = ' ';
        }
        memcpy(q, kSpecialFloats[fpb.kind == STRTOG_NaN][d >= 'a'], 4);
        flags &= ~(FLAGS_PRECISION | FLAGS_PLUS | FLAGS_HASH | FLAGS_SPACE);
        prec = 0;
        return __fmt_stoa(out, arg, s, flags, prec, width, signbit, qchar);
      }
    FormatReal:
      if (fpb.sign /* && (x || sign) */) sign = '-';
      if (prec > 0) width -= prec;
      if (width > 0) {
        if (sign) --width;
        if (decpt <= 0) {
          --width;
          if (prec > 0) --width;
        } else {
          if (s == se) decpt = 1;
          width -= decpt;
          if (prec > 0 || (flags & FLAGS_HASH)) --width;
        }
      }
      if (width > 0 && !(flags & FLAGS_LEFT)) {
        if ((flags & FLAGS_ZEROPAD)) {
          if (sign) __FMT_PUT(sign);
          sign = 0;
          do __FMT_PUT('0');
          while (--width > 0);
        } else
          do __FMT_PUT(' ');
          while (--width > 0);
      }
      if (sign) __FMT_PUT(sign);
      if (decpt <= 0) {
        __FMT_PUT('0');
        if (prec > 0 || (flags & FLAGS_HASH)) __FMT_PUT('.');
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
        if (prec > 0 || (flags & FLAGS_HASH)) __FMT_PUT('.');
      }
      while (--prec >= 0) {
        if ((c = *s)) {
          s++;
        } else {
          c = '0';
        }
        __FMT_PUT(c);
      }
      while (--width >= 0) __FMT_PUT(' ');
      if (s0) freedtoa(s0);
      break;

    case 'G':
    case 'g':
      if (!(flags & FLAGS_PRECISION)) prec = 6;
      if (prec < 1) prec = 1;
      if (!longdouble) {
        x = va_arg(va, double);
        s = s0 = dtoa(x, 2, prec, &decpt, &fpb.sign, &se);
        if (decpt == 9999) {
          if (s && s[0] == 'N') {
            fpb.kind = STRTOG_NaN;
          } else {
            fpb.kind = STRTOG_Infinite;
          }
        }
      } else {
        u.ld = va_arg(va, long double);
        xfpbits(&u, &fpb);
        s = s0 = gdtoa(fpb.fpi, fpb.ex, fpb.bits, &fpb.kind, prec ? 2 : 0, prec,
                       &decpt, &se);
      }
      if (decpt == 9999) goto Format9999;
      c = se - s;
      prec1 = prec;
      if (!prec) {
        prec = c;
        prec1 = c + (s[1] || (flags & FLAGS_HASH) ? 5 : 4);
        // %.0g gives 10 rather than 1e1
      }
      if (decpt > -4 && decpt <= prec1) {
        if ((flags & FLAGS_HASH))
          prec -= decpt;
        else
          prec = c - decpt;
        if (prec < 0) prec = 0;
        goto FormatReal;
      }
      d -= 2;
      if (!(flags & FLAGS_HASH) && prec > c) prec = c;
      --prec;
      goto FormatExpo;

    case 'e':
    case 'E':
      if (!(flags & FLAGS_PRECISION)) prec = 6;
      if (prec < 0) prec = 0;
      if (!longdouble) {
        x = va_arg(va, double);
        s = s0 = dtoa(x, 2, prec + 1, &decpt, &fpb.sign, &se);
        if (decpt == 9999) {
          if (s && s[0] == 'N') {
            fpb.kind = STRTOG_NaN;
          } else {
            fpb.kind = STRTOG_Infinite;
          }
        }
      } else {
        u.ld = va_arg(va, long double);
        xfpbits(&u, &fpb);
        s = s0 = gdtoa(fpb.fpi, fpb.ex, fpb.bits, &fpb.kind, prec ? 2 : 0, prec,
                       &decpt, &se);
      }
      if (decpt == 9999) goto Format9999;
    FormatExpo:
      if (fpb.sign /* && (x || sign) */) sign = '-';
      if ((width -= prec + 5) > 0) {
        if (sign) --width;
        if (prec || (flags & FLAGS_HASH)) --width;
      }
      if ((c = --decpt) < 0) c = -c;
      while (c >= 100) {
        --width;
        c /= 10;
      }
      if (width > 0 && !(flags & FLAGS_LEFT)) {
        if ((flags & FLAGS_ZEROPAD)) {
          if (sign) __FMT_PUT(sign);
          sign = 0;
          do __FMT_PUT('0');
          while (--width > 0);
        } else
          do __FMT_PUT(' ');
          while (--width > 0);
      }
      if (sign) __FMT_PUT(sign);
      __FMT_PUT(*s++);
      if (prec || (flags & FLAGS_HASH)) __FMT_PUT('.');
      while (--prec >= 0) {
        if ((c = *s))
          s++;
        else
          c = '0';
        __FMT_PUT(c);
      }
      __FMT_PUT(d);
      if (decpt < 0) {
        __FMT_PUT('-');
        decpt = -decpt;
      } else
        __FMT_PUT('+');
      for (c = 2, k = 10; 10 * k <= decpt; c++, k *= 10) donothing;
      for (;;) {
        i1 = decpt / k;
        __FMT_PUT(i1 + '0');
        if (--c <= 0) break;
        decpt -= i1 * k;
        decpt *= 10;
      }
      while (--width >= 0) __FMT_PUT(' ');
      freedtoa(s0);
      break;

    case 'A':
      alphabet = "0123456789ABCDEFPX";
      goto FormatBinary;
    case 'a':
      alphabet = "0123456789abcdefpx";
    FormatBinary:
      if (longdouble) {
        u.ld = va_arg(va, long double);
        xfpbits(&u, &fpb);
      } else {
        u.d = va_arg(va, double);
        dfpbits(&u, &fpb);
      }
      if (fpb.kind == STRTOG_Infinite || fpb.kind == STRTOG_NaN) {
        s0 = 0;
        goto Format9999;
      }
      prec1 = fpiprec(&fpb);
      if ((flags & FLAGS_PRECISION) && prec < prec1) {
        prec1 = bround(&fpb, prec, prec1);
      }
      bw = 1;
      bex = fpb.ex + 4 * prec1;
      if (bex) {
        if ((i1 = bex) < 0) i1 = -i1;
        while (i1 >= 10) {
          ++bw;
          i1 /= 10;
        }
      }
      if (fpb.sign /* && (sign || fpb.kind != STRTOG_Zero) */) {
        sign = '-';
      }
      if ((width -= bw + 5) > 0) {
        if (sign) --width;
        if (prec1 || (flags & FLAGS_HASH)) --width;
      }
      if ((width -= prec1) > 0 && !(flags & FLAGS_LEFT) &&
          !(flags & FLAGS_ZEROPAD)) {
        do __FMT_PUT(' ');
        while (--width > 0);
      }
      if (sign) __FMT_PUT(sign);
      __FMT_PUT('0');
      __FMT_PUT(alphabet[17]);
      if ((flags & FLAGS_ZEROPAD) && width > 0 && !(flags & FLAGS_LEFT)) {
        do __FMT_PUT('0');
        while (--width > 0);
      }
      i1 = prec1 & 7;
      k = prec1 >> 3;
      __FMT_PUT(alphabet[(fpb.bits[k] >> 4 * i1) & 0xf]);
      if (prec1 > 0 || (flags & FLAGS_HASH)) __FMT_PUT('.');
      if (prec1 > 0) {
        prec -= prec1;
        while (prec1 > 0) {
          if (--i1 < 0) {
            if (--k < 0) break;
            i1 = 7;
          }
          __FMT_PUT(alphabet[(fpb.bits[k] >> 4 * i1) & 0xf]);
          --prec1;
        }
        if ((flags & FLAGS_HASH) && prec > 0) {
          do __FMT_PUT(0);
          while (--prec > 0);
        }
      }
      __FMT_PUT(alphabet[16]);
      if (bex < 0) {
        __FMT_PUT('-');
        bex = -bex;
      } else
        __FMT_PUT('+');
      for (c = 1; 10 * c <= bex; c *= 10) donothing;
      for (;;) {
        i1 = bex / c;
        __FMT_PUT('0' + i1);
        if (!--bw) break;
        bex -= i1 * c;
        bex *= 10;
      }
      while (--width >= 0) __FMT_PUT(' ');
      break;
    default:
      unreachable;
  }
  return 0;
}
