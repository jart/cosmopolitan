#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_X87_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_X87_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

long double f2xm1(long double);
long double fyl2x(long double, long double);
long double fyl2xp1(long double, long double);
long double fscale(long double, long double);
long double fprem(long double, long double, uint32_t *);
long double fprem1(long double, long double, uint32_t *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_X87_H_ */
