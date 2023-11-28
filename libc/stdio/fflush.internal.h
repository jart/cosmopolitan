#ifndef COSMOPOLITAN_LIBC_STDIO_FFLUSH_H_
#define COSMOPOLITAN_LIBC_STDIO_FFLUSH_H_
#include "libc/stdio/stdio.h"
#include "libc/thread/thread.h"
#include "libc/thread/tls.h"
COSMOPOLITAN_C_START_

struct StdioFlushHandles {
  size_t i, n;
  FILE **p;
};

struct StdioFlush {
  struct StdioFlushHandles handles;
  FILE *handles_initmem[8];
};

extern struct StdioFlush __fflush;
extern pthread_mutex_t __fflush_lock_obj;

void __fflush_lock(void);
void __fflush_unlock(void);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STDIO_FFLUSH_H_ */
