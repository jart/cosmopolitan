#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IOCOUNTERS_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IOCOUNTERS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtIoCounters {
  uint64_t ReadOperationCount;
  uint64_t WriteOperationCount;
  uint64_t OtherOperationCount;
  uint64_t ReadTransferCount;
  uint64_t WriteTransferCount;
  uint64_t OtherTransferCount;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IOCOUNTERS_H_ */
