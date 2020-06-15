#ifndef COSMOPOLITAN_TOOL_VIZ_LIB_GRAPHIC_H_
#define COSMOPOLITAN_TOOL_VIZ_LIB_GRAPHIC_H_
#include "dsp/tty/quant.h"
#include "libc/runtime/buffer.h"
#include "libc/runtime/gc.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Graphic {
  union {
    struct GuardedBuffer b;
    char *bytes;
    float (*lum)[2][8];
    float (*rgba)[2][2];
  };
  unsigned yn, xn;
  unsigned ys, xs;
  unsigned bs, es;
};

void dither(long yw, long xw, unsigned char[3][yw][xw], long, long);
struct Graphic *resizegraphic(struct Graphic *, size_t, size_t);
void getxtermcodes(struct TtyRgb *, const struct Graphic *);
void ycbcr2lum(struct Graphic *, uint8_t *, uint8_t *, uint8_t *, size_t,
               size_t, size_t);

long sharpen(long cn, long yw, long xw, unsigned char[cn][yw][xw], long, long);
long unsharp(long cn, long yw, long xw, unsigned char[cn][yw][xw], long, long);
long gaussian(long yn, long xn, unsigned char[3][yn][xn]);
void sobel(struct Graphic *);
extern void (*ycbcr2rgb)(struct Graphic *, uint8_t *, uint8_t *, uint8_t *,
                         size_t, size_t, size_t);

void emboss(struct Graphic *);
void boxblur(struct Graphic *);
double perlin3(double, double, double);

void stdgamma(unsigned n, __m128 rgba[n]);
void lingamma(unsigned n, __m128 rgba[n]);

void OldBilinearScale(size_t dyw, size_t dxw, __v4sf dst[dyw][dxw], size_t syw,
                      size_t sxw, __v4sf src[syw][sxw], size_t dyn, size_t dxn,
                      size_t syn, size_t sxn);

int MagicScale(unsigned dyn, unsigned dxn, __v4sf dst[dyn][dxn], unsigned syn,
               unsigned sxn, __v4sf src[syn][sxn]);

void interlace(size_t dyn, size_t dxn, float dst[dyn][dxn][4], size_t syn,
               size_t sxn, size_t ssw, unsigned char reds[syn][ssw],
               unsigned char greens[syn][ssw], unsigned char blues[syn][ssw]);

void WriteToFrameBuffer(size_t dyn, size_t dxn, unsigned char dst[dyn][dxn][4],
                        size_t syn, size_t sxn, float src[syn][sxn][4],
                        size_t yn, size_t xn);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_VIZ_LIB_GRAPHIC_H_ */
