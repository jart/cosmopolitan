/*-*- mode:c;indent-tabs-mode:t;c-basic-offset:4;tab-width:4;coding:utf-8   -*-│
│ vi: set et ft=c ts=4 sw=4 fenc=utf-8                                     :vi │
╞══════════════════════════════════════════════════════════════════════════════╡
│  PL_MPEG - MPEG1 Video decoder, MP2 Audio decoder, MPEG-PS demuxer           │
│  Dominic Szablewski - https://phoboslab.org                                  │
│                                                                              │
│  The MIT License(MIT)                                                        │
│  Copyright(c) 2019 Dominic Szablewski                                        │
│                                                                              │
│  Permission is hereby granted, free of charge, to any person obtaining       │
│  a copy of this software and associated documentation files(the              │
│  "Software"), to deal in the Software without restriction, including         │
│  without limitation the rights to use, copy, modify, merge, publish,         │
│  distribute, sublicense, and / or sell copies of the Software, and to        │
│  permit persons to whom the Software is furnished to do so, subject to       │
│  the following conditions:                                                   │
│                                                                              │
│    The above copyright notice and this permission notice shall be            │
│    included in all copies or substantial portions of the Software.           │
│                                                                              │
│  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,             │
│  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF          │
│  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                       │
│  NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE       │
│  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN             │
│  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN           │
│  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE            │
│  SOFTWARE.                                                                   │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "dsp/core/half.h"

asm(".ident\t\"\\n\\n\
PL_MPEG (MIT License)\\n\
Copyright(c) 2019 Dominic Szablewski\\n\
https://phoboslab.org\"");
asm(".include \"libc/disclaimer.inc\"");

/**
 * Computes Fixed-Point 8x8 Inverse Discrete Cosine Transform.
 *
 * @note discovered by Nasir Ahmed
 */
void plm_video_idct(int block[8][8]) {
  int i, t1, t2, m0;
  int b1, b3, b4, b6, b7;
  int y3, y4, y5, y6, y7;
  int x0, x1, x2, x3, x4;

  for (i = 0; i < 8; ++i) {
    b1 = block[4][i];
    b3 = block[2][i] + block[6][i];
    b4 = block[5][i] - block[3][i];
    t1 = block[1][i] + block[7][i];
    t2 = block[3][i] + block[5][i];
    b6 = block[1][i] - block[7][i];
    b7 = t1 + t2;
    m0 = block[0][i];
    x4 = ((b6 * 473 - b4 * 196 + 128) >> 8) - b7;
    x0 = x4 - (((t1 - t2) * 362 + 128) >> 8);
    x1 = m0 - b1;
    x2 = (((block[2][i] - block[6][i]) * 362 + 128) >> 8) - b3;
    x3 = m0 + b1;
    y3 = x1 + x2;
    y4 = x3 + b3;
    y5 = x1 - x2;
    y6 = x3 - b3;
    y7 = -x0 - ((b4 * 473 + b6 * 196 + 128) >> 8);
    block[0][i] = b7 + y4;
    block[1][i] = x4 + y3;
    block[2][i] = y5 - x0;
    block[3][i] = y6 - y7;
    block[4][i] = y6 + y7;
    block[5][i] = x0 + y5;
    block[6][i] = y3 - x4;
    block[7][i] = y4 - b7;
  }

  for (i = 0; i < 8; ++i) {
    b1 = block[i][4];
    b3 = block[i][2] + block[i][6];
    b4 = block[i][5] - block[i][3];
    t1 = block[i][1] + block[i][7];
    t2 = block[i][3] + block[i][5];
    b6 = block[i][1] - block[i][7];
    b7 = t1 + t2;
    m0 = block[i][0];
    x4 = ((b6 * 473 - b4 * 196 + 128) >> 8) - b7;
    x0 = x4 - (((t1 - t2) * 362 + 128) >> 8);
    x1 = m0 - b1;
    x2 = (((block[i][2] - block[i][6]) * 362 + 128) >> 8) - b3;
    x3 = m0 + b1;
    y3 = x1 + x2;
    y4 = x3 + b3;
    y5 = x1 - x2;
    y6 = x3 - b3;
    y7 = -x0 - ((b4 * 473 + b6 * 196 + 128) >> 8);
    block[i][0] = (b7 + y4 + 128) >> 8;
    block[i][1] = (x4 + y3 + 128) >> 8;
    block[i][2] = (y5 - x0 + 128) >> 8;
    block[i][3] = (y6 - y7 + 128) >> 8;
    block[i][4] = (y6 + y7 + 128) >> 8;
    block[i][5] = (x0 + y5 + 128) >> 8;
    block[i][6] = (y3 - x4 + 128) >> 8;
    block[i][7] = (y4 - b7 + 128) >> 8;
  }
}
