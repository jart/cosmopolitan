#ifndef COSMOPOLITAN_LIBC_INTRIN_WAIT0_H_
#define COSMOPOLITAN_LIBC_INTRIN_WAIT0_H_
#include "libc/atomic.h"
#include "libc/calls/struct/timespec.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

errno_t _wait0(const atomic_int *, struct timespec *) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_WAIT0_H_ */
