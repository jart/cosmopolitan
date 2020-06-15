#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_OBJECTALLINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_OBJECTALLINFORMATION_H_
#include "libc/nt/struct/objecttypeinformation.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtObjectAllInformation {
  uint32_t NumberOfObjects;
  struct NtObjectTypeInformation ObjectTypeInformation[1];
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_OBJECTALLINFORMATION_H_ */
