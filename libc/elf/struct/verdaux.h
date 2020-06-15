#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_VERDAUX_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_VERDAUX_H_
#include "libc/elf/scalar.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef struct Elf64_Verdaux {
  Elf64_Word vda_name;
  Elf64_Word vda_next;
} Elf64_Verdaux;

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_VERDAUX_H_ */
