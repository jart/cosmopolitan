#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_SYMINFO_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_SYMINFO_H_
#include "libc/elf/scalar.h"

typedef struct Elf64_Syminfo {
  Elf64_Half si_boundto;
  Elf64_Half si_flags;
} Elf64_Syminfo;

#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_SYMINFO_H_ */
