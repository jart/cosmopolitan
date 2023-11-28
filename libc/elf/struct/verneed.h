#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_VERNEED_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_VERNEED_H_
#include "libc/elf/scalar.h"

typedef struct Elf64_Verneed {
  Elf64_Half vn_version;
  Elf64_Half vn_cnt;
  Elf64_Word vn_file;
  Elf64_Word vn_aux;
  Elf64_Word vn_next;
} Elf64_Verneed;

#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_VERNEED_H_ */
