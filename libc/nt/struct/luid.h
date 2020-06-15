#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_LUID_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_LUID_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtLuid {
  uint32_t LowPart;
  int32_t HighPart;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_LUID_H_ */
