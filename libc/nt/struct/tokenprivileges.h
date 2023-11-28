#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_TOKENPRIVILEGES_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_TOKENPRIVILEGES_H_
#include "libc/nt/struct/luidandattributes.h"

struct NtTokenPrivileges {
  uint32_t PrivilegeCount;
  struct NtLuidAndAttributes Privileges[1];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_TOKENPRIVILEGES_H_ */
