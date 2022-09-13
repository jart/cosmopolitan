#ifndef COSMOPOLITAN_LIBC_INTRIN_WAIT0_H_
#define COSMOPOLITAN_LIBC_INTRIN_WAIT0_H_
#include "libc/atomic.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void _wait0(const atomic_int *) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_WAIT0_H_ */
