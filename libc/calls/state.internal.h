#ifndef COSMOPOLITAN_LIBC_CALLS_STATE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STATE_INTERNAL_H_
#include "libc/intrin/spinlock.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern int __vforked;
hidden extern int __fds_lock_obj;
hidden extern int __sig_lock_obj;
hidden extern bool __time_critical;
hidden extern unsigned __sighandrvas[NSIG];
hidden extern unsigned __sighandflags[NSIG];
hidden extern const struct NtSecurityAttributes kNtIsInheritable;

void __fds_lock(void);
#define __fds_unlock() _spunlock(&__fds_lock_obj)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STATE_INTERNAL_H_ */
