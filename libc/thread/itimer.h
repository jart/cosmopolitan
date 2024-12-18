#ifndef COSMOPOLITAN_LIBC_ITIMER_H_
#define COSMOPOLITAN_LIBC_ITIMER_H_
#include "libc/atomic.h"
#include "libc/calls/struct/itimerval.h"
#include "libc/thread/thread.h"
COSMOPOLITAN_C_START_

struct IntervalTimer {
  atomic_uint once;
  intptr_t thread;
  pthread_mutex_t lock;
  pthread_cond_t cond;
  struct itimerval it;
};

extern struct IntervalTimer __itimer;

void __itimer_lock(void);
void __itimer_unlock(void);
void __itimer_wipe_and_reset(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_ITIMER_H_ */
