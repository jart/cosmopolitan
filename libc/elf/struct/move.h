#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_MOVE_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_MOVE_H_
#include "libc/elf/scalar.h"

typedef struct Elf64_Move {
  Elf64_Xword m_value;
  Elf64_Xword m_info;
  Elf64_Xword m_poffset;
  Elf64_Half m_repeat;
  Elf64_Half m_stride;
} Elf64_Move;

#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_MOVE_H_ */
