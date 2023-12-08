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
#include "libc/intrin/bsr.h"
// clang-format off

/**
 * Returns binary logarithm of 𝑥.
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
int(_bsr)(int x) {
  int r = 0;
  if(x & 0xFFFF0000u) { x >>= 16; r |= 16; }
  if(x & 0xFF00) { x >>= 8; r |= 8; }
  if(x & 0xF0) { x >>= 4; r |= 4; }
  if(x & 0xC) { x >>= 2; r |= 2; }
  if(x & 0x2) { r |= 1; }
  return r;
}
