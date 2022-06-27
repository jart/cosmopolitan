#ifndef COSMOPOLITAN_LIBC_ELF_PF2PROT_INTERNAL_H_
#define COSMOPOLITAN_LIBC_ELF_PF2PROT_INTERNAL_H_
#include "libc/elf/def.h"

#define _PF2PROT(x) ((PF_R & (x)) >> 2 | (PF_W & (x)) | (PF_X & (x)) << 2)

#endif /* COSMOPOLITAN_LIBC_ELF_PF2PROT_INTERNAL_H_ */
