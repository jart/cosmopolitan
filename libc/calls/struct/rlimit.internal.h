#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_RLIMIT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_RLIMIT_INTERNAL_H_
#include "libc/calls/struct/rlimit.h"
#include "libc/mem/alloca.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int sys_getrlimit(int, struct rlimit *) _Hide;
int sys_setrlimit(int, const struct rlimit *) _Hide;
int sys_setrlimit_nt(int, const struct rlimit *) _Hide;

const char *DescribeRlimit(char[64], int, const struct rlimit *);
#define DescribeRlimit(rc, rl) DescribeRlimit(alloca(64), rc, rl)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_RLIMIT_INTERNAL_H_ */
