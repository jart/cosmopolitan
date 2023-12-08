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
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/fmt/itoa.h"
#include "libc/intrin/bsr.h"

static inline int PickGoodWidth(unsigned x) {
  if (x < 16) {
    if (x < 2) return 0;
    if (x < 8) return 7;
    return 15;
  } else {
    if (x < 32) return 31;
    return 63;
  }
}

/**
 * Converts unsigned 64-bit integer to binary string.
 *
 * @param p needs at least 67 bytes
 * @param z is 0 for DIGITS, 1 for 0bDIGITS, 2 for 0bDIGITS if ≠0
 * @return pointer to nul byte
 */
char *FormatBinary64(char p[hasatleast 67], uint64_t x, char z) {
  int i;
  uint64_t b;
  if (x) {
    if (z) {
      *p++ = '0';
      *p++ = 'b';
    }
    i = PickGoodWidth(_bsrl(x));
    do {
      b = 1;
      b <<= i;
      *p++ = '0' + !!(x & b);
    } while (i--);
  } else {
    if (z == 1) {
      *p++ = '0';
      *p++ = 'b';
    }
    *p++ = '0';
  }
  *p = 0;
  return p;
}
