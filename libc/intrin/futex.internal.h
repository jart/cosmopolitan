#ifndef COSMOPOLITAN_LIBC_INTRIN_FUTEX_INTERNAL_H_
#define COSMOPOLITAN_LIBC_INTRIN_FUTEX_INTERNAL_H_
#include "libc/calls/struct/timespec.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int _futex(void *, int, int, struct timespec *, int *) hidden;
int _futex_wait(void *, int, struct timespec *) hidden;
int _futex_wake(void *, int) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_FUTEX_INTERNAL_H_ */
