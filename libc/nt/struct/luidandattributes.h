#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_LUIDANDATTRIBUTES_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_LUIDANDATTRIBUTES_H_
#include "libc/nt/struct/luid.h"

struct NtLuidAndAttributes {
  struct NtLuid Luid;
  uint32_t Attributes;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_LUIDANDATTRIBUTES_H_ */
