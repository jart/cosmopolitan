#ifndef COSMOPOLITAN_LIBC_NT_TYPEDEF_IOAPCROUTINE_H_
#define COSMOPOLITAN_LIBC_NT_TYPEDEF_IOAPCROUTINE_H_
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/struct/iostatusblock.h"
COSMOPOLITAN_C_START_

typedef void (*__msabi NtIoApcRoutine)(void *ApcContext,
				       struct NtIoStatusBlock *IoStatusBlock,
				       uint32_t Reserved);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_TYPEDEF_IOAPCROUTINE_H_ */
