#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_H_
#include "libc/calls/struct/timespec.h"
#include "libc/time/struct/timezone.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct timeval {
  int64_t tv_sec;
  int64_t tv_usec; /* microseconds */
};

int futimes(int, const struct timeval[2]);
int futimesat(int, const char *, const struct timeval[2]);
int gettimeofday(struct timeval *, struct timezone *);
int lutimes(const char *, const struct timeval[2]);
int utimes(const char *, const struct timeval[2]);

int timeval_cmp(struct timeval, struct timeval) pureconst;
struct timeval timeval_frommicros(int64_t) pureconst;
struct timeval timeval_frommillis(int64_t) pureconst;
struct timeval timeval_add(struct timeval, struct timeval) pureconst;
struct timeval timeval_sub(struct timeval, struct timeval) pureconst;
struct timeval timespec_totimeval(struct timespec) pureconst;
struct timespec timeval_totimespec(struct timeval) pureconst;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_TIMEVAL_H_ */
