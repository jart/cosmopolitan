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
#include "libc/nexgen32e/nexgen32e.h"

/**
 * Returns `len(str(x))` where x is an unsigned 64-bit integer.
 */
unsigned LengthUint64(uint64_t x) {
  unsigned w;
  if (x) {
    w = kTensIndex[63 ^ __builtin_clzll(x)];
    w += x >= kTens[w];
    return w;
  } else {
    return 1;
  }
}

/**
 * Returns `len(str(x))` where x is a signed 64-bit integer.
 */
unsigned LengthInt64(int64_t x) {
  if (x >= 0) {
    return LengthUint64(x);
  } else {
    return 1 + LengthUint64(-(uint64_t)x);
  }
}

/**
 * Returns decimal string length of uint64 w/ thousands separators.
 */
unsigned LengthUint64Thousands(uint64_t x) {
  unsigned w;
  w = LengthUint64(x);
  w += (w - 1) / 3;
  return w;
}

/**
 * Returns decimal string length of int64 w/ thousands separators.
 */
unsigned LengthInt64Thousands(int64_t x) {
  unsigned w;
  if (x >= 0) {
    w = LengthUint64(x);
    return w + (w - 1) / 3;
  } else {
    w = LengthUint64(-(uint64_t)x);
    return 1 + w + (w - 1) / 3;
  }
}
