#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_NHDR_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_NHDR_H_
#include "libc/elf/scalar.h"

typedef struct Elf64_Nhdr {
  Elf64_Word n_namesz;
  Elf64_Word n_descsz;
  Elf64_Word n_type;
} Elf64_Nhdr;

#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_NHDR_H_ */
