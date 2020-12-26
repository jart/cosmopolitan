/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ This program is free software; you can redistribute it and/or modify         │
│ it under the terms of the GNU General Public License as published by         │
│ the Free Software Foundation; version 2 of the License.                      │
│                                                                              │
│ This program is distributed in the hope that it will be useful, but          │
│ WITHOUT ANY WARRANTY; without even the implied warranty of                   │
│ MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU             │
│ General Public License for more details.                                     │
│                                                                              │
│ You should have received a copy of the GNU General Public License            │
│ along with this program; if not, write to the Free Software                  │
│ Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA                │
│ 02110-1301 USA                                                               │
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
