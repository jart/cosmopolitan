#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_PROCESSINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_PROCESSINFORMATION_H_

struct NtProcessInformation {
  int64_t hProcess;
  int64_t hThread;
  uint32_t dwProcessId;
  uint32_t dwThreadId;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_PROCESSINFORMATION_H_ */
