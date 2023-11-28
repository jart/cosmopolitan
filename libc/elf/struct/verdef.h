#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_VERDEF_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_VERDEF_H_
#include "libc/elf/scalar.h"

typedef struct Elf64_Verdef {
  Elf64_Half vd_version;
  Elf64_Half vd_flags;
  Elf64_Half vd_ndx;
  Elf64_Half vd_cnt;
  Elf64_Word vd_hash;
  Elf64_Word vd_aux;
  Elf64_Word vd_next;
} Elf64_Verdef;

#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_VERDEF_H_ */
