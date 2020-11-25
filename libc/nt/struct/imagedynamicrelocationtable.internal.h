#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDYNAMICRELOCATIONTABLE_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDYNAMICRELOCATIONTABLE_H_
#include "libc/nt/struct/imagedynamicrelocation.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageDynamicRelocationTable {
  uint32_t Version;
  uint32_t Size;
  struct NtImageDynamicRelocation DynamicRelocations[0];
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDYNAMICRELOCATIONTABLE_H_ */
