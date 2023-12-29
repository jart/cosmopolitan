#ifndef COSMOPOLITAN_LIBC_X_XGETLINE_H_
#define COSMOPOLITAN_LIBC_X_XGETLINE_H_
#include "libc/stdio/stdio.h"
COSMOPOLITAN_C_START_

char *xgetline(FILE *) paramsnonnull() mallocesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_X_XGETLINE_H_ */
