#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_SECURITYATTRIBUTES_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_SECURITYATTRIBUTES_H_
#include "libc/mem/alloca.h"
#include "libc/nt/struct/securitydescriptor.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtSecurityAttributes {
  uint32_t nLength;
  struct NtSecurityDescriptor *lpSecurityDescriptor;
  bool32 bInheritHandle;
};

const char *DescribeNtSecurityAttributes(char[32],
                                         struct NtSecurityAttributes *);
#define DescribeNtSecurityAttributes(x) \
  DescribeNtSecurityAttributes(alloca(32), x)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_SECURITYATTRIBUTES_H_ */
