#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_PRIVILEGESET_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_PRIVILEGESET_H_
#include "libc/nt/struct/luidandattributes.h"

struct NtPrivilegeSet {
  uint32_t PrivilegeCount;
  uint32_t Control;
  struct NtLuidAndAttributes Privilege[1];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_PRIVILEGESET_H_ */
