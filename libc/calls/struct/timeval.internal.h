#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_INTERNAL_H_
#include "libc/calls/struct/timeval.h"
#include "libc/mem/alloca.h"
#include "libc/time/struct/timezone.h"
COSMOPOLITAN_C_START_

int sys_settimeofday(const struct timeval *, const struct timezone *);
int sys_futimes(int, const struct timeval *);
int sys_lutimes(const char *, const struct timeval *);
int sys_utimes(const char *, const struct timeval *);
int sys_utimes_nt(const char *, const struct timeval[2]);

const char *DescribeTimeval(char[45], int, const struct timeval *);
#define DescribeTimeval(rc, ts) DescribeTimeval(alloca(45), rc, ts)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_INTERNAL_H_ */
