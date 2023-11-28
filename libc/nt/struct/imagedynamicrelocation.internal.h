#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDYNAMICRELOCATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDYNAMICRELOCATION_H_
#include "libc/nt/struct/imagebaserelocation.internal.h"

struct NtImageDynamicRelocation {
  uint64_t Symbol;
  uint32_t BaseRelocSize;
  struct NtImageBaseRelocation BaseRelocations[0];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDYNAMICRELOCATION_H_ */
