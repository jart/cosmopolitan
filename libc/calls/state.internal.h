#ifndef COSMOPOLITAN_LIBC_CALLS_STATE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STATE_INTERNAL_H_
#include "libc/calls/calls.h"
#include "libc/nt/struct/securityattributes.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
COSMOPOLITAN_C_START_

extern int __vforked;
extern pthread_mutex_t __fds_lock_obj;
extern unsigned __sighandrvas[NSIG + 1];
extern unsigned __sighandflags[NSIG + 1];
extern uint64_t __sighandmask[NSIG + 1];
extern const struct NtSecurityAttributes kNtIsInheritable;

void __fds_wipe(void);
void __fds_lock(void);
void __fds_unlock(void);

#define __vforked (__tls_enabled && (__get_tls()->tib_flags & TIB_FLAG_VFORKED))

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STATE_INTERNAL_H_ */
