#ifndef COSMOPOLITAN_LIBC_RUNTIME_BRK_INTERNAL_H_
#define COSMOPOLITAN_LIBC_RUNTIME_BRK_INTERNAL_H_
#include "libc/thread/thread.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Brk {
  unsigned char *p;
  pthread_mutex_t m;
};

extern struct Brk __brk;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_BRK_INTERNAL_H_ */
