#ifndef COSMOPOLITAN_LIBC_RUNTIME_CXAATEXIT_H_
#define COSMOPOLITAN_LIBC_RUNTIME_CXAATEXIT_H_
#include "libc/stdio/stdio.h"
#include "libc/thread/tls.h"
COSMOPOLITAN_C_START_

struct CxaAtexitBlocks {
  struct CxaAtexitBlock {
    unsigned mask;
    struct CxaAtexitBlock *next;
    struct CxaAtexit {
      void *fp;
      void *arg;
      void *pred;
    } p[32];
  } *p, root;
};

extern struct CxaAtexitBlocks __cxa_blocks;

void __cxa_lock(void) libcesque;
void __cxa_unlock(void) libcesque;
void __cxa_thread_finalize(void) libcesque;
void __cxa_printexits(FILE *, void *) libcesque;
int __cxa_thread_atexit_impl(void *, void *, void *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_RUNTIME_CXAATEXIT_H_ */
