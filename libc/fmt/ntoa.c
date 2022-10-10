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
                             unsigned char flags) {
  unsigned i;
  /* pad leading zeros */
  if (!(flags & FLAGS_LEFT)) {
    if (width && (flags & FLAGS_ZEROPAD) &&
        (negative || (flags & (FLAGS_PLUS | FLAGS_SPACE)))) {
      width--;
    }
    while ((len < prec) && (len < BUFFER_SIZE)) {
      buf[len++] = '0';
    }
    while ((flags & FLAGS_ZEROPAD) && (len < width) && (len < BUFFER_SIZE)) {
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
    if (log2base == 4 && len < BUFFER_SIZE) {
      buf[len++] = 'x';
    } else if (log2base == 1 && len < BUFFER_SIZE) {
      buf[len++] = 'b';
    }
    if (len < BUFFER_SIZE) {
      buf[len++] = '0';
    }
  }
  if (len < BUFFER_SIZE) {
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
      if (__fmt_pad(out, arg, width - len) == -1) return -1;
    }
  }
  reverse(buf, len);
  if (out(buf, arg, len) == -1) return -1;
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
  if (!value) flags &= ~FLAGS_HASH;
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
                           flags);
}

int __fmt_ntoa(int out(const char *, void *, size_t), void *arg, va_list va,
               unsigned char signbit, unsigned long log2base,
               unsigned long prec, unsigned long width, unsigned char flags,
               const char *lang) {
  bool neg;
  uint128_t value, sign;

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

  return __fmt_ntoa2(out, arg, value, neg, log2base, prec, width, flags, lang);
}
