#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_RELA_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_RELA_H_
#include "libc/elf/scalar.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef struct Elf64_Rela {
  /*u64*/ Elf64_Addr r_offset;
  /*
   * ELF64_R_SYM(r_info) → sym
   * ELF64_R_TYPE(r_info) → R_X86_64_{64,PC32,GOTPCRELX,...}
   * ELF64_R_INFO(sym, type) → r_info
   */
  /*u64*/ Elf64_Xword r_info; /* ELF64_R_{SYM,SIZE,INFO} */
  /*i64*/ Elf64_Sxword r_addend;
} Elf64_Rela;

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_RELA_H_ */
