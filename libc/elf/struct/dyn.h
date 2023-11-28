#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_DYN_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_DYN_H_
#include "libc/elf/scalar.h"

typedef struct Elf64_Dyn {
  Elf64_Sxword d_tag;
  union {
    Elf64_Xword d_val;
    Elf64_Addr d_ptr;
  } d_un;
} Elf64_Dyn;

#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_DYN_H_ */
