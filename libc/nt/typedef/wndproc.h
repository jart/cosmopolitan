#ifndef COSMOPOLITAN_LIBC_NT_TYPEDEF_WNDPROC_H_
#define COSMOPOLITAN_LIBC_NT_TYPEDEF_WNDPROC_H_
#include "libc/nt/thunk/msabi.h"
COSMOPOLITAN_C_START_

typedef int64_t (*__msabi NtWndProc)(int64_t, uint32_t, uint64_t, int64_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_TYPEDEF_WNDPROC_H_ */
