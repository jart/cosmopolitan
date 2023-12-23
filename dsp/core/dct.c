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
#include "dsp/core/core.h"

#define DCT(A, B, C, D, E, F, G, H, T, C0, C1, C2, C3, C4) \
  do {                                                     \
    T z1, z2, z3, z4, z5, z11, z13;                        \
    T t0, t1, t2, t3, t4, t5, t6, t7, t10, t11, t12, t13;  \
    t0 = A + H;                                            \
    t7 = A - H;                                            \
    t1 = B + G;                                            \
    t6 = B - G;                                            \
    t2 = C + F;                                            \
    t5 = C - F;                                            \
    t3 = D + E;                                            \
    t4 = D - E;                                            \
    t10 = t0 + t3;                                         \
    t13 = t0 - t3;                                         \
    t11 = t1 + t2;                                         \
    t12 = t1 - t2;                                         \
    A = t10 + t11;                                         \
    E = t10 - t11;                                         \
    z1 = (t12 + t13) * C0;                                 \
    C = t13 + z1;                                          \
    G = t13 - z1;                                          \
    t10 = t4 + t5;                                         \
    t11 = t5 + t6;                                         \
    t12 = t6 + t7;                                         \
    z5 = (t10 - t12) * C1;                                 \
    z2 = t10 * C2 + z5;                                    \
    z4 = t12 * C3 + z5;                                    \
    z3 = t11 * C4;                                         \
    z11 = t7 + z3;                                         \
    z13 = t7 - z3;                                         \
    F = z13 + z2;                                          \
    D = z13 - z2;                                          \
    B = z11 + z4;                                          \
    H = z11 - z4;                                          \
  } while (0)

/**
 * Performs float forward discrete cosine transform.
 *
 * This takes a tiny block of image data and shoves the information it
 * represents into the top left corner. It can be reversed using idct().
 * It matters because iterating the result in a serpentine pattern makes
 * run-length delta encoding super effective. Furthermore, data downward
 * rightly may be divided away for lossy compression.
 *
 * @cost ~100ns
 */
void *dct(float M[restrict hasatleast 8][8], unsigned stride,
          float c0, float c1, float c2, float c3, float c4) {
  unsigned y, x;
  for (y = 0; y < stride * 8; y += stride) {
    DCT(M[y][0], M[y][1], M[y][2], M[y][3], M[y][4], M[y][5], M[y][6], M[y][7],
        float, c0, c1, c2, c3, c4);
  }
  for (x = 0; x < stride * 8; x += stride) {
    DCT(M[0][x], M[1][x], M[2][x], M[3][x], M[4][x], M[5][x], M[6][x], M[7][x],
        float, c0, c1, c2, c3, c4);
  }
  return M;
}

void *dctjpeg(float M[restrict hasatleast 8][8], unsigned stride) {
  return dct(M, stride, .707106781f, .382683433f, .541196100f, 1.306562965f,
             .707106781f);
}
