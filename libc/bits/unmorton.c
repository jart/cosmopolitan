/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "libc/bits/morton.h"

static unsigned long GetOddBits(unsigned long x) {
  x = (x | x >> 000) & 0x5555555555555555;
  x = (x | x >> 001) & 0x3333333333333333;
  x = (x | x >> 002) & 0x0F0F0F0F0F0F0F0F;
  x = (x | x >> 004) & 0x00FF00FF00FF00FF;
  x = (x | x >> 010) & 0x0000FFFF0000FFFF;
  x = (x | x >> 020) & 0x00000000FFFFFFFF;
  return x;
}

/**
 * Deinterleaves bits.
 *
 * @param 𝑖 is interleaved index
 * @return deinterleaved coordinate {ax := 𝑦, dx := 𝑥}
 * @see en.wikipedia.org/wiki/Z-order_curve
 * @see morton()
 */
axdx_t(unmorton)(unsigned long i) {
  return (axdx_t){GetOddBits(i >> 1), GetOddBits(i)};
}
