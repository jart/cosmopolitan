#ifndef COSMOPOLITAN_LIBC_ELF_H_
#define COSMOPOLITAN_LIBC_ELF_H_
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/phdr.h"
#include "libc/elf/struct/shdr.h"
#include "libc/elf/struct/sym.h"
#ifdef _COSMO_SOURCE
COSMOPOLITAN_C_START_
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § executable linkable format                                ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
/* clang-format off */

bool32 IsElfSymbolContent(const Elf64_Sym *);
bool32 IsElf64Binary(const Elf64_Ehdr *, size_t);
char *GetElfStringTable(const Elf64_Ehdr *, size_t, const char *);
Elf64_Sym *GetElfSymbols(const Elf64_Ehdr *, size_t, int, Elf64_Xword *);
Elf64_Shdr *GetElfSymbolTable(const Elf64_Ehdr *, size_t, int, Elf64_Xword *);
Elf64_Phdr *GetElfProgramHeaderAddress(const Elf64_Ehdr *, size_t, Elf64_Half);
Elf64_Shdr *GetElfSectionHeaderAddress(const Elf64_Ehdr *, size_t, Elf64_Half);
Elf64_Shdr *FindElfSectionByName(const Elf64_Ehdr *, size_t, char *, const char *);
char *GetElfString(const Elf64_Ehdr *, size_t, const char *, Elf64_Word);
void *GetElfSectionAddress(const Elf64_Ehdr *, size_t, const Elf64_Shdr *);
void *GetElfSegmentAddress(const Elf64_Ehdr *, size_t, const Elf64_Phdr *);
char *GetElfSectionName(const Elf64_Ehdr *, size_t, const Elf64_Shdr *);
char *GetElfSectionNameStringTable(const Elf64_Ehdr *, size_t);

COSMOPOLITAN_C_END_
#endif /* _COSMO_SOURCE */
#endif /* COSMOPOLITAN_LIBC_ELF_H_ */
