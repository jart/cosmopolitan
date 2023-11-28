#ifndef COSMOPOLITAN_LIBC_NT_VERSION_H_
#define COSMOPOLITAN_LIBC_NT_VERSION_H_
#include "libc/nt/struct/osversioninfo.h"
COSMOPOLITAN_C_START_

bool32 IsAtLeastWindows10(void) pureconst;
bool32 GetVersionEx(struct NtOsVersionInfo *lpVersionInformation);

#if defined(__GNUC__) && !defined(__STRICT_ANSI__) && defined(__x86_64__)
#define IsAtLeastWindows10() (GetNtMajorVersion() >= 10)
#define GetNtMajorVersion()    \
  ({                           \
    uintptr_t __x;             \
    asm("mov\t%%gs:96,%q0\r\n" \
        "mov\t280(%q0),%b0"    \
        : "=q"(__x));          \
    (unsigned char)__x;        \
  })
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_VERSION_H_ */
