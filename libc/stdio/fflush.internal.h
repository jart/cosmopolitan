#ifndef COSMOPOLITAN_LIBC_STDIO_FFLUSH_H_
#define COSMOPOLITAN_LIBC_STDIO_FFLUSH_H_
#include "libc/intrin/nopl.internal.h"
#include "libc/stdio/stdio.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct StdioFlushHandles {
  size_t i, n;
  FILE **p;
};

struct StdioFlush {
  struct StdioFlushHandles handles;
  FILE *handles_initmem[8];
};

_Hide extern struct StdioFlush __fflush;
_Hide extern pthread_mutex_t __fflush_lock_obj;

void __fflush_lock(void);
void __fflush_unlock(void);

#ifdef _NOPL0
#define __fflush_lock()   _NOPL0("__threadcalls", __fflush_lock)
#define __fflush_unlock() _NOPL0("__threadcalls", __fflush_unlock)
#else
#define __fflush_lock()   (__threaded ? __fflush_lock() : 0)
#define __fflush_unlock() (__threaded ? __fflush_unlock() : 0)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STDIO_FFLUSH_H_ */
