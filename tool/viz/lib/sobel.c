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
#include "libc/calls/calls.h"
#include "libc/macros.internal.h"
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
  tmp = _mapanon((size = ROUNDUP(sizeof(float) * 4 * xn * yn, FRAMESIZE)));
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
    ConvolveGradient(g->yn, g->xn, g->b, 3, &kSobelEmbossKernelY,
                     &kSobelEmbossKernelX);
  }
}
