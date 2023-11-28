#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_RELA_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_RELA_H_
#include "libc/elf/scalar.h"

/*
 * ELF relocation w/ explicit addend.
 *
 * Relocations let us easily apply fixups to compiled object code. This
 * data structure represents the contents of an `sh_type` w/ `SHT_RELA`
 *
 * @see Elf64_Rel
 */
typedef struct Elf64_Rela {

  /*
   * Location to be modified.
   *
   * If `e_type` is `ET_REL` then this is a section data byte offset.
   *
   * If `e_type` isn't `ET_REL` then this is a virtual address.
   */
  Elf64_Addr r_offset;

  /*
   * Relocation type and symbol.
   *
   * This value may be created using:
   *
   *     r_info = ELF64_R_INFO(sym, type);
   *
   * This value may be read using:
   *
   *     Elf64_Word sym = ELF64_R_SYM(r_info);
   *     Elf64_Word type = ELF64_R_TYPE(r_info);
   *
   * Where `sym` is a symbol index, and `type` will likely be:
   *
   * - `R_X86_64_PC32`
   * - `R_X86_64_PLT32`
   * - `R_X86_64_32`
   * - `R_X86_64_64`
   * - `R_X86_64_32S`
   * - `R_X86_64_8`
   * - `R_X86_64_16`
   * - `R_X86_64_DTPOFF32`
   * - `R_X86_64_GOTPCREL`
   * - `R_X86_64_PC16`
   * - `R_X86_64_REX_GOTPCRELX`
   * - `R_X86_64_TPOFF32`
   *
   */
  Elf64_Xword r_info;

  /*
   * Relocation parameter.
   *
   * Each relocation type has its own mathematical formula, which should
   * incorporate this value in its own unique way.
   */
  Elf64_Sxword r_addend;

} Elf64_Rela;

#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_RELA_H_ */
