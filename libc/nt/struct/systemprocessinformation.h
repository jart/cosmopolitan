#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMPROCESSINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMPROCESSINFORMATION_H_
#include "libc/nt/struct/iocounters.h"
#include "libc/nt/struct/unicodestring.h"
#include "libc/nt/struct/vmcounters.h"

struct NtSystemProcessInformation {
  uint32_t NextEntryOffset;
  uint32_t NumberOfThreads;
  int64_t Reserved[3];
  int64_t CreateTime;
  int64_t UserTime;
  int64_t KernelTime;
  struct NtUnicodeString ImageName;
  int32_t BasePriority;
  int64_t UniqueProcessId;
  int64_t InheritedFromUniqueProcessId;
  uint32_t HandleCount;
  uint32_t SessionId;
  uint32_t PageDirectoryBase;
  struct NtVmCounters VirtualMemoryCounters;
  size_t PrivatePageCount;
  struct NtIoCounters IoCounters;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMPROCESSINFORMATION_H_ */
