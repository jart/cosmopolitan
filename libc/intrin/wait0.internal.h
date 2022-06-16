#ifndef COSMOPOLITAN_LIBC_INTRIN_WAIT0_H_
#define COSMOPOLITAN_LIBC_INTRIN_WAIT0_H_
#include "libc/bits/atomic.h"
#include "libc/calls/calls.h"
#include "libc/dce.h"
#include "libc/linux/futex.h"

#define _wait0(ptid)                                               \
  do {                                                             \
    int x;                                                         \
    if (!(x = atomic_load_explicit(ptid, memory_order_relaxed))) { \
      break;                                                       \
    } else if (IsLinux()) {                                        \
      LinuxFutexWait(ptid, x, 0);                                  \
    } else {                                                       \
      sched_yield();                                               \
    }                                                              \
  } while (1)

#endif /* COSMOPOLITAN_LIBC_INTRIN_WAIT0_H_ */
