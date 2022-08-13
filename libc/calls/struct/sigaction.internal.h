#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_INTERNAL_H_
#include "libc/calls/struct/sigaction.h"
#include "libc/mem/alloca.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

const char *DescribeSigaction(char[256], int, const struct sigaction *);
#define DescribeSigaction(rc, sa) DescribeSigaction(alloca(256), rc, sa)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGACTION_INTERNAL_H_ */
