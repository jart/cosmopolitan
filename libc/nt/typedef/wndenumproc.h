#ifndef COSMOPOLITAN_LIBC_NT_TYPEDEF_WNDENUMPROC_H_
#define COSMOPOLITAN_LIBC_NT_TYPEDEF_WNDENUMPROC_H_
#include "libc/nt/thunk/msabi.h"
COSMOPOLITAN_C_START_

typedef int (*__msabi NtWndEnumProc)(int64_t foo, intptr_t bar);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_TYPEDEF_WNDENUMPROC_H_ */
