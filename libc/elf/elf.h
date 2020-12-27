#ifndef COSMOPOLITAN_LIBC_ELF_H_
#define COSMOPOLITAN_LIBC_ELF_H_
#include "libc/elf/struct/ehdr.h"
#include "libc/elf/struct/phdr.h"
#include "libc/elf/struct/shdr.h"
#include "libc/elf/struct/sym.h"
#include "libc/runtime/ezmap.internal.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § executable & linkable format                              ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

char *GetElfStringTable(const Elf64_Ehdr *, size_t);
Elf64_Sym *GetElfSymbolTable(const Elf64_Ehdr *, size_t, Elf64_Xword *);
bool IsElf64Binary(const Elf64_Ehdr *, size_t);
void CheckElfAddress(const Elf64_Ehdr *, size_t, intptr_t, size_t);
bool IsElfSymbolContent(const Elf64_Sym *);
Elf64_Phdr *GetElfSegmentHeaderAddress(const Elf64_Ehdr *, size_t, unsigned);
Elf64_Shdr *GetElfSectionHeaderAddress(const Elf64_Ehdr *, size_t, Elf64_Half);
void *GetElfSectionAddress(const Elf64_Ehdr *, size_t, const Elf64_Shdr *);
char *GetElfSectionNameStringTable(const Elf64_Ehdr *, size_t);
void GetElfVirtualAddressRange(const Elf64_Ehdr *, size_t, intptr_t *,
                               intptr_t *);
char *GetElfString(const Elf64_Ehdr *, size_t, const char *, Elf64_Word);
const char *GetElfSectionName(const Elf64_Ehdr *, size_t, Elf64_Shdr *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ELF_H_ */
