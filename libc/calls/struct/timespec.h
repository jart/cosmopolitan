#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define timespec_zero ((struct timespec){0})
#define timespec_max  ((struct timespec){0x7fffffffffffffff, 999999999})

struct timespec {
  int64_t tv_sec;
  int64_t tv_nsec; /* nanoseconds */
};

int clock_getres(int, struct timespec *);
int clock_gettime(int, struct timespec *);
int clock_nanosleep(int, int, const struct timespec *, struct timespec *);
int futimens(int, const struct timespec[2]);
int nanosleep(const struct timespec *, struct timespec *);
int sys_futex(int *, int, int, const struct timespec *, int *);
int utimensat(int, const char *, const struct timespec[2], int);
int timespec_get(struct timespec *, int);
int timespec_getres(struct timespec *, int);

int timespec_cmp(struct timespec, struct timespec) pureconst;
int64_t timespec_tomicros(struct timespec) pureconst;
int64_t timespec_tomillis(struct timespec) pureconst;
int64_t timespec_tonanos(struct timespec) pureconst;
struct timespec timespec_add(struct timespec, struct timespec) pureconst;
struct timespec timespec_fromnanos(int64_t) pureconst;
struct timespec timespec_frommicros(int64_t) pureconst;
struct timespec timespec_frommillis(int64_t) pureconst;
struct timespec timespec_real(void);
struct timespec timespec_mono(void);
struct timespec timespec_sleep(struct timespec);
int timespec_sleep_until(struct timespec);
struct timespec timespec_sub(struct timespec, struct timespec) pureconst;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_ */
