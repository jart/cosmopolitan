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
#include "dsp/tty/rgb2xterm256.h"

/* 1bc */
forceinline int sqr(int x) { return x * x; }
/* forceinline int dst6(int x) { return x * x; } */
int rgb2xterm256v2(int r, int g, int b) {
  static const int i2cv[] = {0, 0137, 0207, 0257, 0327, 0377, 0377};
#define v2ci(v)                (v < 060 ? 0 : v < 0163 ? 01 : (v - 043) / 050)
#define dst6(A, B, C, a, b, c) (sqr(A - a) + sqr(B - b) + sqr(C - c))
  int ir = v2ci(r);
  int ig = v2ci(g);
  int ib = v2ci(b);
  int avg = (r + g + b) / 3;
  int cr = i2cv[ir];
  int cg = i2cv[ig];
  int cb = i2cv[ib];
  int gidx = avg > 238 ? 23 : (avg - 3) / 10;
  int gv = 8 + 10 * gidx;
  int cerr = dst6(cr, cg, cb, r, g, b);
  int gerr = dst6(gv, gv, gv, r, g, b);
  return cerr <= gerr ? 16 + (36 * ir + 6 * ig + ib) : 232 + gidx;
#undef dst6
#undef cidx
#undef v2ci
}

/* 1e3 */
// Convert RGB24 to xterm-256 8-bit value
// For simplicity, assume RGB space is perceptually uniform.
// There are 5 places where one of two outputs needs to be chosen when
// input is the exact middle:
// - The r/g/b channels and the gray value: choose higher value output
// - If gray and color have same distance from input - choose color
int rgb2xterm256(uint8_t r, uint8_t g, uint8_t b) {
  // Calculate the nearest 0-based color index at 16 .. 231
#define v2ci(v) (v < 48 ? 0 : v < 115 ? 1 : (v - 35) / 40)
  int ir = v2ci(r), ig = v2ci(g), ib = v2ci(b);  // 0..5 each
#define color_index() (36 * ir + 6 * ig + ib)    /* 0..215, lazy eval */
  // Calculate the nearest 0-based gray index at 232 .. 255
  int average = (r + g + b) / 3;
  int gray_index = average > 238 ? 23 : (average - 3) / 10;  // 0..23
  // Calculate the represented colors back from the index
  static const int i2cv[6] = {0, 0x5f, 0x87, 0xaf, 0xd7, 0xff};
  int cr = i2cv[ir], cg = i2cv[ig], cb = i2cv[ib];  // r/g/b, 0..255 each
  int gv = 8 + 10 * gray_index;  // same value for r/g/b, 0..255
// Return the one which is nearer to the original input rgb value
#define dist_square(A, B, C, a, b, c) \
  ((A - a) * (A - a) + (B - b) * (B - b) + (C - c) * (C - c))
  int color_err = dist_square(cr, cg, cb, r, g, b);
  int gray_err = dist_square(gv, gv, gv, r, g, b);
  return color_err <= gray_err ? 16 + color_index() : 232 + gray_index;
}
