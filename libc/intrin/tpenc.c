/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2023 Justine Alexandra Roberts Tunney                              │
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
#include "libc/intrin/bsr.h"
#include "libc/str/str.h"

static const uint16_t kTpEnc[32 - 7] = {
    1 | 0300 << 8, 1 | 0300 << 8, 1 | 0300 << 8, 1 | 0300 << 8, 2 | 0340 << 8,
    2 | 0340 << 8, 2 | 0340 << 8, 2 | 0340 << 8, 2 | 0340 << 8, 3 | 0360 << 8,
    3 | 0360 << 8, 3 | 0360 << 8, 3 | 0360 << 8, 3 | 0360 << 8, 4 | 0370 << 8,
    4 | 0370 << 8, 4 | 0370 << 8, 4 | 0370 << 8, 4 | 0370 << 8, 5 | 0374 << 8,
    5 | 0374 << 8, 5 | 0374 << 8, 5 | 0374 << 8, 5 | 0374 << 8, 5 | 0374 << 8,
};

/**
 * Encodes Thompson-Pike variable-length integer.
 */
uint64_t tpenc(uint32_t c) {
  int e, n;
  uint64_t w;
  if (0 <= c && c <= 127) return c;
  e = kTpEnc[_bsr(c) - 7];
  n = e & 0xff;
  w = 0;
  do {
    w |= 0200 | (c & 077);
    w <<= 8;
    c >>= 6;
  } while (--n);
  return c | w | e >> 8;
}
