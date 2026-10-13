#ifndef COSMOPOLITAN_LIBC_NT_TYPEDEF_HANDLERROUTINE_H_
#define COSMOPOLITAN_LIBC_NT_TYPEDEF_HANDLERROUTINE_H_
#include "libc/nt/thunk/msabi.h"
#include "libc/nt/enum/ctrlevent.h"
COSMOPOLITAN_C_START_

typedef bool32 (*__msabi NtHandlerRoutine)(uint32_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_TYPEDEF_HANDLERROUTINE_H_ */
