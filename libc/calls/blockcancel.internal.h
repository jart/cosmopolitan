#ifndef COSMOPOLITAN_LIBC_CALLS_BLOCKCANCEL_H_
#define COSMOPOLITAN_LIBC_CALLS_BLOCKCANCEL_H_
#include "libc/thread/thread.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define BLOCK_CANCELLATIONS \
  do {                      \
    int _CancelState;       \
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &_CancelState)

#define ALLOW_CANCELLATIONS                \
  pthread_setcancelstate(_CancelState, 0); \
  }                                        \
  while (0)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_BLOCKCANCEL_H_ */
