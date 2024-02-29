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
│                                                                              │
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
#include "libc/errno.h"
#include "libc/fmt/conv.h"
#include "libc/fmt/divmod10.internal.h"
#include "libc/fmt/itoa.h"
#include "libc/intrin/bsr.h"
#include "libc/intrin/nomultics.internal.h"
#include "libc/intrin/safemacros.internal.h"
#include "libc/limits.h"
#include "libc/macros.internal.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/mem/reverse.internal.h"
#include "libc/runtime/internal.h"
#include "libc/serialize.h"
#include "libc/str/str.h"
#include "libc/str/strwidth.h"
#include "libc/str/tab.internal.h"
#include "libc/str/thompike.h"
#include "libc/str/unicode.h"
#include "libc/str/utf16.h"
#include "libc/sysv/errfuns.h"
#include "third_party/gdtoa/gdtoa.h"

#define BUFFER_SIZE 144

#define PRINTF_NTOA_BUFFER_SIZE 144

#define FLAGS_ZEROPAD   0x01
#define FLAGS_LEFT      0x02
#define FLAGS_PLUS      0x04
#define FLAGS_SPACE     0x08
#define FLAGS_HASH      0x10
#define FLAGS_PRECISION 0x20
#define FLAGS_ISSIGNED  0x40
#define FLAGS_NOQUOTE   0x80
#define FLAGS_QUOTE     FLAGS_SPACE
#define FLAGS_GROUPING  FLAGS_NOQUOTE
#define FLAGS_REPR      FLAGS_PLUS

#define __FMT_PUT(C)              \
  do {                            \
    char Buf[1] = {C};            \
    if (out(Buf, arg, 1) == -1) { \
      return -1;                  \
    }                             \
  } while (0)

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
  uint32_t ui[4];
  uint16_t us[5];
};

static const FPI kFpiDbl = {
    .nbits = DBL_MANT_DIG,
    .emin = 3 - DBL_MAX_EXP - DBL_MANT_DIG,
    .emax = DBL_MAX_EXP - DBL_MANT_DIG,
    .rounding = FPI_Round_near,
    .sudden_underflow = 0,
};

static const FPI kFpiLdbl = {
    .nbits = LDBL_MANT_DIG,
    .emin = 3 - LDBL_MAX_EXP - LDBL_MANT_DIG,
    .emax = LDBL_MAX_EXP - LDBL_MANT_DIG,
    .rounding = FPI_Round_near,
    .sudden_underflow = 0,
};

static const char kSpecialFloats[2][2][4] = {
    {"INF", "inf"},
    {"NAN", "nan"},
};

typedef int (*out_f)(const char *, void *, size_t);
typedef int (*emit_f)(out_f, void *, uint64_t);

uint128_t __udivmodti4(uint128_t, uint128_t, uint128_t *);

static int __fmt_atoi(const char **str) {
  int i;
  for (i = 0; '0' <= **str && **str <= '9'; ++*str) {
    i *= 10;
    i += **str - '0';
  }
  return i;
}

static int __fmt_pad(int out(const char *, void *, size_t), void *arg,
                     unsigned long n) {
  int i, rc;
  for (rc = i = 0; i < n; ++i) rc |= out(" ", arg, 1);
  return rc;
}

