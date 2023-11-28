#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_REL_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_REL_H_
#include "libc/elf/scalar.h"

/*
 * ELF relocation.
 *
 * Relocations let us easily apply fixups to compiled object code. This
 * data structure represents the contents of an `sh_type` w/ `SHT_REL`.
 *
 * @see Elf64_Rela
 */
typedef struct Elf64_Rel {

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
   * Where `sym` is a symbol index, and `type` might be:
   *
   * - `R_X86_64_64`
   * - `R_X86_64_PC32`
   * - `R_X86_64_GOTPCRELX`
   * - `R_AARCH64_ABS64`
   *
   * Each relocation type specifies a mathematical formula that's used
   * to compute the appropriate value for the fixed-up object code. If
   * it needs an addend, then this struct doesn't have one, but it can
   * still be embedded by the compiler in the location to be modified.
   */
  Elf64_Xword r_info;

} Elf64_Rel;

#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_REL_H_ */
