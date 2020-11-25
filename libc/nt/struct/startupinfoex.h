#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_STARTUPINFOEX_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_STARTUPINFOEX_H_
#include "libc/nt/struct/procthreadattributelist.h"
#include "libc/nt/struct/startupinfo.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtStartupInfoEx {
  struct NtStartupInfo StartupInfo;
  struct NtProcThreadAttributeList *lpAttributeList;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_STARTUPINFOEX_H_ */
