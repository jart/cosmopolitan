#ifndef COSMOPOLITAN_LIBC_NT_TYPEDEF_HOOKPROC_H_
#define COSMOPOLITAN_LIBC_NT_TYPEDEF_HOOKPROC_H_
#include "libc/nt/thunk/msabi.h"
COSMOPOLITAN_C_START_

typedef intptr_t (*__msabi NtHookProc)(int code, uintptr_t wParam,
				       intptr_t lParam);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_TYPEDEF_HOOKPROC_H_ */
