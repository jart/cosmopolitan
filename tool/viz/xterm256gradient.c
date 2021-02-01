/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2021 Justine Alexandra Roberts Tunney                              │
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
#include "libc/math.h"
#include "libc/stdio/stdio.h"

#define N 8

#define SQR(X) ((X) * (X))

static const uint8_t kXtermCube[6] = {0, 0137, 0207, 0257, 0327, 0377};

static int rgb2xterm256(int r, int g, int b) {
  int cerr, gerr, ir, ig, ib, gray, grai, cr, cg, cb, gv;
  gray = round(r * .299 + g * .587 + b * .114);
  grai = gray > 238 ? 23 : (gray - 3) / 10;
  ir = r < 48 ? 0 : r < 115 ? 1 : (r - 35) / 40;
  ig = g < 48 ? 0 : g < 115 ? 1 : (g - 35) / 40;
  ib = b < 48 ? 0 : b < 115 ? 1 : (b - 35) / 40;
  cr = kXtermCube[ir];
  cg = kXtermCube[ig];
  cb = kXtermCube[ib];
  gv = 8 + 10 * grai;
  cerr = SQR(cr - r) + SQR(cg - g) + SQR(cb - b);
  gerr = SQR(gv - r) + SQR(gv - g) + SQR(gv - b);
  if (cerr <= gerr) {
    return 16 + 36 * ir + 6 * ig + ib;
  } else {
    return 232 + grai;
  }
}

int main(int argc, char *argv[]) {
  double d;
  int i, j, x;
  int r, g, b;
  double G[N][3];
  double rgb[2][3] = {
      {1, 0, 0},
      {0, 1, 0},
  };

  for (i = 0; i < N; ++i) {
    for (j = 0; j < 3; ++j) {
      d = (rgb[1][j] - rgb[0][j]) / (N - 1);
      G[i][j] = rgb[0][j] + d * i;
    }
  }

  for (i = 0; i < N; ++i) {
    r = round(G[i][0] * 255);
    g = round(G[i][1] * 255);
    b = round(G[i][2] * 255);
    x = rgb2xterm256(r, g, b);
    printf("\e[38;5;232;48;5;%dmabcdefg       \e[0m %3d "
           "\e[38;5;232;48;2;%d;%d;%dmabcdgefg      \e[0m "
           "%3d %3d %3d %f %f %f\n",
           x, x, r, g, b, r, g, b, G[i][0], G[i][1], G[i][2]);
  }

  return 0;
}
