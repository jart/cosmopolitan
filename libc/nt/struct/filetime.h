#ifndef COSMOPOLITAN_LIBC_NT_FILETIME_H_
#define COSMOPOLITAN_LIBC_NT_FILETIME_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtFileTime {
  uint32_t dwLowDateTime;
  uint32_t dwHighDateTime;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_FILETIME_H_ */
