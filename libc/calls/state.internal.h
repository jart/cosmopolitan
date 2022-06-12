#ifndef COSMOPOLITAN_LIBC_CALLS_STATE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STATE_INTERNAL_H_
#include "libc/nexgen32e/threaded.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern int __vforked;
hidden extern bool __time_critical;
hidden extern unsigned __sighandrvas[NSIG];
hidden extern unsigned __sighandflags[NSIG];
hidden extern const struct NtSecurityAttributes kNtIsInheritable;

void __fds_lock(void);
void __fds_unlock(void);
void __sig_lock(void);
void __sig_unlock(void);

#define __fds_lock()   (__threaded ? __fds_lock() : 0)
#define __fds_unlock() (__threaded ? __fds_unlock() : 0)
#define __sig_lock()   (__threaded ? __sig_lock() : 0)
#define __sig_unlock() (__threaded ? __sig_unlock() : 0)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STATE_INTERNAL_H_ */
