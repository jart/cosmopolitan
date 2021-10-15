#ifndef COSMOPOLITAN_LIBC_CALLS_MATH_H_
#define COSMOPOLITAN_LIBC_CALLS_MATH_H_
#include "libc/calls/struct/rusage.h"
#include "libc/calls/struct/timespec.h"
#include "libc/calls/struct/timeval.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct timeval AddTimeval(struct timeval, struct timeval);
struct timespec AddTimespec(struct timespec, struct timespec);
void AddRusage(struct rusage *, const struct rusage *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_MATH_H_ */
