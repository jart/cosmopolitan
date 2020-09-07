#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_SSEMOV_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_SSEMOV_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void OpLddquVdqMdq(struct Machine *, uint32_t);
void OpMovntiMdqpGdqp(struct Machine *, uint32_t);
void OpPmovmskbGdqpNqUdq(struct Machine *, uint32_t);
void OpMaskMovDiXmmRegXmmRm(struct Machine *, uint32_t);
void OpMovntdqaVdqMdq(struct Machine *, uint32_t);
void OpMovWpsVps(struct Machine *, uint32_t);
void OpMov0f28(struct Machine *, uint32_t);
void OpMov0f6e(struct Machine *, uint32_t);
void OpMov0f6f(struct Machine *, uint32_t);
void OpMov0fE7(struct Machine *, uint32_t);
void OpMov0f7e(struct Machine *, uint32_t);
void OpMov0f7f(struct Machine *, uint32_t);
void OpMov0f10(struct Machine *, uint32_t);
void OpMov0f29(struct Machine *, uint32_t);
void OpMov0f2b(struct Machine *, uint32_t);
void OpMov0f12(struct Machine *, uint32_t);
void OpMov0f13(struct Machine *, uint32_t);
void OpMov0f16(struct Machine *, uint32_t);
void OpMov0f17(struct Machine *, uint32_t);
void OpMov0fD6(struct Machine *, uint32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_SSEMOV_H_ */
