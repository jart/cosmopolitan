#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_FLOCK_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_FLOCK_INTERNAL_H_
#include "libc/calls/struct/flock.h"
#include "libc/mem/alloca.h"
COSMOPOLITAN_C_START_

const char *DescribeFlock(char[300], int, const struct flock *);
#define DescribeFlock(c, l) DescribeFlock(alloca(300), c, l)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_FLOCK_INTERNAL_H_ */
