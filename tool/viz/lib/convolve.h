#ifndef COSMOPOLITAN_TOOL_VIZ_LIB_CONVOLVE_H_
#define COSMOPOLITAN_TOOL_VIZ_LIB_CONVOLVE_H_
#include "dsp/tty/quant.h"
#include "libc/mem/mem.h"
#include "libc/str/str.h"
#include "tool/viz/lib/graphic.h"
COSMOPOLITAN_C_START_

typedef float convolve_t __attribute__((__vector_size__(16)));

forceinline void convolve(unsigned yn, unsigned xn, ttyrgb_m128 img[yn][xn],
                          int KW, const float kernel[KW][KW], float C1,
                          float C2) {
  /* TODO(jart): nontemporal herringbone strategy */
  float f;
  struct Graphic g;
  unsigned y, x, i, j;
  convolve_t p, kflip[KW][KW], (*tmp)[yn][xn];
  for (i = 0; i < KW; ++i) {
    for (j = 0; j < KW; ++j) {
      f = kernel[i][j] / C1;
      kflip[KW - i - 1][KW - j - 1] = (convolve_t){f, f, f, f};
    }
  }
  bzero(&g, sizeof(g));
  resizegraphic(&g, yn, xn);
  tmp = g.b;
  for (y = 0; y < yn - KW; ++y) {
    for (x = 0; x < xn - KW; ++x) {
      bzero(&p, sizeof(p));
      for (i = 0; i < KW; ++i) {
        for (j = 0; j < KW; ++j) {
          p += img[y + i][x + j] * kflip[i][j] + C2;
        }
      }
      memcpy(&(*tmp)[y + KW / 2][x + KW / 2], &p, sizeof(p));
    }
  }
  memcpy(img, tmp, yn * xn * sizeof(img[0][0]));
  free(g.b);
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_VIZ_LIB_CONVOLVE_H_ */
