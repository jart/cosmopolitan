#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_INTERNAL_H_
#include "libc/calls/struct/timespec.h"
#include "libc/mem/alloca.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
/* clang-format off */

int __sys_clock_nanosleep(int, int, const struct timespec *, struct timespec *) _Hide;
int __sys_utimensat(int, const char *, const struct timespec[2], int) _Hide;
int __utimens(int, const char *, const struct timespec[2], int) _Hide;
int sys_clock_getres(int, struct timespec *) _Hide;
int sys_clock_gettime(int, struct timespec *) _Hide;
int sys_clock_gettime_nt(int, struct timespec *) _Hide;
int sys_clock_gettime_xnu(int, struct timespec *) _Hide;
int sys_clock_nanosleep_nt(int, int, const struct timespec *, struct timespec *) _Hide;
int sys_clock_nanosleep_openbsd(int, int, const struct timespec *, struct timespec *) _Hide;
int sys_clock_nanosleep_xnu(int, int, const struct timespec *, struct timespec *) _Hide;
int sys_futimens(int, const struct timespec[2]) _Hide;
int sys_nanosleep(const struct timespec *, struct timespec *) _Hide;
int sys_nanosleep_nt(const struct timespec *, struct timespec *) _Hide;
int sys_nanosleep_xnu(const struct timespec *, struct timespec *) _Hide;
int sys_sem_timedwait(int64_t, const struct timespec *) _Hide;
int sys_utimensat(int, const char *, const struct timespec[2], int) _Hide;
int sys_utimensat_nt(int, const char *, const struct timespec[2], int) _Hide;
int sys_utimensat_xnu(int, const char *, const struct timespec[2], int) _Hide;

const char *DescribeTimespec(char[45], int, const struct timespec *);
#define DescribeTimespec(rc, ts) DescribeTimespec(alloca(45), rc, ts)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_INTERNAL_H_ */
