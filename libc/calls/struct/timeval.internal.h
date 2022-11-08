#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_INTERNAL_H_
#include "libc/calls/struct/timeval.h"
#include "libc/time/struct/timezone.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

axdx_t sys_gettimeofday(struct timeval *, struct timezone *, void *) _Hide;
int sys_futimes(int, const struct timeval *) _Hide;
int sys_futimesat(int, const char *, const struct timeval *) _Hide;
int sys_lutimes(const char *, const struct timeval *) _Hide;
int sys_utimes(const char *, const struct timeval *) _Hide;
axdx_t sys_gettimeofday_xnu(struct timeval *, struct timezone *, void *) _Hide;
axdx_t sys_gettimeofday_nt(struct timeval *, struct timezone *, void *) _Hide;
int sys_utimes_nt(const char *, const struct timeval[2]) _Hide;
axdx_t sys_gettimeofday_metal(struct timeval *, struct timezone *, void *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_INTERNAL_H_ */
