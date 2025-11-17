#ifndef COSMOPOLITAN_LIBC_COSMOTIME_H_
#define COSMOPOLITAN_LIBC_COSMOTIME_H_
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
COSMOPOLITAN_C_START_

/*
 * cosmopolitan timespec (nanoseconds)
 */

#define timespec_max  ((struct timespec){0x7fffffffffffffff, 999999999})
#define timespec_zero ((struct timespec){0})

#define timespec_add         __timespec_add
#define timespec_cmp         __timespec_cmp
#define timespec_frommicros  __timespec_frommicros
#define timespec_frommillis  __timespec_frommillis
#define timespec_fromnanos   __timespec_fromnanos
#define timespec_mono        __timespec_mono
#define timespec_real        __timespec_real
#define timespec_sleep       __timespec_sleep
#define timespec_sleep_until __timespec_sleep_until
#define timespec_sub         __timespec_sub
#define timespec_subz        __timespec_subz
#define timespec_tomicros    __timespec_tomicros
#define timespec_tomillis    __timespec_tomillis
#define timespec_tonanos     __timespec_tonanos
#define timespec_totimeval   __timespec_totimeval

struct timespec timespec_real(void) libcesque;
struct timespec timespec_mono(void) libcesque;
struct timespec timespec_sleep(int, struct timespec) libcesque;
int timespec_sleep_until(int, struct timespec) libcesque;

libcesque int timespec_cmp(struct timespec, struct timespec) pureconst;
libcesque int64_t timespec_tomicros(struct timespec) pureconst;
libcesque int64_t timespec_tomillis(struct timespec) pureconst;
libcesque int64_t timespec_tonanos(struct timespec) pureconst;
libcesque struct timespec timespec_add(struct timespec,
                                       struct timespec) pureconst;
libcesque struct timespec timespec_frommicros(int64_t) pureconst;
libcesque struct timespec timespec_frommillis(int64_t) pureconst;
libcesque struct timespec timespec_fromnanos(int64_t) pureconst;
libcesque struct timespec timespec_sub(struct timespec,
                                       struct timespec) pureconst;
libcesque struct timespec timespec_subz(struct timespec,
                                        struct timespec) pureconst;
libcesque struct timeval timespec_totimeval(struct timespec) pureconst;

static inline struct timespec timespec_fromseconds(int64_t __x) {
  return (struct timespec){__x};
}

static inline int timespec_iszero(struct timespec __ts) {
  return !(__ts.tv_sec | __ts.tv_nsec);
}

static inline int timespec_isvalid(struct timespec __ts) {
  return __ts.tv_sec >= 0 && __ts.tv_nsec + 0ull < 1000000000ull;
}

/*
 * cosmopolitan timeval (microseconds)
 */

#define timeval_max  ((struct timeval){0x7fffffffffffffff, 999999})
#define timeval_zero ((struct timeval){0})

#define timeval_add        __timeval_add
#define timeval_cmp        __timeval_cmp
#define timeval_frommicros __timeval_frommicros
#define timeval_frommillis __timeval_frommillis
#define timeval_real       __timeval_real
#define timeval_sub        __timeval_sub
#define timeval_subz       __timeval_subz
#define timeval_tomicros   __timeval_tomicros
#define timeval_tomillis   __timeval_tomillis
#define timeval_toseconds  __timeval_toseconds

struct timeval timeval_real(void) libcesque;

libcesque int timeval_cmp(struct timeval, struct timeval) pureconst;
libcesque int64_t timeval_tomicros(struct timeval) pureconst;
libcesque int64_t timeval_tomillis(struct timeval) pureconst;
libcesque int64_t timeval_toseconds(struct timeval) pureconst;
libcesque struct timeval timeval_add(struct timeval, struct timeval) pureconst;
libcesque struct timeval timeval_frommicros(int64_t) pureconst;
libcesque struct timeval timeval_frommillis(int64_t) pureconst;
libcesque struct timeval timeval_sub(struct timeval, struct timeval) pureconst;
libcesque struct timeval timeval_subz(struct timeval, struct timeval) pureconst;

static inline struct timeval timeval_fromseconds(int64_t __x) {
  return (struct timeval){__x};
}

static inline struct timespec timeval_totimespec(struct timeval __tv) {
  return (struct timespec){__tv.tv_sec, __tv.tv_usec * 1000};
}

static inline int timeval_iszero(struct timeval __tv) {
  return !(__tv.tv_sec | __tv.tv_usec);
}

static inline int timeval_isvalid(struct timeval __tv) {
  return __tv.tv_sec >= 0 && __tv.tv_usec + 0ull < 1000000ull;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_COSMOTIME_H_ */
