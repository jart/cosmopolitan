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
#include "libc/fmt/conv.h"
#include "libc/fmt/divmod10.internal.h"
#include "libc/fmt/fmt.internal.h"
#include "libc/fmt/internal.h"
#include "libc/limits.h"
#include "libc/mem/reverse.internal.h"

#define BUFFER_SIZE 144

uint128_t __udivmodti4(uint128_t, uint128_t, uint128_t *);

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

int __fmt_ntoa2(int out(const char *, void *, size_t), void *arg,
                uint128_t value, bool neg, unsigned log2base, unsigned prec,
                unsigned width, unsigned flags, const char *alphabet) {
  uint128_t remainder;
  unsigned len, count, digit;
  char buf[BUFFER_SIZE];
  len = 0;
  /* we check for log2base != 3 because otherwise we'll print nothing for a
   * value of 0 with precision 0 when # mandates that one be printed */
  if (!value && log2base != 3) flags &= ~FLAGS_HASH;
  if (value || !(flags & FLAGS_PRECISION)) {
    count = 0;
    do {
      if (!log2base) {
        if (value <= UINT64_MAX) {
          value = DivMod10(value, &digit);
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
    _npassert(count <= BUFFER_SIZE);
  }
  return __fmt_ntoa_format(out, arg, buf, len, neg, log2base, prec, width,
                           flags, alphabet);
}

int __fmt_ntoa(int out(const char *, void *, size_t), void *arg,
               uint128_t value, unsigned char signbit, unsigned long log2base,
               unsigned long prec, unsigned long width, unsigned char flags,
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
