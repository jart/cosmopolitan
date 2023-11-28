#ifndef COSMOPOLITAN_TOOL_VIZ_LIB_YCBCR_H_
#define COSMOPOLITAN_TOOL_VIZ_LIB_YCBCR_H_
COSMOPOLITAN_C_START_

struct YCbCr;

extern const double kBt601Primaries[3];
extern const double kBt709Primaries[3];

extern long magikarp_latency_;
extern long ycbcr2rgb_latency_;

void Y2Rgb(long yn, long xn, unsigned char[restrict 3][yn][xn], long yys,
           long yxs, const unsigned char[restrict yys][yxs], const int[8][4],
           const unsigned char[256]);
void YCbCr2Rgb(long yn, long xn, unsigned char[restrict 3][yn][xn], long yys,
               long yxs, const unsigned char[restrict yys][yxs], long cys,
               long cxs, const unsigned char[restrict cys][cxs],
               const unsigned char[restrict cys][cxs], const int[8][4],
               const int[6][4], const unsigned char[256]);

void YCbCrFree(struct YCbCr **);
void YCbCrInit(struct YCbCr **, bool, int, double, const double[3],
               const double[3]);
void *YCbCr2RgbScale(long dyn, long dxn, unsigned char[restrict 3][dyn][dxn],
                     long yys, long yxs, unsigned char[restrict yys][yxs],
                     long cys, long cxs, unsigned char[restrict cys][cxs],
                     unsigned char[restrict cys][cxs], long, long, long, long,
                     double, double, double, double, struct YCbCr **);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_VIZ_LIB_YCBCR_H_ */
