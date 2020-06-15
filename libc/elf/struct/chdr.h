#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_CHDR_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_CHDR_H_
#include "libc/elf/scalar.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef struct Elf64_Chdr {
  Elf64_Word ch_type;
  Elf64_Word ch_reserved;
  Elf64_Xword ch_size;
  Elf64_Xword ch_addralign;
} Elf64_Chdr;

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_CHDR_H_ */
