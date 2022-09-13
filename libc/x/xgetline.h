#ifndef COSMOPOLITAN_LIBC_X_XGETLINE_H_
#define COSMOPOLITAN_LIBC_X_XGETLINE_H_
#include "libc/stdio/stdio.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

char *xgetline(struct FILE *) paramsnonnull() mallocesque;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_X_XGETLINE_H_ */
