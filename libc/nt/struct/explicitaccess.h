#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_EXPLICITACCESS_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_EXPLICITACCESS_H_
#include "libc/nt/struct/trustee.h"

struct NtExplicitAccess {
  uint32_t grfAccessPermissions;
  uint32_t grfAccessMode;
  uint32_t grfInheritance;
  struct NtTrustee Trustee;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_EXPLICITACCESS_H_ */
