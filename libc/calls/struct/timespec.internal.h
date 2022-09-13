#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_INTERNAL_H_
#include "libc/calls/struct/timespec.h"
#include "libc/mem/alloca.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int __sys_utimensat(int, const char *, const struct timespec[2], int) hidden;
int sys_clock_getres(int, struct timespec *) hidden;
int sys_clock_gettime(int, struct timespec *) hidden;
int sys_clock_gettime_nt(int, struct timespec *) hidden;
int sys_clock_gettime_xnu(int, struct timespec *) hidden;
int sys_futimens(int, const struct timespec[2]) hidden;
int sys_nanosleep(const struct timespec *, struct timespec *) hidden;
int sys_nanosleep_nt(const struct timespec *, struct timespec *) hidden;
int sys_nanosleep_xnu(const struct timespec *, struct timespec *) hidden;
int sys_utimensat(int, const char *, const struct timespec[2], int) hidden;
int sys_utimensat_nt(int, const char *, const struct timespec[2], int) hidden;
int sys_utimensat_xnu(int, const char *, const struct timespec[2], int) hidden;

const char *DescribeTimespec(char[45], int, const struct timespec *);
#define DescribeTimespec(rc, ts) DescribeTimespec(alloca(45), rc, ts)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_INTERNAL_H_ */
