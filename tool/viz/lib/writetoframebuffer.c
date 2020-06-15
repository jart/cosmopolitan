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
#include "libc/macros.h"
#include "tool/viz/lib/graphic.h"

void WriteToFrameBuffer(size_t dyn, size_t dxn, unsigned char dst[dyn][dxn][4],
                        size_t syn, size_t sxn, float src[syn][sxn][4],
                        size_t yn, size_t xn) {
  int ipix[4];
  float fpix[4];
  unsigned y, x, k, upix[4];
  for (y = 0; y < yn; ++y) {
    for (x = 0; x < xn; ++x) {
      for (k = 0; k < 4; ++k) fpix[k] = src[y][x][k];
      for (k = 0; k < 4; ++k) fpix[k] *= 255;
      for (k = 0; k < 4; ++k) ipix[k] = fpix[k] + .5f;
      for (k = 0; k < 4; ++k) upix[k] = MAX(0, ipix[k]);
      for (k = 0; k < 4; ++k) upix[k] = MIN(255, upix[k]);
      dst[y][x][0] = upix[2];
      dst[y][x][1] = upix[1];
      dst[y][x][2] = upix[0];
      dst[y][x][3] = 0;
    }
  }
}
