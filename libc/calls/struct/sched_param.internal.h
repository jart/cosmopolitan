#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SCHED_PARAM_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SCHED_PARAM_INTERNAL_H_
#include "libc/calls/struct/sched_param.h"
#include "libc/mem/alloca.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

const char *DescribeSchedParam(char[32], const struct sched_param *);
#define DescribeSchedParam(x) DescribeSchedParam(alloca(32), x)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SCHED_PARAM_INTERNAL_H_ */
