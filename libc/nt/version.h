#ifndef COSMOPOLITAN_LIBC_NT_VERSION_H_
#define COSMOPOLITAN_LIBC_NT_VERSION_H_
#include "libc/nt/struct/osversioninfo.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

bool IsAtLeastWindows10(void) pureconst;
bool32 GetVersionEx(struct NtOsVersionInfo *lpVersionInformation);

#if defined(__GCC_ASM_FLAG_OUTPUTS__) && !defined(__STRICT_ANSI__)
#define IsAtLeastWindows10()            \
  ({                                    \
    long ReG;                           \
    bool NoTbelow;                      \
    asm("mov\t%%gs:96,%1\r\n"           \
        "cmpb\t%2,280(%1)"              \
        : "=@ccnb"(NoTbelow), "=l"(ReG) \
        : "i"(10));                     \
    NoTbelow;                           \
  })
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_VERSION_H_ */
