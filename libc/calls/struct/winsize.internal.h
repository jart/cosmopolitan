#ifndef COSMOPOLITAN_LIBC_CALLS_STRUCT_WINSIZE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_CALLS_STRUCT_WINSIZE_INTERNAL_H_
#include "libc/calls/struct/fd.internal.h"
#include "libc/calls/struct/winsize.h"
#include "libc/mem/alloca.h"
COSMOPOLITAN_C_START_

int tcgetwinsize_nt(int, struct winsize *);
const char *DescribeWinsize(char[64], int, const struct winsize *);
#define DescribeWinsize(rc, ws) DescribeWinsize(alloca(64), rc, ws)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_CALLS_STRUCT_WINSIZE_INTERNAL_H_ */
