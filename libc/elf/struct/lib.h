#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_LIB_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_LIB_H_
#include "libc/elf/scalar.h"

typedef struct Elf64_Lib {
  Elf64_Word l_name;
  Elf64_Word l_time_stamp;
  Elf64_Word l_checksum;
  Elf64_Word l_version;
  Elf64_Word l_flags;
} Elf64_Lib;

#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_LIB_H_ */
