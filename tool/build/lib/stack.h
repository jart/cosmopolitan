#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_STACK_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_STACK_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void Push64(struct Machine *, uint64_t);
uint64_t Pop64(struct Machine *, uint16_t);
void Push16(struct Machine *, uint16_t);
uint16_t Pop16(struct Machine *, uint16_t);
void OpCallJvds(struct Machine *);
void OpRet(struct Machine *, uint16_t);
void OpLeave(struct Machine *);
void PushOsz(struct Machine *, uint64_t);
void OpCallEq(struct Machine *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_STACK_H_ */
