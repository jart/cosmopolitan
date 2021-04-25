#ifndef COSMOPOLITAN_LIBC_RUNTIME_CLKTCK_H_
#define COSMOPOLITAN_LIBC_RUNTIME_CLKTCK_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define CLK_TCK (__clk_tck())

int __clk_tck(void) pureconst;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_CLKTCK_H_ */
