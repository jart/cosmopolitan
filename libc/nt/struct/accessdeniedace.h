#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_ACCESSDENIEDACE_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_ACCESSDENIEDACE_H_
#include "libc/nt/struct/aceheader.h"

struct NtAccessDeniedAce {
  struct NtAceHeader Header;
  uint32_t Mask;
  uint32_t SidStart;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_ACCESSDENIEDACE_H_ */
