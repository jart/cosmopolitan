#ifndef COSMOPOLITAN_TOOL_VIZ_LIB_BILINEARSCALE_H_
#define COSMOPOLITAN_TOOL_VIZ_LIB_BILINEARSCALE_H_
COSMOPOLITAN_C_START_

void *BilinearScale(long dcw, long dyw, long dxw,
                    unsigned char dst[dcw][dyw][dxw], long scw, long syw,
                    long sxw, const unsigned char src[scw][syw][sxw], long,
                    long, long, long, long, long, double, double, double,
                    double);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_VIZ_LIB_BILINEARSCALE_H_ */
