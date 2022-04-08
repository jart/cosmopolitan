#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_ERROR_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_ERROR_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void Raise(int) relegated wontreturn;
void Error(const char *, ...) relegated wontreturn;
void OutOfMemory(void) relegated wontreturn;
void StackOverflow(void) relegated wontreturn;
void React(int, int, int) relegated wontreturn;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_ERROR_H_ */
