#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_TOKENPRIVILEGES_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_TOKENPRIVILEGES_H_
#include "libc/nt/struct/luidandattributes.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtTokenPrivileges {
  uint32_t PrivilegeCount;
  struct NtLuidAndAttributes Privileges[1];
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_TOKENPRIVILEGES_H_ */
