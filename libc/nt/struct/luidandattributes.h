#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_LUIDANDATTRIBUTES_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_LUIDANDATTRIBUTES_H_
#include "libc/nt/struct/luid.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtLuidAndAttributes {
  struct NtLuid Luid;
  uint32_t Attributes;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_LUIDANDATTRIBUTES_H_ */
