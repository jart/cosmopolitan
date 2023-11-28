#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_ITIMERVAL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_ITIMERVAL_INTERNAL_H_
#include "libc/calls/struct/itimerval.h"
#include "libc/mem/alloca.h"
COSMOPOLITAN_C_START_

int sys_getitimer(int, struct itimerval *);
int sys_setitimer(int, const struct itimerval *, struct itimerval *);
int sys_setitimer_nt(int, const struct itimerval *, struct itimerval *);

const char *DescribeItimerval(char[90], int, const struct itimerval *);
#define DescribeItimerval(rc, ts) DescribeItimerval(alloca(90), rc, ts)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_ITIMERVAL_INTERNAL_H_ */
