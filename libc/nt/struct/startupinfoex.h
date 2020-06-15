#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_STARTUPINFOEX_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_STARTUPINFOEX_H_
#include "libc/nt/struct/startupinfo.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct NtProcThreadAttributeList;

struct NtStartupInfoEx {
  struct NtStartupInfo StartupInfo;
  struct NtProcThreadAttributeList *lpAttributeList;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_STARTUPINFOEX_H_ */
