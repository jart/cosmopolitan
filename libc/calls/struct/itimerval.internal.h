#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_ITIMERVAL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_ITIMERVAL_INTERNAL_H_
#include "libc/calls/struct/itimerval.h"
#include "libc/mem/alloca.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int sys_getitimer(int, struct itimerval *) _Hide;
int sys_setitimer(int, const struct itimerval *, struct itimerval *) _Hide;
int sys_setitimer_nt(int, const struct itimerval *, struct itimerval *) _Hide;

const char *DescribeTimeval(char[45], int, const struct timeval *);
#define DescribeTimeval(rc, ts) DescribeTimeval(alloca(45), rc, ts)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_ITIMERVAL_INTERNAL_H_ */
