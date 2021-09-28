#ifndef COSMOPOLITAN_LIBC_NT_VERSION_H_
#define COSMOPOLITAN_LIBC_NT_VERSION_H_
#include "libc/nt/struct/osversioninfo.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

bool32 GetVersionEx(struct NtOsVersionInfo *lpVersionInformation);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_VERSION_H_ */
