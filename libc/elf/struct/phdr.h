#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_PHDR_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_PHDR_H_
#include "libc/elf/scalar.h"

typedef struct Elf64_Phdr {
  Elf64_Word p_type;
  Elf64_Word p_flags;
  Elf64_Off p_offset;
  Elf64_Addr p_vaddr;
  Elf64_Addr p_paddr;
  Elf64_Xword p_filesz;
  Elf64_Xword p_memsz;
  Elf64_Xword p_align;
} Elf64_Phdr;

#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_PHDR_H_ */
