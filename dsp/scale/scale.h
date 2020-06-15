#ifndef COSMOPOLITAN_DSP_SCALE_SCALE_H_
#define COSMOPOLITAN_DSP_SCALE_SCALE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern long gyarados_latency_;

extern signed char g_magikarp[8];
extern signed char g_magkern[8];
extern const signed char kMagikarp[8][8];
extern const signed char kMagkern[8][8];

struct SamplingSolution;
void FreeSamplingSolution(struct SamplingSolution *);
struct SamplingSolution *ComputeSamplingSolution(long, long, double, double,
                                                 double);

void *Scale2xX(long ys, long xs, unsigned char[ys][xs], long, long);
void *Scale2xY(long ys, long xs, unsigned char[ys][xs], long, long);
void *Magikarp2xX(long ys, long xs, unsigned char[ys][xs], long, long);
void *Magikarp2xY(long ys, long xs, unsigned char[ys][xs], long, long);
void *Magkern2xX(long ys, long xs, unsigned char[ys][xs], long, long);
void *Magkern2xY(long ys, long xs, unsigned char[ys][xs], long, long);
void *MagikarpY(long dys, long dxs, unsigned char d[restrict dys][dxs],
                long sys, long sxs, const unsigned char s[sys][sxs], long yn,
                long xn, const signed char K[8]);

void *GyaradosUint8(long dyw, long dxw, unsigned char dst[dyw][dxw], long syw,
                    long sxw, const unsigned char src[syw][sxw], long dyn,
                    long dxn, long syn, long sxn, long lo, long hi,
                    struct SamplingSolution *cy, struct SamplingSolution *cx,
                    bool sharpen);
void *EzGyarados(long dcw, long dyw, long dxw, unsigned char dst[dcw][dyw][dxw],
                 long scw, long syw, long sxw,
                 const unsigned char src[scw][syw][sxw], long c0, long cn,
                 long dyn, long dxn, long syn, long sxn, double ry, double rx,
                 double oy, double ox);

void Decimate2xUint8x8(unsigned long n, unsigned char[n * 2],
                       const signed char[static 8]);
void *cDecimate2xUint8x8(unsigned long n, unsigned char[n * 2],
                         const signed char[8]);

void *transpose(long yn, long xn, const unsigned char[yn][xn]);
extern void (*const transpose88b)(unsigned char[8][8]);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_DSP_SCALE_SCALE_H_ */
