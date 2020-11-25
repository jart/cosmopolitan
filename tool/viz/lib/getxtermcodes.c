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
#include "dsp/tty/quant.h"
#include "libc/bits/xmmintrin.internal.h"
#include "libc/macros.h"
#include "tool/viz/lib/graphic.h"

void getxtermcodes(struct TtyRgb *p, const struct Graphic *g) {
  unsigned y, x;
  unsigned char(*img)[3][g->yn][g->xn] = g->b.p;
  for (y = 0; y < g->yn; ++y) {
    for (x = 0; x < g->xn; ++x) {
      *p++ = rgb2tty((*img)[0][y][x], (*img)[1][y][x], (*img)[2][y][x]);
    }
  }
}
