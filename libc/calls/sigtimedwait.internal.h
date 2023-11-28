#ifndef COSMOPOLITAN_LIBC_CALLS_SIGTIMEDWAIT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SIGTIMEDWAIT_INTERNAL_H_
#include "libc/calls/struct/siginfo-meta.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
COSMOPOLITAN_C_START_

int sys_sigtimedwait(const sigset_t *, union siginfo_meta *,
                     const struct timespec *, size_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_SIGTIMEDWAIT_INTERNAL_H_ */
