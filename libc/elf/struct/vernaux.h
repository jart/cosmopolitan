#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_VERNAUX_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_VERNAUX_H_
#include "libc/elf/scalar.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef struct Elf64_Vernaux {
  Elf64_Word vna_hash;
  Elf64_Half vna_flags;
  Elf64_Half vna_other;
  Elf64_Word vna_name;
  Elf64_Word vna_next;
} Elf64_Vernaux;

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_VERNAUX_H_ */
