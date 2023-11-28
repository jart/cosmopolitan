#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_EHDR_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_EHDR_H_
#include "libc/elf/scalar.h"

/*
 * ELF header.
 */
typedef struct Elf64_Ehdr {

  /*
   * Leading bytes of ELF header.
   *
   * - `e_ident[0]` is always `127`
   * - `e_ident[1]` is always `'E'`
   * - `e_ident[2]` is always `'L'`
   * - `e_ident[3]` is always `'F'`
   *
   * - `e_ident[EI_CLASS]` is mandatory and should be:
   *
   *   - `ELFCLASSNONE64` if it's an Elf64 image
   *   - `ELFCLASSNONE32` if it's an Elf32 image
   *   - Otherwise we assume it's an Elf64 image
   *
   * - `e_ident[EI_DATA]` is advisory and could be:
   *
   *   - `ELFDATANONE` isn't strictly valid
   *   - `ELFDATA2LSB` for little-endian
   *   - `ELFDATA2MSB` for big-endian
   *
   * - `e_ident[EI_VERSION]` is advisory and should be:
   *
   *   - `EV_NONE` if it's zero or unspecified
   *   - `EV_CURRENT` for current ELF version (which is 1)
   *
   * - `e_ident[EI_OSABI]` is mandatory and could be:
   *
   *   - `ELFOSABI_NONE` is zero
   *   - `ELFOSABI_GNU` is for GNU
   *   - `ELFOSABI_SYSV` used by GNU
   *   - `ELFOSABI_LINUX` doesn't care
   *   - `ELFOSABI_FREEBSD` does care (recommended)
   *   - `ELFOSABI_NETBSD` doesn't care (see `PT_NOTE`)
   *   - `ELFOSABI_OPENBSD` doesn't care (see `PT_NOTE`)
   *
   * - `e_ident[EI_ABIVERSION]` is advisory
   *
   */
  unsigned char e_ident[16];

  /*
   * ELF image type.
   *
   * This field is mandatory and should be one of:
   *
   * - `ET_REL` for `.o` object files
   * - `ET_DYN` for `.so` files and `-pie` executables
   * - `ET_EXEC` for statically-linked executables
   *
   */
  Elf64_Half e_type;

  /*
   * ELF machine type.
   *
   * This field is mandatory and could be one of:
   *
   * - `EM_M32` for Bellmac
   * - `EM_X86_64` for Amd64
   * - `EM_AARCH64` for Arm64
   * - `EM_PPC64` for Raptors
   * - `EM_RISCV` for Berkeley
   * - `EM_S390` for System/360
   *
   */
  Elf64_Half e_machine;

  /*
   * ELF version.
   *
   * This field is advisory and could be:
   *
   * - `EV_NONE` if it's zero or unspecified
   * - `EV_CURRENT` for current ELF version (which is 1)
   *
   * @see `e_ident[EI_VERSION]`
   */
  Elf64_Word e_version;

  /*
   * ELF executable entrypoint.
   *
   * Static executables should use this field to store the virtual
   * address of the _start() function. This field may be zero, for
   * unspecified.
   */
  Elf64_Addr e_entry;

  /*
   * `Elf64_Phdr` file offset.
   *
   * This field is mandatory. Object files should set it to zero.
   */
  Elf64_Off e_phoff;

  /*
   * `Elf64_Shdr` file offset.
   *
   * This field is advisory.
   */
  Elf64_Off e_shoff;

  /*
   * ELF flags.
   *
   * This field is advisory.
   */
  Elf64_Word e_flags;

  /*
   * `Elf64_Ehdr` size.
   *
   * This field is advisory and should be 64.
   */
  Elf64_Half e_ehsize;

  /*
   * `Elf64_Phdr` element size.
   *
   * This field *is* cared about and should be set to 56. Cosmopolitan
   * permits larger values for the pleasure of it.
   */
  Elf64_Half e_phentsize;

  /*
   * `Elf64_Phdr` array count.
   */
  Elf64_Half e_phnum;

  /*
   * `Elf64_Shdr` element size.
   *
   * This field is advisory and should be set to 64. Cosmopolitan
   * permits larger values for the pleasure of it.
   */
  Elf64_Half e_shentsize;

  /*
   * `Elf64_Shdr` count.
   *
   * This field is advisory.
   */
  Elf64_Half e_shnum;

  /*
   * Section header index of section name string table.
   */
  Elf64_Half e_shstrndx;

} Elf64_Ehdr;

#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_EHDR_H_ */
