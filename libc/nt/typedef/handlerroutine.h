#ifndef COSMOPOLITAN_LIBC_NT_TYPEDEF_HANDLERROUTINE_H_
#define COSMOPOLITAN_LIBC_NT_TYPEDEF_HANDLERROUTINE_H_
#include "libc/nt/enum/ctrlevent.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef bool32 (*NtHandlerRoutine)(uint32_t);

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_TYPEDEF_HANDLERROUTINE_H_ */
