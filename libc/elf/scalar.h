#ifndef COSMOPOLITAN_LIBC_ELF_SCALAR_H_
#define COSMOPOLITAN_LIBC_ELF_SCALAR_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define Elf64_Half uint16_t
#define Elf64_Word uint32_t
#define Elf64_Sword int32_t
#define Elf64_Xword uint64_t
#define Elf64_Sxword int64_t
#define Elf64_Addr uint64_t
#define Elf64_Off uint64_t
#define Elf64_Section uint16_t
#define Elf64_Versym Elf64_Half

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ELF_SCALAR_H_ */
