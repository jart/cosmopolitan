#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_PRINTF_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_PRINTF_H_
#include <stdarg.h>
#include <wchar.h>

int Printf(const wchar_t *, ...);
int Fprintf(int, const wchar_t *, ...);
int Fnprintf(int, int, const wchar_t *, ...);
int Vfnprintf(const wchar_t *, va_list, int, int);

#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_PRINTF_H_ */
