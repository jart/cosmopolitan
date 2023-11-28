#ifndef COSMOPOLITAN_LIBC_X_XSPAWN_H_
#define COSMOPOLITAN_LIBC_X_XSPAWN_H_
#include "libc/calls/struct/rusage.h"
COSMOPOLITAN_C_START_

int xspawn(struct rusage *);
int xvspawn(void (*)(void *), void *, struct rusage *) returnstwice;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_X_XSPAWN_H_ */
