#ifndef COSMOPOLITAN_LIBC_NT_TYPEDEF_TIMERPROC_H_
#define COSMOPOLITAN_LIBC_NT_TYPEDEF_TIMERPROC_H_
#include "libc/nt/thunk/msabi.h"
COSMOPOLITAN_C_START_

typedef void (*__msabi NtTimerProc)(int64_t, uint32_t, uintptr_t, uint32_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_TYPEDEF_TIMERPROC_H_ */
