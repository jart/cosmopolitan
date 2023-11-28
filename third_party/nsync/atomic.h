#ifndef NSYNC_ATOMIC_H_
#define NSYNC_ATOMIC_H_
#include "libc/atomic.h"
COSMOPOLITAN_C_START_

#define nsync_atomic_uint32_ atomic_uint

#define NSYNC_ATOMIC_UINT32_INIT_        0
#define NSYNC_ATOMIC_UINT32_LOAD_(p)     (*(p))
#define NSYNC_ATOMIC_UINT32_STORE_(p, v) (*(p) = (v))
#define NSYNC_ATOMIC_UINT32_PTR_(p)      (p)

COSMOPOLITAN_C_END_
#endif /* NSYNC_ATOMIC_H_ */
