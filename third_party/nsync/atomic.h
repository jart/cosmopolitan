#ifndef NSYNC_ATOMIC_H_
#define NSYNC_ATOMIC_H_
#include "libc/atomic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

typedef atomic_uint_fast32_t nsync_atomic_uint32_;

#define NSYNC_ATOMIC_UINT32_INIT_        0
#define NSYNC_ATOMIC_UINT32_LOAD_(p)     (*(p))
#define NSYNC_ATOMIC_UINT32_STORE_(p, v) (*(p) = (v))
#define NSYNC_ATOMIC_UINT32_PTR_(p)      (p)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* NSYNC_ATOMIC_H_ */
