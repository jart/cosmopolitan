#ifndef COSMOPOLITAN_LIBC_CALLS_BLOCKCANCEL_H_
#define COSMOPOLITAN_LIBC_CALLS_BLOCKCANCEL_H_
#include "libc/thread/thread.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define BLOCK_CANCELLATIONS \
  do {                      \
    int _CancelState;       \
  _CancelState = _pthread_block_cancellations()

#define ALLOW_CANCELLATIONS                   \
  _pthread_allow_cancellations(_CancelState); \
  }                                           \
  while (0)

int _pthread_block_cancellations(void);
void _pthread_allow_cancellations(int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_BLOCKCANCEL_H_ */
