#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_SECURITYATTRIBUTES_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_SECURITYATTRIBUTES_H_
#include "libc/mem/alloca.h"
#include "libc/nt/struct/securitydescriptor.h"

struct NtSecurityAttributes {
  uint32_t nLength;
  struct NtSecurityDescriptor *lpSecurityDescriptor;
  bool32 bInheritHandle;
};

const char *DescribeNtSecurityAttributes(char[32],
                                         const struct NtSecurityAttributes *);
#define DescribeNtSecurityAttributes(x) \
  DescribeNtSecurityAttributes(alloca(32), x)

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_SECURITYATTRIBUTES_H_ */
