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
#include "libc/dce.h"
#include "libc/macros.h"
#include "tool/build/lib/alu.h"
#include "tool/build/lib/endian.h"
#include "tool/build/lib/flags.h"
#include "tool/build/lib/modrm.h"

/**
 * NexGen32e Bit Shift Unit.
 */
int64_t Bsu(int w, int h, uint64_t x, uint64_t y, uint32_t *f) {
  bool of;
  uint64_t s, k, t, xm, ym, cf;
  assert(w < 4);
  k = 8;
  k <<= w;
  s = 1;
  s <<= k - 1;
  xm = s;
  xm |= s - 1;
  ym = w == 3 ? 0x3F : 0x1F;
  switch (h & 7) {
    case BSU_SHR:
      x &= xm;
      if ((y &= ym)) {
        *f = SetFlag(*f, FLAGS_CF, !!(x & (1ull << (y - 1))));
        x = x >> y;
        *f = SetLazyParityByte(*f, x);
        *f = SetFlag(*f, FLAGS_OF, !!(((x << 1) ^ x) & s));
        *f = SetFlag(*f, FLAGS_ZF, !x);
        *f = SetFlag(*f, FLAGS_SF, !!(x & s));
      }
      return x;
    case BSU_SAL:
    case BSU_SHL:
      x &= xm;
      if ((y &= ym)) {
        *f = SetFlag(*f, FLAGS_CF, (cf = !!(x & (1ull << ((k - y) & ym)))));
        x = (x << y) & xm;
        *f = SetLazyParityByte(*f, x);
        *f = SetFlag(*f, FLAGS_OF, !!(x & s) ^ cf);
        *f = SetFlag(*f, FLAGS_ZF, !x);
        *f = SetFlag(*f, FLAGS_SF, !!(x & s));
      }
      return x;
    case BSU_SAR:
      x &= xm;
      if ((y &= ym)) {
        x &= xm;
        t = !!(x & s);
        x >>= (y - 1);
        if (t) x |= ~(xm >> (y - 1));
        *f = SetFlag(*f, FLAGS_CF, x & 1);
        x >>= 1;
        if (t) x = (x | ~(xm >> y)) & xm;
        *f = SetLazyParityByte(*f, x);
        *f = SetFlag(*f, FLAGS_OF, 0);
        *f = SetFlag(*f, FLAGS_ZF, !x);
        *f = SetFlag(*f, FLAGS_SF, !!(x & s));
      }
      return x;
    case BSU_ROL:
      x &= xm;
      if (y & (k - 1)) {
        y &= k - 1;
        x = (x << y | x >> (k - y)) & xm;
        *f = SetFlag(*f, FLAGS_CF, x & 1);
        *f = SetFlag(*f, FLAGS_OF, ((x >> (k - 1)) ^ x) & 1);
      } else if (y & 0x1F) {
        *f = SetFlag(*f, FLAGS_CF, x & 1);
        *f = SetFlag(*f, FLAGS_OF, ((x >> (k - 1)) ^ x) & 1);
      }
      return x;
    case BSU_ROR:
      x &= xm;
      if (y & (k - 1)) {
        y &= k - 1;
        x = (x >> y | x << (k - y)) & xm;
        *f = SetFlag(*f, FLAGS_CF, (x >> (k - 1)) & 1);
        *f = SetFlag(*f, FLAGS_OF, ((x >> (k - 2)) ^ (x >> (k - 1))) & 1);
      } else if (y & 0x1F) {
        *f = SetFlag(*f, FLAGS_CF, (x >> (k - 1)) & 1);
        *f = SetFlag(*f, FLAGS_OF, ((x >> (k - 2)) ^ (x >> (k - 1))) & 1);
      }
      return x;
    case BSU_RCR:
      x &= xm;
      if ((y = (y & ym) % (k + 1))) {
        cf = GetFlag(*f, FLAGS_CF);
        *f = SetFlag(*f, FLAGS_CF, (x >> (y - 1)) & 1);
        if (y == 1) {
          x = (x >> 1 | cf << (k - 1)) & xm;
        } else {
          x = (x >> y | cf << (k - y) | x << (k + 1 - y)) & xm;
        }
        *f = SetFlag(*f, FLAGS_OF, (((x << 1) ^ x) >> (k - 1)) & 1);
      }
      return x;
    case BSU_RCL:
      x &= xm;
      if ((y = (y & ym) % (k + 1))) {
        cf = GetFlag(*f, FLAGS_CF);
        *f = SetFlag(*f, FLAGS_CF, (t = (x >> (k - y)) & 1));
        if (y == 1) {
          x = (x << 1 | cf) & xm;
        } else {
          x = (x << y | cf << (y - 1) | x >> (k + 1 - y)) & xm;
        }
        *f = SetFlag(*f, FLAGS_OF, t ^ !!(x & s));
      }
      return x;
    default:
      unreachable;
  }
}

uint64_t BsuDoubleShift(int w, uint64_t x, uint64_t y, uint8_t b, bool isright,
                        uint32_t *f) {
  bool cf, of;
  uint64_t s, k, m, z;
  k = 8;
  k <<= w;
  s = 1;
  s <<= k - 1;
  m = s | s - 1;
  b &= w == 3 ? 63 : 31;
  x &= m;
  if (b) {
    if (isright) {
      z = x >> b | y << (k - b);
      cf = (x >> (b - 1)) & 1;
      of = b == 1 && (z & s) != (x & s);
    } else {
      z = x << b | y >> (k - b);
      cf = (x >> (k - b)) & 1;
      of = b == 1 && (z & s) != (x & s);
    }
    x = z;
    x &= m;
    *f = SetFlag(*f, FLAGS_CF, cf);
    *f = SetFlag(*f, FLAGS_OF, of);
    *f = SetFlag(*f, FLAGS_ZF, !x);
    *f = SetFlag(*f, FLAGS_SF, !!(x & s));
    *f = SetLazyParityByte(*f, x & 0xff);
  }
  return x;
}
