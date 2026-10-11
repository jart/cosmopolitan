#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_ERROR_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_ERROR_H_
#include <wchar.h>

[[noreturn]] void Raise(int);
[[noreturn]] void Error(const wchar_t *, ...);
[[noreturn]] void OutOfMemory(void);
[[noreturn]] void StackOverflow(void);
[[noreturn]] void React(int, int, int);

#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_ERROR_H_ */
