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
#include "libc/assert.h"
#include "tool/build/lib/alu.h"
#include "tool/build/lib/flags.h"

/**
 * NexGen32e Arithmetic Unit.
 */
int64_t Alu(int w, int h, uint64_t x, uint64_t y, uint32_t *flags) {
  uint64_t t, z, s, m, k;
  bool cf, of, zf, sf, af, carry;
  assert(w < 4);
  k = 8;
  k <<= w;
  s = 1;
  s <<= k - 1;
  m = s;
  m |= s - 1;
  t = x;
  cf = 0;
  of = 0;
  af = 0;
  carry = GetFlag(*flags, FLAGS_CF);
  switch (h & 7) {
    case ALU_OR:
      z = x | y;
      break;
    case ALU_AND:
      z = x & y;
      break;
    case ALU_XOR:
      z = x ^ y;
      break;
    case ALU_CMP:
      h |= 8;
      carry = 0;
    case ALU_SBB:
      t = (x & m) - carry;
      cf = (x & m) < (t & m);
      af = (x & 15) < (t & 15);
    case ALU_SUB:
      z = (t & m) - (y & m);
      cf |= (t & m) < (z & m);
      af |= (t & 15) < (z & 15);
      of = !!((z ^ x) & (x ^ y) & s);
      break;
    case ALU_ADC:
      t = (x & m) + carry;
      cf = (t & m) < (x & m);
      af = (t & 15) < (x & 15);
    case ALU_ADD:
      z = (t & m) + (y & m);
      cf |= (z & m) < (y & m);
      af |= (z & 15) < (y & 15);
      of = !!((z ^ x) & (z ^ y) & s);
      break;
    default:
      unreachable;
  }
  z &= m;
  zf = !z;
  sf = !!(z & s);
  *flags = (*flags & ~(1 << FLAGS_CF | 1 << FLAGS_ZF | 1 << FLAGS_SF |
                       1 << FLAGS_OF | 1 << FLAGS_AF)) |
           cf << FLAGS_CF | zf << FLAGS_ZF | sf << FLAGS_SF | of << FLAGS_OF |
           af << FLAGS_AF;
  *flags = SetLazyParityByte(*flags, x);
  if (h & ALU_TEST) z = x;
  return z;
}
