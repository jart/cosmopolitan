#ifndef COSMOPOLITAN_LIBC_CALLS_STATE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STATE_INTERNAL_H_
#include "libc/assert.h"
#include "libc/calls/calls.h"
#include "libc/nt/struct/securityattributes.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
COSMOPOLITAN_C_START_

extern pthread_mutex_t __fds_lock_obj;
extern const struct NtSecurityAttributes kNtIsInheritable;

void __fds_lock(void);
void __fds_unlock(void);

/* this helps us return ENOTSUP, when users try to open zipos files from
   a vfork() child. it also helps system calls not clobber static memory */
#define __vforked (__tls_enabled && (__get_tls()->tib_flags & TIB_FLAG_VFORKED))

/* our posix_spawn() implementation uses sys_fork() instead of fork()
   which means certain libraries like mutexes are very unsafe to use! */
#define __spawned (__tls_enabled && (__get_tls()->tib_flags & TIB_FLAG_SPAWNED))
#ifdef MODE_DBG
#define FORBIDDEN_IN_POSIX_SPAWN unassert(!__spawned)
#else
#define FORBIDDEN_IN_POSIX_SPAWN (void)0
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STATE_INTERNAL_H_ */
