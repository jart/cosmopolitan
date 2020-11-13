#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_RECT_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_RECT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct NtRect {
  int32_t left;
  int32_t top;
  int32_t right;
  int32_t bottom;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_RECT_H_ */
