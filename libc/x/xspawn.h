#ifndef COSMOPOLITAN_LIBC_X_XSPAWN_H_
#define COSMOPOLITAN_LIBC_X_XSPAWN_H_
#include "libc/calls/struct/rusage.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int xspawn(struct rusage *);
int xvspawn(void (*)(void *), void *, struct rusage *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_X_XSPAWN_H_ */
