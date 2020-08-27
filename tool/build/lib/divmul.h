#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_DIVMUL_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_DIVMUL_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void OpDivAlAhAxEbSigned(struct Machine *, uint32_t);
void OpDivAlAhAxEbUnsigned(struct Machine *, uint32_t);
void OpDivRdxRaxEvqpSigned(struct Machine *, uint32_t);
void OpDivRdxRaxEvqpUnsigned(struct Machine *, uint32_t);
void OpImulGvqpEvqp(struct Machine *, uint32_t);
void OpImulGvqpEvqpImm(struct Machine *, uint32_t);
void OpMulAxAlEbSigned(struct Machine *, uint32_t);
void OpMulAxAlEbUnsigned(struct Machine *, uint32_t);
void OpMulRdxRaxEvqpSigned(struct Machine *, uint32_t);
void OpMulRdxRaxEvqpUnsigned(struct Machine *, uint32_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_DIVMUL_H_ */
