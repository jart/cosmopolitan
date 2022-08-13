#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGALTSTACK_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGALTSTACK_INTERNAL_H_
#include "libc/calls/struct/sigaltstack.h"
#include "libc/mem/alloca.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

const char *DescribeSigaltstk(char[128], int, const struct sigaltstack *);
#define DescribeSigaltstk(rc, ss) DescribeSigaltstk(alloca(128), rc, ss)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_SIGALTSTACK_INTERNAL_H_ */
