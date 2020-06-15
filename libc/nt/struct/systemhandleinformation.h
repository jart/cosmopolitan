#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMHANDLEINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMHANDLEINFORMATION_H_
#include "libc/nt/struct/systemhandleentry.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtSystemHandleInformation {
  uint32_t Count;
  struct NtSystemHandleEntry Handle[1];
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMHANDLEINFORMATION_H_ */
