#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_STRING_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_STRING_H_
#include "tool/build/lib/machine.h"

#define STRING_CMPS 0
#define STRING_MOVS 1
#define STRING_STOS 2
#define STRING_LODS 3
#define STRING_SCAS 4
#define STRING_OUTS 5
#define STRING_INS  6

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void OpMovsb(struct Machine *, uint32_t);
void OpStosb(struct Machine *, uint32_t);
void OpMovs(struct Machine *, uint32_t);
void OpCmps(struct Machine *, uint32_t);
void OpStos(struct Machine *, uint32_t);
void OpLods(struct Machine *, uint32_t);
void OpScas(struct Machine *, uint32_t);
void OpIns(struct Machine *, uint32_t);
void OpOuts(struct Machine *, uint32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_STRING_H_ */
