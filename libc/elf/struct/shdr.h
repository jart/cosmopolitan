#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_SHDR_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_SHDR_H_
#include "libc/elf/scalar.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Section header.
 * @see https://docs.oracle.com/cd/E19683-01/816-1386/chapter6-94076/index.html
 */
typedef struct Elf64_Shdr {
  Elf64_Word sh_name;
  Elf64_Word sh_type;   /* SHT_{PROGBITS,NOBITS,STRTAB,SYMTAB,RELA,...} */
  Elf64_Xword sh_flags; /* SHF_{WRITE,ALLOC,EXECINSTR,MERGE,STRINGS,...} */
  Elf64_Addr sh_addr;
  Elf64_Off sh_offset;
  Elf64_Xword sh_size;
  /*
   * If SHT_RELA: Index of section of associated symbol table.
   * If SHT_SYMTAB: Index of section of associated string table.
   */
  Elf64_Word sh_link;
  /*
   * If SHT_RELA: Index of section to which relocations apply.
   * If SHT_SYMTAB: One greater than symbol table index of last local symbol.
   */
  Elf64_Word sh_info;
  Elf64_Xword sh_addralign;
  Elf64_Xword sh_entsize;
} Elf64_Shdr;

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_SHDR_H_ */
