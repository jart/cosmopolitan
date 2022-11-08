#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_INTERNAL_H_
#include "libc/calls/struct/siginfo.h"
#include "libc/mem/alloca.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int sys_sigqueueinfo(int, const siginfo_t *) _Hide;

const char *DescribeSiginfo(char[300], int, const siginfo_t *);
#define DescribeSiginfo(x, y) DescribeSiginfo(alloca(300), x, y)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGINFO_INTERNAL_H_ */
