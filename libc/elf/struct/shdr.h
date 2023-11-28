#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_SHDR_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_SHDR_H_
#include "libc/elf/scalar.h"

/**
 * Section header.
 * @see https://docs.oracle.com/cd/E19683-01/816-1386/chapter6-94076/index.html
 */
typedef struct Elf64_Shdr {

  Elf64_Word sh_name;

  Elf64_Word sh_type; /* SHT_{PROGBITS,NOBITS,STRTAB,SYMTAB,RELA,...} */

  Elf64_Xword sh_flags; /* SHF_{WRITE,ALLOC,EXECINSTR,TLS,MERGE,STRINGS,,...} */

  Elf64_Addr sh_addr;

  Elf64_Off sh_offset;

  Elf64_Xword sh_size;

  /*
   * Index of linked section header.
   *
   * If `sh_type` is `SHT_RELA` then `sh_link` holds the section header
   * index of the associated symbol table.
   *
   * If `sh_type` is `SHT_SYMTAB` then `sh_link` holds the section
   * header index of the associated string table.
   */
  Elf64_Word sh_link;

  /*
   * If `sh_type` is `SHT_RELA` then `sh_info` contains the index of the
   * section to which relocations apply.
   *
   * If `sh_type` is `SHT_SYMTAB` or `SHT_DYNSYM` then `sh_info`
   * contains an index that's one greater than symbol table index of
   * last `STB_LOCAL` symbol.
   */
  Elf64_Word sh_info;

  Elf64_Xword sh_addralign;

  Elf64_Xword sh_entsize;

} Elf64_Shdr;

#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_SHDR_H_ */
