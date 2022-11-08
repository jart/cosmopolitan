#ifndef COSMOPOLITAN_LIBC_CALLS_SIGTIMEDWAIT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SIGTIMEDWAIT_INTERNAL_H_
#include "libc/calls/struct/siginfo-meta.internal.h"
#include "libc/calls/struct/sigset.h"
#include "libc/calls/struct/timespec.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int sys_sigtimedwait(const sigset_t *, union siginfo_meta *,
                     const struct timespec *, size_t) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_SIGTIMEDWAIT_INTERNAL_H_ */
