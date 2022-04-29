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
#include "libc/fmt/fmt.h"
#include "libc/fmt/itoa.h"
#include "libc/math.h"

static inline int CountZeroesHex(uint64_t x) {
  int n;
  for (n = 0; x;) {
    if (!(x & 15)) {
      ++n;
    }
    x >>= 4;
  }
  return n;
}

static inline int CountZeroesDec(int64_t s) {
  int n, r;
  uint64_t x;
  x = s >= 0 ? s : -(uint64_t)s;
  for (n = 0; x;) {
    r = x % 10;
    x = x / 10;
    if (!r) ++n;
  }
  return n;
}

/**
 * Formats integer using decimal or hexadecimal.
 *
 * We choose hex vs. decimal based on whichever one has the most zeroes.
 * We only bother counting zeroes for numbers outside -256 ≤ 𝑥 ≤ 256.
 */
char *FormatFlex64(char p[hasatleast 24], int64_t x, char z) {
  int zhex, zdec;
  if (-256 <= x && x <= 256) goto UseDecimal;
  zhex = CountZeroesHex(x) * 16;
  zdec = CountZeroesDec(x) * 10;
  if (zdec >= zhex) {
  UseDecimal:
    return FormatInt64(p, x);
  } else {
    return FormatHex64(p, x, z);
  }
}
