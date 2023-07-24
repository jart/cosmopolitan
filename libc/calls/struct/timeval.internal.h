#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_INTERNAL_H_
#include "libc/calls/struct/timeval.h"
#include "libc/mem/alloca.h"
#include "libc/time/struct/timezone.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

axdx_t sys_gettimeofday(struct timeval *, struct timezone *, void *);
int sys_settimeofday(const struct timeval *, const struct timezone *);
int sys_futimes(int, const struct timeval *);
int sys_lutimes(const char *, const struct timeval *);
int sys_utimes(const char *, const struct timeval *);
axdx_t sys_gettimeofday_m1(struct timeval *, struct timezone *, void *);
axdx_t sys_gettimeofday_xnu(struct timeval *, struct timezone *, void *);
axdx_t sys_gettimeofday_nt(struct timeval *, struct timezone *, void *);
int sys_utimes_nt(const char *, const struct timeval[2]);
axdx_t sys_gettimeofday_metal(struct timeval *, struct timezone *, void *);

const char *DescribeTimeval(char[45], int, const struct timeval *);
#define DescribeTimeval(rc, ts) DescribeTimeval(alloca(45), rc, ts)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_INTERNAL_H_ */
