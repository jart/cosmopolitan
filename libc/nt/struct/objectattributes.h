#ifndef COSMOPOLITAN_LIBC_NT_I_OBJECTATTRIBUTES_H_
#define COSMOPOLITAN_LIBC_NT_I_OBJECTATTRIBUTES_H_
#include "libc/nt/struct/rtluserprocessparameters.h"
#include "libc/nt/struct/securityattributes.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtObjectAttributes {
  uint32_t Length;
  int64_t RootDirectory;
  struct NtUnicodeString *ObjectName;
  uint32_t Attributes; /* OBJ_INHERIT, etc. */
  struct NtSecurityDescriptor *SecurityDescriptor;
  void *SecurityQualityOfService;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_I_OBJECTATTRIBUTES_H_ */
