#ifndef COSMOPOLITAN_LIBC_NT_VERSION_H_
#define COSMOPOLITAN_LIBC_NT_VERSION_H_
#include "libc/nt/struct/osversioninfo.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

bool IsAtLeastWindows10(void) pureconst;
bool32 GetVersionEx(struct NtOsVersionInfo *lpVersionInformation);

#if defined(__GCC_ASM_FLAG_OUTPUTS__) && !defined(__STRICT_ANSI__)
#define IsAtLeastWindows10() (GetNtMajorVersion() >= 10)
static pureconst inline unsigned char GetNtMajorVersion(void) {
  uintptr_t _x;
  asm("mov\t%%gs:96,%q0\r\n"
      "mov\t280(%q0),%b0"
      : "=q"(_x));
  return _x;
}
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_VERSION_H_ */
