#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_OBJECTNAMEINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_OBJECTNAMEINFORMATION_H_
#include "libc/nt/struct/unicodestring.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtObjectNameInformation {
  struct NtUnicodeString Name;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_OBJECTNAMEINFORMATION_H_ */
