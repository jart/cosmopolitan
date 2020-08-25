#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_DIVMUL_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_DIVMUL_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void OpDivAlAhAxEbSigned(struct Machine *);
void OpDivAlAhAxEbUnsigned(struct Machine *);
void OpDivRdxRaxEvqpSigned(struct Machine *);
void OpDivRdxRaxEvqpUnsigned(struct Machine *);
void OpImulGvqpEvqp(struct Machine *);
void OpImulGvqpEvqpImm(struct Machine *);
void OpMulAxAlEbSigned(struct Machine *);
void OpMulAxAlEbUnsigned(struct Machine *);
void OpMulRdxRaxEvqpSigned(struct Machine *);
void OpMulRdxRaxEvqpUnsigned(struct Machine *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_DIVMUL_H_ */
