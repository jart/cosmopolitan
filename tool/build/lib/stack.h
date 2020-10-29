#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_STACK_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_STACK_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void Push(struct Machine *, uint32_t, uint64_t);
uint64_t Pop(struct Machine *, uint32_t, uint16_t);
void OpCallJvds(struct Machine *, uint32_t);
void OpRet(struct Machine *, uint32_t);
void OpRetf(struct Machine *, uint32_t);
void OpLeave(struct Machine *, uint32_t);
void OpCallEq(struct Machine *, uint32_t);
void OpPopEvq(struct Machine *, uint32_t);
void OpPopZvq(struct Machine *, uint32_t);
void OpPushZvq(struct Machine *, uint32_t);
void OpPushEvq(struct Machine *, uint32_t);
void PopVq(struct Machine *, uint32_t);
void PushVq(struct Machine *, uint32_t);
void OpJmpEq(struct Machine *, uint32_t);
void OpPusha(struct Machine *, uint32_t);
void OpPopa(struct Machine *, uint32_t);
void OpCallf(struct Machine *, uint32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_STACK_H_ */
