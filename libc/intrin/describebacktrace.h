#ifndef COSMOPOLITAN_LIBC_INTRIN_DESCRIBEBACKTRACE_H_
#define COSMOPOLITAN_LIBC_INTRIN_DESCRIBEBACKTRACE_H_
#include "libc/mem/alloca.h"
#include "libc/nexgen32e/stackframe.h"
COSMOPOLITAN_C_START_

const char *_DescribeBacktrace(char[280], const struct StackFrame *) libcesque;
#define DescribeBacktrace(x) _DescribeBacktrace(alloca(280), x)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_DESCRIBEBACKTRACE_H_ */
