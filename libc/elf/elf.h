#ifndef COSMOPOLITAN_LIBC_ELF_H_
#define COSMOPOLITAN_LIBC_ELF_H_
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/phdr.h"
#include "libc/elf/struct/shdr.h"
#include "libc/elf/struct/sym.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_
#ifdef COSMO
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § executable linkable format                                ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

char *GetElfStringTable(const Elf64_Ehdr *, size_t, const char *);
Elf64_Sym *GetElfSymbolTable(const Elf64_Ehdr *, size_t, int, Elf64_Xword *);
bool IsElf64Binary(const Elf64_Ehdr *, size_t);
bool IsElfSymbolContent(const Elf64_Sym *);
Elf64_Phdr *GetElfProgramHeaderAddress(const Elf64_Ehdr *, size_t, Elf64_Half);
Elf64_Shdr *GetElfSectionHeaderAddress(const Elf64_Ehdr *, size_t, Elf64_Half);
void *GetElfSectionAddress(const Elf64_Ehdr *, size_t, const Elf64_Shdr *);
char *GetElfSectionNameStringTable(const Elf64_Ehdr *, size_t);
char *GetElfString(const Elf64_Ehdr *, size_t, const char *, Elf64_Word);
const char *GetElfSectionName(const Elf64_Ehdr *, size_t, Elf64_Shdr *);

#endif /* COSMO */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ELF_H_ */
