#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_FLOCK_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_FLOCK_INTERNAL_H_
#include "libc/calls/struct/flock.h"
#include "libc/mem/alloca.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

const char *DescribeFlock(char[300], int, const struct flock *);
#define DescribeFlock(c, l) DescribeFlock(alloca(300), c, l)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_FLOCK_INTERNAL_H_ */
