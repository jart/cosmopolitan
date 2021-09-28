#ifndef COSMOPOLITAN_LIBC_CALLS_SYSDEBUG_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_SYSDEBUG_INTERNAL_H_
#include "libc/log/libfatal.internal.h"

#ifndef DEBUGSYS
#define DEBUGSYS 0
#endif

#if DEBUGSYS
#define SYSDEBUG(FMT, ...) __printf("SYS: " FMT "\n", ##__VA_ARGS__)
#else
#define SYSDEBUG(FMT, ...) (void)0
#endif

#endif /* COSMOPOLITAN_LIBC_CALLS_SYSDEBUG_INTERNAL_H_ */
