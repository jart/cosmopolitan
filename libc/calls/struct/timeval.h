#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_H_
#include "libc/calls/struct/timespec.h"
#include "libc/time/struct/timezone.h"
COSMOPOLITAN_C_START_

struct timeval {
  int64_t tv_sec;
  int64_t tv_usec; /* microseconds */
};

int futimes(int, const struct timeval[2]);
int futimesat(int, const char *, const struct timeval[2]);
int gettimeofday(struct timeval *, struct timezone *);
int settimeofday(const struct timeval *, const struct timezone *);
int lutimes(const char *, const struct timeval[2]);
int utimes(const char *, const struct timeval[2]);

#ifdef _COSMO_SOURCE
/* cosmopolitan libc's non-posix timevals library
   removed by default due to emacs codebase clash */
#define timeval_zero ((struct timeval){0})
#define timeval_max  ((struct timeval){0x7fffffffffffffff, 999999})
int timeval_cmp(struct timeval, struct timeval) pureconst;
struct timeval timeval_real(void);
struct timeval timeval_frommicros(int64_t) pureconst;
struct timeval timeval_frommillis(int64_t) pureconst;
struct timeval timeval_add(struct timeval, struct timeval) pureconst;
struct timeval timeval_sub(struct timeval, struct timeval) pureconst;
struct timeval timeval_subz(struct timeval, struct timeval) pureconst;
int64_t timeval_toseconds(struct timeval);
int64_t timeval_tomicros(struct timeval);
int64_t timeval_tomillis(struct timeval);
struct timeval timespec_totimeval(struct timespec) pureconst;
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
#endif /* _COSMO_SOURCE */

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define timerisset(t) ((t)->tv_sec || (t)->tv_usec)
#define timerclear(t) ((t)->tv_sec = (t)->tv_usec = 0)
#define timercmp(s, t, op)                                  \
  ((s)->tv_sec == (t)->tv_sec ? (s)->tv_usec op(t)->tv_usec \
                              : (s)->tv_sec op(t)->tv_sec)
#define timeradd(s, t, a)                                           \
  (void)((a)->tv_sec = (s)->tv_sec + (t)->tv_sec,                   \
         ((a)->tv_usec = (s)->tv_usec + (t)->tv_usec) >= 1000000 && \
             ((a)->tv_usec -= 1000000, (a)->tv_sec++))
#define timersub(s, t, a)                                    \
  (void)((a)->tv_sec = (s)->tv_sec - (t)->tv_sec,            \
         ((a)->tv_usec = (s)->tv_usec - (t)->tv_usec) < 0 && \
             ((a)->tv_usec += 1000000, (a)->tv_sec--))
#endif

#ifdef _GNU_SOURCE
#define TIMEVAL_TO_TIMESPEC(tv, ts) \
  ((ts)->tv_sec = (tv)->tv_sec, (ts)->tv_nsec = (tv)->tv_usec * 1000, (void)0)
#define TIMESPEC_TO_TIMEVAL(tv, ts) \
  ((tv)->tv_sec = (ts)->tv_sec, (tv)->tv_usec = (ts)->tv_nsec / 1000, (void)0)
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_H_ */
