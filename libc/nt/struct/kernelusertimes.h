#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_KERNELUSERTIMES_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_KERNELUSERTIMES_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtKernelUserTimes {
  uint64_t CreateFileTime;
  uint64_t ExitFileTime;
  int64_t KernelTime;
  int64_t UserTime;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_KERNELUSERTIMES_H_ */
