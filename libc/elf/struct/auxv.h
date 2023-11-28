#ifndef COSMOPOLITAN_LIBC_ELF_STRUCT_AUXV_H_
#define COSMOPOLITAN_LIBC_ELF_STRUCT_AUXV_H_

typedef struct Elf64_auxv_t {
  uint64_t a_type;
  union {
    uint64_t a_val;
  } a_un;
} Elf64_auxv_t;

#endif /* COSMOPOLITAN_LIBC_ELF_STRUCT_AUXV_H_ */
