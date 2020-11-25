#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDYNAMICRELOCATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDYNAMICRELOCATION_H_
#include "libc/nt/struct/imagebaserelocation.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageDynamicRelocation {
  uint64_t Symbol;
  uint32_t BaseRelocSize;
  struct NtImageBaseRelocation BaseRelocations[0];
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEDYNAMICRELOCATION_H_ */
