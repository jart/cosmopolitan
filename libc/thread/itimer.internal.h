#ifndef COSMOPOLITAN_LIBC_ITIMER_H_
#define COSMOPOLITAN_LIBC_ITIMER_H_
#include "libc/atomic.h"
#include "libc/calls/struct/itimerval.h"
#include "third_party/nsync/cv.h"
#include "third_party/nsync/mu.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct IntervalTimer {
  atomic_uint once;
  intptr_t thread;
  nsync_mu lock;
  nsync_cv cond;
  struct itimerval it;
};

extern struct IntervalTimer __itimer;

void __itimer_reset(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ITIMER_H_ */
