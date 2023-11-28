#ifndef COSMOPOLITAN_LIBC_CALLS_SIGTIMEDWAIT_H_
#define COSMOPOLITAN_LIBC_CALLS_SIGTIMEDWAIT_H_
#include "libc/calls/struct/siginfo.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
COSMOPOLITAN_C_START_

int sigwait(const sigset_t *, int *);
int sigtimedwait(const sigset_t *, siginfo_t *, const struct timespec *);
int sigwaitinfo(const sigset_t *, siginfo_t *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_SIGTIMEDWAIT_H_ */
