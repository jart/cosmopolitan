#ifndef COSMOPOLITAN_LIBC_RUNTIME_CXAATEXIT_H_
#define COSMOPOLITAN_LIBC_RUNTIME_CXAATEXIT_H_
#include "libc/intrin/nopl.internal.h"
#include "libc/thread/tls.h"
#include "libc/stdio/stdio.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct CxaAtexitBlocks {
  struct CxaAtexitBlock {
    unsigned mask;
    struct CxaAtexitBlock *next;
    struct CxaAtexit {
      void *fp;
      void *arg;
      void *pred;
    } p[ATEXIT_MAX];
  } * p, root;
};

extern struct CxaAtexitBlocks __cxa_blocks;

void __cxa_lock(void);
void __cxa_unlock(void);
void __cxa_printexits(FILE *, void *);

#ifdef _NOPL0
#define __cxa_lock()   _NOPL0("__threadcalls", __cxa_lock)
#define __cxa_unlock() _NOPL0("__threadcalls", __cxa_unlock)
#else
#define __cxa_lock()   (__threaded ? __cxa_lock() : 0)
#define __cxa_unlock() (__threaded ? __cxa_unlock() : 0)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_CXAATEXIT_H_ */
