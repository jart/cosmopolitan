#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_SYM_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_SYM_H_
#include "libc/elf/scalar.h"

typedef struct Elf64_Sym {

  /*
   * Symbol name.
   *
   * This value is a byte offset into the `.strtab` section. If this
   * value is zero, then the symbol has no name.
   */
  Elf64_Word st_name;

  /*
   * Symbol type and binding.
   *
   * This value may be created using:
   *
   *     sym.st_info = ELF64_ST_INFO(bind, type);
   *
   * This value may be read using:
   *
   *     int bind = ELF64_ST_BIND(sym.st_info);
   *     int type = ELF64_ST_TYPE(sym.st_info);
   *
   * Where `bind` is typically:
   *
   * - `STB_LOCAL`
   * - `STB_GLOBAL`
   * - `STB_WEAK`
   *
   * Where `type` is typically:
   *
   * - `STT_NOTYPE`
   * - `STT_OBJECT`
   * - `STT_FUNC`
   * - `STT_SECTION`
   * - `STT_FILE`
   * - `STT_COMMON`
   * - `STT_TLS`
   */
  uint8_t st_info;

  /*
   * Symbol visibility.
   *
   * This value should be accessed using:
   *
   *     int visibility = ELF64_ST_VISIBILITY(sym.st_other);
   *
   * Where `visibility` is typically:
   *
   * - `STV_DEFAULT`
   * - `STV_INTERNAL`
   * - `STV_HIDDEN`
   * - `STV_PROTECTED`
   */
  uint8_t st_other;

  /*
   * Symbol section.
   *
   * If `st_shndx` is within `(SHN_UNDEF,SHN_LORESERVE)` then it holds
   * an index into the section header table.
   *
   * Otherwise `st_shndx` is usually one of the following magic numbers:
   *
   * - `SHN_UNDEF` means symbol is undefined
   * - `SHN_ABS` means symbol is a linker integer
   * - `SHN_COMMON` means symbol is defined traditionally
   */
  Elf64_Section st_shndx;

  /*
   * Symbol value.
   *
   * If `e_type` is `ET_REL` and `st_shndx` is `SHN_COMMON`, then
   * `st_value` holds the required symbol alignment, or ≤ 1 if no
   * alignment is required.
   *
   * If `e_type` is `ET_REL` and `st_shndx` is a section index, then
   * `st_value` holds a byte offset into the section memory.
   *
   * If `e_type` isn't `ET_REL` then `st_value` holds a virtual address.
   */
  Elf64_Addr st_value;

  /* byte length optionally set by .size directive */
  Elf64_Xword st_size;

} Elf64_Sym;

#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_SYM_H_ */
