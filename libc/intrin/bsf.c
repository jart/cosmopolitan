/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│ vi: set et ft=c ts=2 sts=2 sw=2 fenc=utf-8                               :vi │
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
#include "libc/intrin/bsf.h"

/**
 * Returns position of first bit set.
 *
 *                           ctz(𝑥)         31^clz(𝑥)   clz(𝑥)
 *       uint32 𝑥  _bsf(𝑥) tzcnt(𝑥)   ffs(𝑥)  _bsr(𝑥) lzcnt(𝑥)
 *     0x00000000      wut       32        0      wut       32
 *     0x00000001        0        0        1        0       31
 *     0x80000001        0        0        1       31        0
 *     0x80000000       31       31       32       31        0
 *     0x00000010        4        4        5        4       27
 *     0x08000010        4        4        5       27        4
 *     0x08000000       27       27       28       27        4
 *     0xffffffff        0        0        1       31        0
 *
 * @param 𝑥 is a 64-bit integer
 * @return number in range 0..63 or undefined if 𝑥 is 0
 */
int(_bsfl)(long x) {
  unsigned l, r;
  x &= -x;
  l = x | x >> 32;
  r = !!(x >> 32), r <<= 1;
  r += !!((l & 0xffff0000)), r <<= 1;
  r += !!((l & 0xff00ff00)), r <<= 1;
  r += !!((l & 0xf0f0f0f0)), r <<= 1;
  r += !!((l & 0xcccccccc)), r <<= 1;
  r += !!((l & 0xaaaaaaaa));
  return r;
}

/**
 * Returns position of first bit set.
 *
 *                           ctz(𝑥)         31^clz(𝑥)   clz(𝑥)
 *       uint32 𝑥  _bsf(𝑥) tzcnt(𝑥)   ffs(𝑥)  _bsr(𝑥) lzcnt(𝑥)
 *     0x00000000      wut       32        0      wut       32
 *     0x00000001        0        0        1        0       31
 *     0x80000001        0        0        1       31        0
 *     0x80000000       31       31       32       31        0
 *     0x00000010        4        4        5        4       27
 *     0x08000010        4        4        5       27        4
 *     0x08000000       27       27       28       27        4
 *     0xffffffff        0        0        1       31        0
 *
 * @param x is a 32-bit integer
 * @return number in range 0..31 or undefined if 𝑥 is 0
 */
int(_bsf)(int x) {
  return _bsf((unsigned)x);
}

__weak_reference(_bsfl, _bsfll);
