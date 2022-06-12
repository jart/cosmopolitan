#ifndef COSMOPOLITAN_LIBC_RUNTIME_CXAATEXIT_H_
#define COSMOPOLITAN_LIBC_RUNTIME_CXAATEXIT_H_
#include "libc/intrin/pthread.h"
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

extern pthread_mutex_t __cxa_lock_obj;
extern struct CxaAtexitBlocks __cxa_blocks;

void __cxa_printexits(FILE *, void *);

#define __cxa_lock()   pthread_mutex_lock(&__cxa_lock_obj)
#define __cxa_unlock() pthread_mutex_unlock(&__cxa_lock_obj)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_CXAATEXIT_H_ */
