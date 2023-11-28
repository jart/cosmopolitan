#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_INTERNAL_H_
#include "libc/calls/struct/timespec.h"
#include "libc/mem/alloca.h"
COSMOPOLITAN_C_START_
/* clang-format off */

int __sys_clock_nanosleep(int, int, const struct timespec *, struct timespec *);
int __sys_utimensat(int, const char *, const struct timespec[2], int);
int __utimens(int, const char *, const struct timespec[2], int);
int sys_clock_getres(int, struct timespec *);
int sys_clock_gettime(int, struct timespec *);
int sys_clock_gettime_m1(int, struct timespec *);
int sys_clock_gettime_mono(struct timespec *);
int sys_clock_gettime_nt(int, struct timespec *);
int sys_clock_gettime_xnu(int, struct timespec *);
int sys_clock_nanosleep_nt(int, int, const struct timespec *, struct timespec *);
int sys_clock_nanosleep_openbsd(int, int, const struct timespec *, struct timespec *);
int sys_clock_nanosleep_xnu(int, int, const struct timespec *, struct timespec *);
int sys_clock_settime(int, const struct timespec *);
int sys_futimens(int, const struct timespec[2]);
int sys_nanosleep(const struct timespec *, struct timespec *);
int sys_nanosleep_nt(const struct timespec *, struct timespec *);
int sys_sem_timedwait(int64_t, const struct timespec *);
int sys_utimensat(int, const char *, const struct timespec[2], int);
int sys_utimensat_nt(int, const char *, const struct timespec[2], int);
int sys_utimensat_old(int, const char *, const struct timespec[2], int);

const char *DescribeTimespec(char[45], int, const struct timespec *);
#define DescribeTimespec(rc, ts) DescribeTimespec(alloca(45), rc, ts)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_INTERNAL_H_ */
