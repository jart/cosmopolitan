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
#include "ape/lib/pc.h"
#include "libc/math.h"
#include "tool/build/lib/x87.h"

static long ltruncl(long double x) {
  return x;
}

static int ClearC2(int sw) {
  return sw & ~FPU_C2;
}

static long double x87remainder(long double x, long double y, uint32_t *sw,
                                long double rem(long double, long double),
                                long rnd(long double)) {
  int s;
  long q;
  long double r;
  s = 0;
  r = rem(x, y);
  q = rnd(x / y);
  s &= ~FPU_C2; /* ty libm */
  if (q & 0b001) s |= FPU_C1;
  if (q & 0b010) s |= FPU_C3;
  if (q & 0b100) s |= FPU_C0;
  if (sw) *sw = s | (*sw & ~(FPU_C0 | FPU_C1 | FPU_C2 | FPU_C3));
  return r;
}

long double f2xm1(long double x) {
  return exp2l(x) - 1;
}

long double fyl2x(long double x, long double y) {
  return y * log2l(x);
}

long double fyl2xp1(long double x, long double y) {
  return y * log2l(x + 1);
}

long double fscale(long double significand, long double exponent) {
  return scalbl(significand, exponent);
}

long double fprem(long double dividend, long double modulus, uint32_t *sw) {
  return x87remainder(dividend, modulus, sw, fmodl, ltruncl);
}

long double fprem1(long double dividend, long double modulus, uint32_t *sw) {
  return x87remainder(dividend, modulus, sw, remainderl, lrintl);
}
