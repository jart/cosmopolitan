#ifndef NSYNC_FUTEX_INTERNAL_H_
#define NSYNC_FUTEX_INTERNAL_H_
#include "libc/calls/struct/timespec.h"
#include "libc/dce.h"
COSMOPOLITAN_C_START_

#ifndef __cplusplus
#define _FUTEX_ATOMIC(x) _Atomic(x)
#else
#define _FUTEX_ATOMIC(x) x
#endif

int nsync_futex_wake_(_FUTEX_ATOMIC(int) *, int, char);
int nsync_futex_wait_(_FUTEX_ATOMIC(int) *, int, char, int, const struct timespec *);

COSMOPOLITAN_C_END_
#endif /* NSYNC_FUTEX_INTERNAL_H_ */
