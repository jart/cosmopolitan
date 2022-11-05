#ifndef COSMOPOLITAN_LIBC_CALLS_BLOCKSIGS_H_
#define COSMOPOLITAN_LIBC_CALLS_BLOCKSIGS_H_
#include "libc/calls/struct/sigset.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define BLOCK_SIGNALS  \
  do {                 \
    sigset_t _SigMask; \
  _SigMask = _sigblockall()

#define ALLOW_SIGNALS    \
  _sigsetmask(_SigMask); \
  }                      \
  while (0)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_CALLS_BLOCKSIGS_H_ */
