#ifndef COSMOPOLITAN_LIBC_X_XSIGACTION_H_
#define COSMOPOLITAN_LIBC_X_XSIGACTION_H_
#include "libc/calls/struct/sigaction.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int xsigaction(int, void *, uint64_t, uint64_t, struct sigaction *);

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define xsigaction(SIG, HANDLER, FLAGS, MASK, OLD) \
  ({                                               \
    __SIGACTION_YOINK(SIG);                        \
    xsigaction(SIG, HANDLER, FLAGS, MASK, OLD);    \
  })
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_X_XSIGACTION_H_ */
