#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDYNAMICRELOCATIONTABLE_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDYNAMICRELOCATIONTABLE_H_
#include "libc/nt/struct/imagedynamicrelocation.internal.h"

struct NtImageDynamicRelocationTable {
  uint32_t Version;
  uint32_t Size;
  struct NtImageDynamicRelocation DynamicRelocations[0];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDYNAMICRELOCATIONTABLE_H_ */
