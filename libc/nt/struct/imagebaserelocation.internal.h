#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEBASERELOCATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEBASERELOCATION_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtImageBaseRelocation {
  uint32_t VirtualAddress;
  uint32_t SizeOfBlock;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEBASERELOCATION_H_ */
