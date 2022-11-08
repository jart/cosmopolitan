#ifndef COSMOPOLITAN_LIBC_CALLS_WAIT4_H_
#define COSMOPOLITAN_LIBC_CALLS_WAIT4_H_
#include "libc/calls/struct/rusage.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int sys_wait4_nt(int, int *, int, struct rusage *) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_WAIT4_H_ */
