#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_CHDR_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_CHDR_H_
#include "libc/elf/scalar.h"

typedef struct Elf64_Chdr {
  Elf64_Word ch_type;
  Elf64_Word ch_reserved;
  Elf64_Xword ch_size;
  Elf64_Xword ch_addralign;
} Elf64_Chdr;

#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_CHDR_H_ */
