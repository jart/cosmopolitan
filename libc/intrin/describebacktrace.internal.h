#ifndef COSMOPOLITAN_LIBC_INTRIN_DESCRIBEBACKTRACE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_INTRIN_DESCRIBEBACKTRACE_INTERNAL_H_
#include "libc/mem/alloca.h"
#include "libc/nexgen32e/stackframe.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

const char *DescribeBacktrace(char[64], struct StackFrame *);
#define DescribeBacktrace(x) DescribeBacktrace(alloca(64), x)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_DESCRIBEBACKTRACE_INTERNAL_H_ */
