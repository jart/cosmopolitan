#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_PRINTF_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_PRINTF_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int Printf(const char *, ...);
int Fprintf(int, const char *, ...);
int Fnprintf(int, int, const char *, ...);
int Vfprintf(const char *, va_list, int);
int Vfnprintf(const char *, va_list, int, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_PRINTF_H_ */
