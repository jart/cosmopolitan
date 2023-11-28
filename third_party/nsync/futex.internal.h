#ifndef NSYNC_FUTEX_INTERNAL_H_
#define NSYNC_FUTEX_INTERNAL_H_
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
COSMOPOLITAN_C_START_

int nsync_futex_wake_(_Atomic(int) *, int, char);
int nsync_futex_wait_(_Atomic(int) *, int, char, const struct timespec *);

COSMOPOLITAN_C_END_
#endif /* NSYNC_FUTEX_INTERNAL_H_ */
