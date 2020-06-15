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
#include "libc/calls/calls.h"
#include "libc/macros.h"
#include "libc/math.h"
#include "libc/mem/mem.h"
#include "libc/nexgen32e/nexgen32e.h"
#include "libc/runtime/runtime.h"
#include "tool/viz/lib/convolve.h"
#include "tool/viz/lib/graphic.h"

#define LCB           {
#define RCB           }
#define BROADCAST(X)  LCB X, X, X, X RCB
#define FLIP(A, B, C) LCB C, B, A RCB

forceinline void ConvolveGradient(unsigned yn, unsigned xn,
                                  float (*img)[yn][xn][4], unsigned KW,
                                  const float (*ky)[KW][KW][4],
                                  const float (*kx)[KW][KW][4]) {
  size_t size;
  unsigned y, x, i, j, k;
  float py[4], px[4], (*tmp)[yn][xn][4];
  tmp = mapanon((size = ROUNDUP(sizeof(float) * 4 * xn * yn, FRAMESIZE)));
  for (y = 0; y < yn - KW + 1; ++y) {
    for (x = 0; x < xn - KW + 1; ++x) {
      for (k = 0; k < 4; ++k) py[k] = 0;
      for (k = 0; k < 4; ++k) px[k] = 0;
      for (i = 0; i < KW; ++i) {
        for (j = 0; j < KW; ++j) {
          for (k = 0; k < 4; ++k) {
            py[k] += (*ky)[i][j][k] * (*img)[y + i][x + j][k];
            px[k] += (*kx)[i][j][k] * (*img)[y + i][x + j][k];
          }
        }
      }
      for (k = 0; k < 4; ++k) {
        (*tmp)[y + KW / 2][x + KW / 2][k] = sqrt(py[k] * py[k] + px[k] * px[k]);
      }
    }
  }
  memcpy(img, tmp, sizeof(float) * 4 * xn * yn);
  munmap(tmp, size);
}

void sobel(struct Graphic* g) {
  static const float kSobelEmbossKernelY[3][3][4] =
      FLIP(FLIP(BROADCAST(+1), BROADCAST(+2), BROADCAST(+1)),
           FLIP(BROADCAST(+0), BROADCAST(+0), BROADCAST(+0)),
           FLIP(BROADCAST(-1), BROADCAST(-2), BROADCAST(-1)));
  static const float kSobelEmbossKernelX[3][3][4] =
      FLIP(FLIP(BROADCAST(-1), BROADCAST(+0), BROADCAST(+1)),
           FLIP(BROADCAST(-2), BROADCAST(+0), BROADCAST(+2)),
           FLIP(BROADCAST(-1), BROADCAST(+0), BROADCAST(+1)));
  if (g->yn >= 3 && g->xn >= 3) {
    ConvolveGradient(g->yn, g->xn, g->b.p, 3, &kSobelEmbossKernelY,
                     &kSobelEmbossKernelX);
  }
}
