#ifndef COSMOPOLITAN_TOOL_VIZ_LIB_CONVOLUTION_H_
#define COSMOPOLITAN_TOOL_VIZ_LIB_CONVOLUTION_H_
#include "dsp/core/q.h"
/* clang-format off */

#define CONVROUND(X) ((int)rint(X))

#define CONVOLVE1X1(COEFFICIENT, DY, DX) (COEFFICIENT * (img[iy[y DY]][ix[x DX]]))
#define CONVOLVE1X12(IMG, COEFFICIENT, DY, DX) (COEFFICIENT * IMG[iy[y DY]][ix[x DX]])

#define CONVOLVE3X3(NORMALIZER,                                         \
                    C00, C01, C02,                                      \
                    C10, C11, C12,                                      \
                    C20, C21, C22)                                      \
  (NORMALIZER *                                                         \
   (CONVOLVE1X1(C00,-1,-1)+CONVOLVE1X1(C01,-1,+0)+CONVOLVE1X1(C02,-1,+1)+ \
    CONVOLVE1X1(C10,+0,-1)+CONVOLVE1X1(C11,+0,+0)+CONVOLVE1X1(C12,+0,+1)+ \
    CONVOLVE1X1(C20,+1,-1)+CONVOLVE1X1(C21,+1,+0)+CONVOLVE1X1(C22,+1,+1)))

#define CONVOLVE3X6(NORMALIZER,                                         \
                    C00, C01, C02,                                      \
                    C10, C11, C12,                                      \
                    C20, C21, C22)                                      \
  (NORMALIZER *                                                         \
   (CONVOLVE1X1(C00/2.f,-1,-1)+CONVOLVE1X1(C01/2.f,-1,+0)+CONVOLVE1X1(C02/2.f,-1,+1)+ \
    CONVOLVE1X1(C00/2.f,-1,-1)+CONVOLVE1X1(C01/2.f,-1,+0)+CONVOLVE1X1(C02/2.f,-1,+1)+ \
    CONVOLVE1X1(C10/2.f,+0,-1)+CONVOLVE1X1(C11/2.f,+0,+0)+CONVOLVE1X1(C12/2.f,+0,+1)+ \
    CONVOLVE1X1(C10/2.f,+0,-1)+CONVOLVE1X1(C11/2.f,+0,+0)+CONVOLVE1X1(C12/2.f,+0,+1)+ \
    CONVOLVE1X1(C20/2.f,+1,-1)+CONVOLVE1X1(C21/2.f,+1,+0)+CONVOLVE1X1(C22/2.f,+1,+1)+ \
    CONVOLVE1X1(C20/2.f,+1,-1)+CONVOLVE1X1(C21/2.f,+1,+0)+CONVOLVE1X1(C22/2.f,+1,+1)))

/* clang-format on */

#define CONVOLVE5X5(M, NORM, IMG, C00, C01, C02, C03, C04, C10, C11, C12, C13, \
                    C14, C20, C21, C22, C23, C24, C30, C31, C32, C33, C34,     \
                    C40, C41, C42, C43, C44)                                   \
  QRS(M, (CONVOLVE1X12(IMG, CONVROUND((NORM * C00) * (1 << M)), -2, -2) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C01) * (1 << M)), -2, -1) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C02) * (1 << M)), -2, +0) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C03) * (1 << M)), -2, +1) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C04) * (1 << M)), -2, +2) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C10) * (1 << M)), -1, -2) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C11) * (1 << M)), -1, -1) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C12) * (1 << M)), -1, +0) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C13) * (1 << M)), -1, +1) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C14) * (1 << M)), -1, +2) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C20) * (1 << M)), +0, -2) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C21) * (1 << M)), +0, -1) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C22) * (1 << M)), +0, +0) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C23) * (1 << M)), +0, +1) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C24) * (1 << M)), +0, +2) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C30) * (1 << M)), +1, -2) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C31) * (1 << M)), +1, -1) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C32) * (1 << M)), +1, +0) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C33) * (1 << M)), +1, +1) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C34) * (1 << M)), +1, +2) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C40) * (1 << M)), +2, -2) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C41) * (1 << M)), +2, -1) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C42) * (1 << M)), +2, +0) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C43) * (1 << M)), +2, +1) +      \
          CONVOLVE1X12(IMG, CONVROUND((NORM * C44) * (1 << M)), +2, +2)))

unsigned *convoindex(unsigned, unsigned, unsigned) mallocesque;

#endif /* COSMOPOLITAN_TOOL_VIZ_LIB_CONVOLUTION_H_ */
