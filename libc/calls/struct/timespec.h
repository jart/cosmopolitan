#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct timespec {
  int64_t tv_sec;
  int64_t tv_nsec; /* nanoseconds */
};

int clock_getres(int, struct timespec *) libcesque;
int clock_gettime(int, struct timespec *) libcesque;
int clock_settime(int, const struct timespec *) libcesque;
int clock_nanosleep(int, int, const struct timespec *, struct timespec *);
int futimens(int, const struct timespec[2]) libcesque;
int nanosleep(const struct timespec *, struct timespec *) libcesque;
int utimensat(int, const char *, const struct timespec[2], int) libcesque;
int timespec_getres(struct timespec *, int) libcesque;
int timespec_get(struct timespec *, int) libcesque;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_ */
