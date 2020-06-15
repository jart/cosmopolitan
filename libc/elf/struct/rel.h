#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_REL_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_REL_H_
#include "libc/elf/scalar.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef struct Elf64_Rel {
  Elf64_Addr r_offset;
  Elf64_Xword r_info; /** @see ELF64_R_{SYM,SIZE,INFO} */
} Elf64_Rel;

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_REL_H_ */
