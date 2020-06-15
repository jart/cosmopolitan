#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_PRIVILEGESET_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_PRIVILEGESET_H_
#include "libc/nt/struct/luidandattributes.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtPrivilegeSet {
  uint32_t PrivilegeCount;
  uint32_t Control;
  struct NtLuidAndAttributes Privilege[1];
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_PRIVILEGESET_H_ */
