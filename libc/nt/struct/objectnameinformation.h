#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_OBJECTNAMEINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_OBJECTNAMEINFORMATION_H_
#include "libc/nt/struct/unicodestring.h"

struct NtObjectNameInformation {
  struct NtUnicodeString Name;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_OBJECTNAMEINFORMATION_H_ */
