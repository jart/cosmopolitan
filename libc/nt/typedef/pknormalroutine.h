#ifndef COSMOPOLITAN_LIBC_NT_TYPEDEF_PKNORMALROUTINE_H_
#define COSMOPOLITAN_LIBC_NT_TYPEDEF_PKNORMALROUTINE_H_
#include "libc/nt/thunk/msabi.h"
COSMOPOLITAN_C_START_

typedef void (*__msabi NtPkNormalRoutine)(void *NormalContext,
					  void *SystemArgument1,
					  void *SystemArgument2);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_TYPEDEF_PKNORMALROUTINE_H_ */
