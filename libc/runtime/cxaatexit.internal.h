#ifndef COSMOPOLITAN_LIBC_RUNTIME_CXAATEXIT_H_
#define COSMOPOLITAN_LIBC_RUNTIME_CXAATEXIT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
#include "libc/stdio/stdio.h"

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

void __cxa_printexits(FILE *, void *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_CXAATEXIT_H_ */
