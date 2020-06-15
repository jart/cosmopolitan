#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_LIB_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_LIB_H_
#include "libc/elf/scalar.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef struct Elf64_Lib {
  Elf64_Word l_name;
  Elf64_Word l_time_stamp;
  Elf64_Word l_checksum;
  Elf64_Word l_version;
  Elf64_Word l_flags;
} Elf64_Lib;

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_LIB_H_ */
