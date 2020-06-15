#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_AUXV_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_AUXV_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

typedef struct Elf64_auxv_t {
  uint64_t a_type;
  union {
    uint64_t a_val;
  } a_un;
} Elf64_auxv_t;

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_AUXV_H_ */
