#ifndef COSMOPOLITAN_LIBC_X_XSIGACTION_H_
#define COSMOPOLITAN_LIBC_X_XSIGACTION_H_
#include "libc/calls/struct/sigaction.h"
COSMOPOLITAN_C_START_

int xsigaction(int, void *, uint64_t, uint64_t, struct sigaction *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_X_XSIGACTION_H_ */
