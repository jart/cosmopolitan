#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGVAL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGVAL_INTERNAL_H_
#include "libc/calls/struct/sigval.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int sys_sigqueue(int, int, const union sigval) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGVAL_INTERNAL_H_ */
