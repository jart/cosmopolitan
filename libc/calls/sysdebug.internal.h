#ifndef COSMOPOLITAN_LIBC_CALLS_SYSDEBUG_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SYSDEBUG_INTERNAL_H_
#include "libc/calls/calls.h"

#if 0
#define SYSDEBUG(FMT, ...) (dprintf)(2, FMT "\n", ##__VA_ARGS__)
#else
#define SYSDEBUG(FMT, ...) (void)0
#endif

#endif /* COSMOPOLITAN_LIBC_CALLS_SYSDEBUG_INTERNAL_H_ */
