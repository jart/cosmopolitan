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
#include "libc/macros.internal.h"
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
