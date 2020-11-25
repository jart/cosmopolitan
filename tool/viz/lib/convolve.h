#ifndef COSMOPOLITAN_TOOL_VIZ_LIB_CONVOLVE_H_
#define COSMOPOLITAN_TOOL_VIZ_LIB_CONVOLVE_H_
#include "libc/bits/xmmintrin.internal.h"
#include "libc/str/str.h"
#include "tool/viz/lib/graphic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

forceinline void convolve(unsigned yn, unsigned xn, __m128 img[yn][xn], int KW,
                          const float kernel[KW][KW], float C1, float C2) {
  /* TODO(jart): nontemporal herringbone strategy */
  float f;
  struct Graphic g;
  unsigned y, x, i, j;
  __v4sf p, kflip[KW][KW], (*tmp)[yn][xn];
  for (i = 0; i < KW; ++i) {
    for (j = 0; j < KW; ++j) {
      f = kernel[i][j] / C1;
      kflip[KW - i - 1][KW - j - 1] = (__v4sf){f, f, f, f};
    }
  }
  memset(&g, 0, sizeof(g));
  resizegraphic(&g, yn, xn);
  tmp = g.b.p;
  for (y = 0; y < yn - KW; ++y) {
    for (x = 0; x < xn - KW; ++x) {
      memset(&p, 0, sizeof(p));
      for (i = 0; i < KW; ++i) {
        for (j = 0; j < KW; ++j) {
          p += img[y + i][x + j] * kflip[i][j] + C2;
        }
      }
      memcpy(&(*tmp)[y + KW / 2][x + KW / 2], &p, sizeof(p));
    }
  }
  memcpy(img, tmp, yn * xn * sizeof(img[0][0]));
  bfree(&g.b);
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_VIZ_LIB_CONVOLVE_H_ */
