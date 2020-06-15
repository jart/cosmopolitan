#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_SYMINFO_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_SYMINFO_H_
#include "libc/elf/scalar.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef struct Elf64_Syminfo {
  Elf64_Half si_boundto;
  Elf64_Half si_flags;
} Elf64_Syminfo;

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_SYMINFO_H_ */
