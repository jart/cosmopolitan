#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IOCOUNTERS_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IOCOUNTERS_H_

struct NtIoCounters {
  uint64_t ReadOperationCount;
  uint64_t WriteOperationCount;
  uint64_t OtherOperationCount;
  uint64_t ReadTransferCount;
  uint64_t WriteTransferCount;
  uint64_t OtherTransferCount;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IOCOUNTERS_H_ */
