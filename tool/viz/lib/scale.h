#ifndef COSMOPOLITAN_TOOL_VIZ_LIB_SCALE_H_
#define COSMOPOLITAN_TOOL_VIZ_LIB_SCALE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void *Scale(long dcw, long dyw, long dxw, float dst[dcw][dyw][dxw], long scw,
            long syw, long sxw, const float src[scw][syw][sxw], long, long,
            long, long, long, long, double, double, double, double);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_VIZ_LIB_SCALE_H_ */
