#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_

#ifdef COSMO
#define timespec_get         __timespec_get
#define timespec_getres      __timespec_getres
#define timespec_cmp         __timespec_cmp
#define timespec_tomicros    __timespec_tomicros
#define timespec_tomillis    __timespec_tomillis
#define timespec_tonanos     __timespec_tonanos
#define timespec_add         __timespec_add
#define timespec_fromnanos   __timespec_fromnanos
#define timespec_frommicros  __timespec_frommicros
#define timespec_frommillis  __timespec_frommillis
#define timespec_real        __timespec_real
#define timespec_mono        __timespec_mono
#define timespec_sleep       __timespec_sleep
#define timespec_sleep_until __timespec_sleep_until
#define timespec_sub         __timespec_sub
#endif /* COSMO */

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

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

#ifdef COSMO
/* cosmopolitan libc's non-posix timespec library
   removed by default due to emacs codebase clash */
#define timespec_zero ((struct timespec){0})
#define timespec_max  ((struct timespec){0x7fffffffffffffff, 999999999})
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
#endif /* COSMO */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMESPEC_H_ */
