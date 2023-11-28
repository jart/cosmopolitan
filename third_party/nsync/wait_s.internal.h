#ifndef COSMOPOLITAN_LIBC_THREAD_WAIT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_THREAD_WAIT_INTERNAL_H_
#include "libc/intrin/dll.h"
#include "third_party/nsync/atomic.h"
COSMOPOLITAN_C_START_

/* Implementations of "struct nsync_waitable_s" must provide functions
   in struct nsync_waitable_funcs_s (see public/nsync_wait.h). When
   nsync_wait_n() waits on a client's object, those functions are called
   with v pointing to the client's object and nw pointing to a struct
   nsync_waiter_s. */
struct nsync_waiter_s {
  uint32_t tag;                   /* used for debugging */
  uint32_t flags;                 /* see below */
  struct Dll q;                   /* used to link children of parent */
  nsync_atomic_uint32_ waiting;   /* non-zero <=> the waiter is waiting */
  struct nsync_semaphore_s_ *sem; /* *sem will be Ved when waiter is woken */
};

/* set if waiter is embedded in Mu/CV's internal structures */
#define NSYNC_WAITER_FLAG_MUCV 0x1

void nsync_waiter_destroy(void *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_THREAD_WAIT_INTERNAL_H_ */
