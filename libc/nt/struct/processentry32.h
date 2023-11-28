#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_PROCESSENTRY32_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_PROCESSENTRY32_H_

struct NtProcessEntry32 {
  uint32_t dwSize;
  uint32_t cntUsage; /* unused */
  uint32_t th32ProcessID;
  uint64_t th32DefaultHeapID; /* unused */
  uint32_t th32ModuleID;      /* unused */
  uint32_t cntThreads;
  uint32_t th32ParentProcessID;
  int32_t cPriClassBase;
  uint32_t dwFlags; /* unused */
  char16_t szExeFile[260];
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_PROCESSENTRY32_H_ */
