#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_TYPES_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_TYPES_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

typedef unsigned long dword;

struct qword {
  dword ax;
  dword dx;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_TYPES_H_ */
