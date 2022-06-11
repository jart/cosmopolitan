#ifndef COSMOPOLITAN_LIBC_CALLS_STATE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STATE_INTERNAL_H_
#include "libc/intrin/pthread.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

hidden extern int __vforked;
hidden extern bool __time_critical;
hidden extern unsigned __sighandrvas[NSIG];
hidden extern unsigned __sighandflags[NSIG];
hidden extern pthread_mutex_t __sig_lock_obj;
hidden extern pthread_mutex_t __fds_lock_obj;
hidden extern const struct NtSecurityAttributes kNtIsInheritable;

#define __fds_lock()   pthread_mutex_lock(&__fds_lock_obj)
#define __fds_unlock() pthread_mutex_unlock(&__fds_lock_obj)
#define __sig_lock()   pthread_mutex_lock(&__sig_lock_obj)
#define __sig_unlock() pthread_mutex_unlock(&__sig_lock_obj)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STATE_INTERNAL_H_ */