static int __fmt_ntoa_format(int out(const char *, void *, size_t), void *arg,
                             char *buf, unsigned len, bool negative,
                             unsigned log2base, unsigned prec, unsigned width,
                             unsigned char flags, const char *alphabet) {
  unsigned i, prec_width_zeros;
  char alternate_form_middle_char, sign_character;
  unsigned actual_buf_len;
  actual_buf_len = len;
  prec_width_zeros = 0;
  /* pad leading zeros */
  if (width && (flags & FLAGS_ZEROPAD) &&
      (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
    width--;
  }
  if (len < prec) {
    prec_width_zeros += (prec - len);
    len = prec;
  }
  if ((flags & FLAGS_ZEROPAD) && (len < width)) {
    prec_width_zeros += (width - len);
    len = width;
  }
  /* handle hash */
  if (flags & FLAGS_HASH) {
    if ((!(flags & FLAGS_PRECISION) || log2base == 3) && len &&
        ((len >= prec) || (len >= width)) &&
        (prec_width_zeros || buf[len - 1] == '0')) {
      if (prec_width_zeros) {
        --prec_width_zeros;
      }
      --len;
      if (len < actual_buf_len) {
        actual_buf_len = len;
      }
      if (len && (log2base == 4 || log2base == 1) &&
          (prec_width_zeros || buf[len - 1] == '0')) {
        if (prec_width_zeros) {
          --prec_width_zeros;
        }
        --len;
        if (len < actual_buf_len) {
          actual_buf_len = len;
        }
      }
    }
    alternate_form_middle_char = '\0';
    if ((log2base == 4 || log2base == 1)) {
      ++len;
      alternate_form_middle_char =
          alphabet[17];  // x, X or b (for the corresponding conversion
                         // specifiers)
    }
    ++len;
  }
  sign_character = '\0';
  if (negative) {
    ++len;
    sign_character = '-';
  } else if (flags & FLAGS_PLUS) {
    ++len;
    sign_character = '+'; /* ignore the space if the '+' exists */
  } else if (flags & FLAGS_SPACE) {
    ++len;
    sign_character = ' ';
  }
  /* pad spaces up to given width */
  if (!(flags & FLAGS_LEFT) && !(flags & FLAGS_ZEROPAD)) {
    if (len < width) {
      if (__fmt_pad(out, arg, width - len) == -1) return -1;
    }
  }
  if (sign_character != '\0' && out(&sign_character, arg, 1) == -1) return -1;
  if (flags & FLAGS_HASH) {
    if (out("0", arg, 1) == -1) return -1;
    if (alternate_form_middle_char != '\0' &&
        out(&alternate_form_middle_char, arg, 1) == -1)
      return -1;
  }
  for (i = 0; i < prec_width_zeros; ++i)
    if (out("0", arg, 1) == -1) return -1;
  reverse(buf, actual_buf_len);
  if (out(buf, arg, actual_buf_len) == -1) return -1;
  /* append pad spaces up to given width */
  if (flags & FLAGS_LEFT) {
    if (len < width) {
      if (__fmt_pad(out, arg, width - len) == -1) return -1;
    }
  }
  return 0;
}

static int __fmt_ntoa2(int out(const char *, void *, size_t), void *arg,
                       uint128_t value, bool neg, unsigned log2base,
                       unsigned prec, unsigned width, unsigned flags,
                       const char *alphabet) {
  uint128_t remainder;
  unsigned len, count, digit;
  char buf[BUFFER_SIZE];
  len = 0;
  // we check for log2base!=3, since otherwise we'll print nothing for
  // a value of 0 with precision 0 when # mandates that one be printed
  if (!value && log2base != 3) flags &= ~FLAGS_HASH;
  if (value || !(flags & FLAGS_PRECISION)) {
    count = 0;
    do {
      if (!log2base) {
        if (value <= UINT64_MAX) {
          value = __divmod10(value, &digit);
        } else {
          value = __udivmodti4(value, 10, &remainder);
          digit = remainder;
        }
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
    npassert(count <= BUFFER_SIZE);
  }
  return __fmt_ntoa_format(out, arg, buf, len, neg, log2base, prec, width,
                           flags, alphabet);
}

static int __fmt_ntoa(int out(const char *, void *, size_t), void *arg,
                      uint128_t value, unsigned char signbit,
                      unsigned long log2base, unsigned long prec,
                      unsigned long width, unsigned char flags,
                      const char *lang) {
  bool neg;
  uint128_t sign;

  /* ignore '0' flag when prec or minus flag is given */
  if (flags & (FLAGS_PRECISION | FLAGS_LEFT)) {
    flags &= ~FLAGS_ZEROPAD;
  }

  /* no plus / space flag for u, x, X, o, b */
  if (!(flags & FLAGS_ISSIGNED)) {
    flags &= ~(FLAGS_PLUS | FLAGS_SPACE);
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

  return __fmt_ntoa2(out, arg, value, neg, log2base, prec, width, flags, lang);
}

/**
 * Converts byte to word-encoded C string literal representation.
 */
static int __fmt_cescapec(int c) {
  switch ((c &= 255)) {
    case '\a':
      return '\\' | 'a' << 8;
    case '\b':
      return '\\' | 'b' << 8;
    case '\t':
      return '\\' | 't' << 8;
    case '\n':
      return '\\' | 'n' << 8;
    case '\v':
      return '\\' | 'v' << 8;
    case '\f':
      return '\\' | 'f' << 8;
    case '\r':
      return '\\' | 'r' << 8;
    case '"':
      return '\\' | '"' << 8;
    case '\'':
      return '\\' | '\'' << 8;
    case '\\':
      return '\\' | '\\' << 8;
    default:
      if (' ' <= c && c <= '~') {
        return c;
      } else {
        return '\\' |                          //
               ('0' + (c >> 6)) << 8 |         //
               ('0' + ((c >> 3) & 7)) << 16 |  //
               ('0' + (c & 7)) << 24;
      }
  }
}

static int __fmt_stoa_byte(out_f out, void *a, uint64_t c) {
  char buf[1] = {c};
  return out(buf, a, 1);
}

static int __fmt_stoa_wide(out_f out, void *a, uint64_t w) {
  char buf[8];
  if (!isascii(w)) w = tpenc(w);
  WRITE64LE(buf, w);
  return out(buf, a, w ? (_bsr(w) >> 3) + 1 : 1);
}

static int __fmt_stoa_bing(out_f out, void *a, uint64_t w) {
  char buf[8];
  w = tpenc(kCp437[w & 0xFF]);
  WRITE64LE(buf, w);
  return out(buf, a, w ? (_bsr(w) >> 3) + 1 : 1);
}

static int __fmt_stoa_quoted(out_f out, void *a, uint64_t w) {
  char buf[8];
  if (isascii(w)) {
    w = __fmt_cescapec(w);
  } else {
    w = tpenc(w);
  }
  WRITE64LE(buf, w);
  return out(buf, a, w ? (_bsr(w) >> 3) + 1 : 1);
}

/**
 * Converts string to array.
 *
 * This is used by __fmt() to implement the %s and %c directives. The
 * content outputted to the array is always UTF-8, but the input may be
 * UTF-16 or UTF-32.
 *
 * @see __fmt()
 */
static int __fmt_stoa(int out(const char *, void *, size_t), void *arg,
                      void *data, unsigned long flags, unsigned long precision,
                      unsigned long width, unsigned char signbit,
                      unsigned char qchar) {
  wint_t wc;
  unsigned n;
  emit_f emit;
  char *p, buf[1];
  unsigned w, pad;
  bool justdobytes, ignorenul;

  p = data;
  if (!p) {
    p = ((flags & FLAGS_REPR) ? "NULL" : "(null)");
    signbit = 0;
    flags |= FLAGS_NOQUOTE;
    if (flags & FLAGS_PRECISION) {
      precision = min(strlen(p), precision);
    }
  }

  ignorenul = false;
  justdobytes = false;
  if (signbit == 15 || signbit == 63) {
    if (flags & FLAGS_QUOTE) {
      emit = __fmt_stoa_quoted;
      ignorenul = flags & FLAGS_PRECISION;
    } else {
      emit = __fmt_stoa_wide;
    }
  } else if (flags & FLAGS_HASH) {
    justdobytes = true;
    emit = __fmt_stoa_bing;
    ignorenul = flags & FLAGS_PRECISION;
  } else if (flags & FLAGS_QUOTE) {
    emit = __fmt_stoa_quoted;
    ignorenul = flags & FLAGS_PRECISION;
  } else {
    justdobytes = true;
    emit = __fmt_stoa_byte;
  }

  if (!(flags & FLAGS_PRECISION)) precision = -1;
  if (!(flags & FLAGS_PRECISION) || !ignorenul) {
    if (signbit == 63) {
      precision = wcsnlen((const wchar_t *)p, precision);
    } else if (signbit == 15) {
      precision = strnlen16((const char16_t *)p, precision);
    } else {
      precision = strnlen(p, precision);
    }
  }

  pad = 0;
  if (width) {
    w = precision;
    if (signbit == 63) {
      w = wcsnwidth((const wchar_t *)p, precision, 0);
    } else if (signbit == 15) {
      w = strnwidth16((const char16_t *)p, precision, 0);
    } else {
      w = strnwidth(p, precision, 0);
    }
    if (!(flags & FLAGS_NOQUOTE) && (flags & FLAGS_REPR)) {
      w += 2 + (signbit == 63) + (signbit == 15);
    }
    if (w < width) {
      pad = width - w;
    }
  }

  if (pad && !(flags & FLAGS_LEFT)) {
    if (__fmt_pad(out, arg, pad) == -1) return -1;
  }

  if (!(flags & FLAGS_NOQUOTE) && (flags & FLAGS_REPR)) {
    if (signbit == 63) {
      if (out("L", arg, 1) == -1) return -1;
    } else if (signbit == 15) {
      if (out("u", arg, 1) == -1) return -1;
    }
    buf[0] = qchar;
    if (out(buf, arg, 1) == -1) return -1;
  }

  if (justdobytes) {
    while (precision--) {
      wc = *p++ & 0xff;
      if (!wc && !ignorenul) break;
      if (emit(out, arg, wc) == -1) return -1;
    }
  } else {
    while (precision--) {
      if (signbit == 15) {
        wc = *(const char16_t *)p;
        if (!wc && !ignorenul) break;
        if (IsUcs2(wc)) {
          p += sizeof(char16_t);
        } else if (IsUtf16Cont(wc)) {
          p += sizeof(char16_t);
          continue;
        } else if (!precision) {
          break;
        } else {
          --precision;
          wc = MergeUtf16(wc, *(const char16_t *)p);
        }
      } else if (signbit == 63) {
        wc = *(const wint_t *)p;
        if (!wc && !ignorenul) break;
        p += sizeof(wint_t);
        if (!wc) break;
      } else {
        wc = *p++ & 0xff;
        if (!wc && !ignorenul) break;
        if (!isascii(wc)) {
          if (ThomPikeCont(wc)) continue;
          n = ThomPikeLen(wc) - 1;
          wc = ThomPikeByte(wc);
          if (n > precision) break;
          precision -= n;
          while (n--) {
            wc = ThomPikeMerge(wc, *p++);
          }
        }
      }
      if (emit(out, arg, wc) == -1) return -1;
    }
  }

  if (!(flags & FLAGS_NOQUOTE) && (flags & FLAGS_REPR)) {
    buf[0] = qchar;
    if (out(buf, arg, 1) == -1) return -1;
  }

  if (pad && (flags & FLAGS_LEFT)) {
    if (__fmt_pad(out, arg, pad) == -1) return -1;
  }

  return 0;
}

static void __fmt_dfpbits(union U *u, struct FPBits *b) {
  int ex, i;
  b->fpi = &kFpiDbl;
  b->sign = u->ui[1] & 0x80000000L;
  b->bits[1] = u->ui[1] & 0xfffff;
  b->bits[0] = u->ui[0];
  if ((ex = (u->ui[1] & 0x7ff00000L) >> 20) != 0) {
    if (ex != 0x7ff) {
      i = STRTOG_Normal;
      b->bits[1] |= 1 << (52 - 32);  // set lowest exponent bit
    } else if (b->bits[0] | b->bits[1]) {
      i = STRTOG_NaN;
    } else {
      i = STRTOG_Infinite;
    }
  } else if (b->bits[0] | b->bits[1]) {
    i = STRTOG_Denormal;
    ex = 1;
  } else {
    i = STRTOG_Zero;
  }
  b->kind = i;
  b->ex = ex - (0x3ff + 52);
}

static void __fmt_ldfpbits(union U *u, struct FPBits *b) {
#if LDBL_MANT_DIG == 53
  __fmt_dfpbits(u, b);
#else
  int ex, i;
  uint16_t sex;
#if LDBL_MANT_DIG == 64
  b->bits[3] = 0;
  b->bits[2] = 0;
  b->bits[1] = ((unsigned)u->us[3] << 16) | u->us[2];
  b->bits[0] = ((unsigned)u->us[1] << 16) | u->us[0];
  sex = u->us[4];
#elif LDBL_MANT_DIG == 113
  b->bits[3] = u->ui[3] & 0xffff;
  b->bits[2] = u->ui[2];
  b->bits[1] = u->ui[1];
  b->bits[0] = u->ui[0];
  sex = u->ui[3] >> 16;
#else
#error "unsupported architecture"
#endif
  b->fpi = &kFpiLdbl;
  b->sign = sex & 0x8000;
  if ((ex = sex & 0x7fff) != 0) {
    if (ex != 0x7fff) {
      i = STRTOG_Normal;
#if LDBL_MANT_DIG == 113
      b->bits[3] |= 1 << (112 - 32 * 3);  // set lowest exponent bit
#endif
    } else if (b->bits[0] | b->bits[1] | b->bits[2] | b->bits[3]) {
      i = STRTOG_NaN;
    } else {
      i = STRTOG_Infinite;
    }
  } else if (b->bits[0] | b->bits[1] | b->bits[2] | b->bits[3]) {
    i = STRTOG_Denormal;
    ex = 1;
  } else {
    i = STRTOG_Zero;
  }
  b->kind = i;
  b->ex = ex - (0x3fff + (LDBL_MANT_DIG - 1));
#endif
}

// returns number of hex digits minus 1, or 0 for zero
static int __fmt_fpiprec(struct FPBits *b) {
  const FPI *fpi;
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
static int __fmt_bround(struct FPBits *b, int prec, int prec1) {
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

static int __fmt_noop(const char *, void *, size_t) {
  return 0;
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
 *   - `%Lg`  long double
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
 * This implements most of ANSI C's printf floating-point directives,
 * omitting L, with %.0g and %.0G giving the shortest decimal string
 * that rounds to the number being converted, and with negative
 * precisions allowed for %f.
 *
 * @note implementation detail of printf(), snprintf(), etc.
 * @see printf() for wordier documentation
 * @note netlib.org is so helpful
 * @asyncsignalsafe if floating point isn't used
 * @vforksafe if floating point isn't used
 */
int __fmt(void *fn, void *arg, const char *format, va_list va) {
  long ld;
  void *p;
  double x;
  unsigned u;
  union U un;
  char ibuf[21];
  bool longdouble;
  unsigned long lu;
  struct FPBits fpb;
  wchar_t charbuf[1];
  const char *alphabet;
  unsigned char signbit, log2base;
  int c, k, i1, bw, rc, bex, prec1, decpt;
  int (*out)(const char *, void *, size_t);
  char *se, *s0, *s, *q, qchar, special[8];
  int d, w, n, sign, prec, flags, width, lasterr;

  x = 0;
  lasterr = errno;
  out = fn ? fn : (void *)__fmt_noop;

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
        const char *s = va_arg(va, const char *);
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
      flags &= ~FLAGS_PRECISION;
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
#if !(LDBL_MANT_DIG == 53 && LDBL_MAX_EXP == 1024)
        longdouble = true;
#endif
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
        alphabet = "0123456789abcdefpb";
        goto FormatNumber;
      case 'o':
        log2base = 3;
        goto FormatNumber;
      case 'd':
      case 'i':
        flags |= FLAGS_ISSIGNED;
        // fallthrough
      case 'u': {
        uint128_t value;
        flags &= ~FLAGS_HASH;  // no hash for dec format
      FormatNumber:
        if (signbit > 63) {
          value = va_arg(va, uint128_t);
        } else {
          value = va_arg(va, uint64_t);
        }
        if (__fmt_ntoa(out, arg, value, signbit, log2base, prec, width, flags,
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
        } else if (flags & (FLAGS_QUOTE | FLAGS_REPR)) {
          p = "'\\0'";
          flags &= ~(FLAGS_QUOTE | FLAGS_REPR | FLAGS_HASH);
          goto FormatString;
        } else if (flags & FLAGS_HASH) {
          flags &= ~FLAGS_HASH;
          p = " ";
          goto FormatString;
        } else {
          __FMT_PUT('\0');
          break;
        }
      case 'm':
        p = strerror(lasterr);
        signbit = 0;
        goto FormatString;
      case 'r':
        // undocumented %r specifier
        // used for good carriage return
        // helps integrate loggers with repls
        if (!__ttyconf.replstderr) {
          break;
        } else {
          p = "\r\e[K";
          goto FormatString;
        }
      case 'S':
        // Specified by POSIX to be equivalent to %ls
        signbit = 63;
        goto FormatStringPNotFetchedYet;
      case 'q':
        flags |= FLAGS_QUOTE;
        // fallthrough
      case 's':
      FormatStringPNotFetchedYet:
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
          un.ld = va_arg(va, long double);
          __fmt_ldfpbits(&un, &fpb);
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
          rc = __fmt_stoa(out, arg, s, flags, prec, width, signbit, qchar);
          if (rc == -1) return -1;
          break;
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
          un.ld = va_arg(va, long double);
          __fmt_ldfpbits(&un, &fpb);
          s = s0 = gdtoa(fpb.fpi, fpb.ex, fpb.bits, &fpb.kind, prec ? 2 : 0,
                         prec, &decpt, &se);
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
          un.ld = va_arg(va, long double);
          __fmt_ldfpbits(&un, &fpb);
          s = s0 = gdtoa(fpb.fpi, fpb.ex, fpb.bits, &fpb.kind, prec ? 2 : 0,
                         prec, &decpt, &se);
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
          if ((c = *s)) {
            s++;
          } else {
            c = '0';
          }
          __FMT_PUT(c);
        }
        __FMT_PUT(d);
        if (decpt < 0) {
          __FMT_PUT('-');
          decpt = -decpt;
        } else {
          __FMT_PUT('+');
        }
        for (c = 2, k = 10; 10 * k <= decpt; c++, k *= 10) {
          donothing;
        }
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
        freedtoa(s0);
        break;

      case 'A':
        alphabet = "0123456789ABCDEFPX";
        goto FormatBinary;
      case 'a':
        alphabet = "0123456789abcdefpx";
      FormatBinary:
        if (longdouble) {
          un.ld = va_arg(va, long double);
          __fmt_ldfpbits(&un, &fpb);
        } else {
          un.d = va_arg(va, double);
          __fmt_dfpbits(&un, &fpb);
        }
        if (fpb.kind == STRTOG_Infinite || fpb.kind == STRTOG_NaN) {
          s0 = 0;
          goto Format9999;
        }
        prec1 = __fmt_fpiprec(&fpb);
        if ((flags & FLAGS_PRECISION) && prec < prec1) {
          prec1 = __fmt_bround(&fpb, prec, prec1);
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
        if ((width -= MAX(prec, prec1)) > 0 && !(flags & FLAGS_LEFT) &&
            !(flags & FLAGS_ZEROPAD)) {
          do __FMT_PUT(' ');
          while (--width > 0);
        }
        if (sign) __FMT_PUT(sign);
        __FMT_PUT('0');
        __FMT_PUT(alphabet[17]);  // x or X
        if ((flags & FLAGS_ZEROPAD) && width > 0 && !(flags & FLAGS_LEFT)) {
          do __FMT_PUT('0');
          while (--width > 0);
        }
        i1 = prec1 & 7;
        k = prec1 >> 3;
        __FMT_PUT(alphabet[(fpb.bits[k] >> 4 * i1) & 0xf]);
        if (prec1 > 0 || prec > 0) {
          __FMT_PUT('.');
        }
        while (prec1 > 0) {
          if (--i1 < 0) {
            if (--k < 0) break;
            i1 = 7;
          }
          __FMT_PUT(alphabet[(fpb.bits[k] >> 4 * i1) & 0xf]);
          --prec1;
          --prec;
        }
        while (prec-- > 0) {
          __FMT_PUT('0');
        }
        __FMT_PUT(alphabet[16]);  // p or P
        if (bex < 0) {
          __FMT_PUT('-');
          bex = -bex;
        } else {
          __FMT_PUT('+');
        }
        for (c = 1; 10 * c <= bex; c *= 10) {
          donothing;
        }
        for (;;) {
          i1 = bex / c;
          __FMT_PUT('0' + i1);
          if (!--bw) break;
          bex -= i1 * c;
          bex *= 10;
        }
        while (--width >= 0) {
          __FMT_PUT(' ');
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
