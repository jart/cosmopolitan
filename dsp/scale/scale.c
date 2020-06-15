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
#include "dsp/core/half.h"
#include "dsp/scale/scale.h"

void *Scale2xX(long ys, long xs, unsigned char p[ys][xs], long yn, long xn) {
  long y, x, w;
  for (w = HALF(xn), y = 0; y < yn; ++y) {
    for (x = 0; x < w; ++x) {
      p[y][x] = p[y][x * 2];
    }
  }
  return p;
}

void *Scale2xY(long ys, long xs, unsigned char p[ys][xs], long yn, long xn) {
  long y, x, h;
  for (h = HALF(yn), y = 0; y < h; ++y) {
    for (x = 0; x < xn; ++x) {
      p[y][x] = p[y * 2][x];
    }
  }
  return p;
}
