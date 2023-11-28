#ifndef COSMOPOLITAN_LIBC_ITIMER_H_
#define COSMOPOLITAN_LIBC_ITIMER_H_
#include "libc/atomic.h"
#include "libc/calls/struct/itimerval.h"
#include "third_party/nsync/cv.h"
#include "third_party/nsync/mu.h"
COSMOPOLITAN_C_START_

struct IntervalTimer {
  atomic_uint once;
  intptr_t thread;
  nsync_mu lock;
  nsync_cv cond;
  struct itimerval it;
};

extern struct IntervalTimer __itimer;

void __itimer_wipe(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_ITIMER_H_ */
