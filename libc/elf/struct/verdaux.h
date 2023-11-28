#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_VERDAUX_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_VERDAUX_H_
#include "libc/elf/scalar.h"

typedef struct Elf64_Verdaux {
  Elf64_Word vda_name;
  Elf64_Word vda_next;
} Elf64_Verdaux;

#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_VERDAUX_H_ */
