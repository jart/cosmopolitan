#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_VALENT_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_VALENT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtValent {
  char16_t *ve_valuename;
  uint32_t ve_valuelen;
  uintptr_t ve_valueptr;
  uint32_t ve_type;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_VALENT_H_ */
