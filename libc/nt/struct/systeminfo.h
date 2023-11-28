#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMINFO_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMINFO_H_

struct NtSystemInfo {
  union {
    uint32_t dwOemId;
    struct {
      uint16_t wProcessorArchitecture;
      uint16_t wReserved;
    };
  };
  uint32_t dwPageSize;
  void *lpMinimumApplicationAddress;
  void *lpMaximumApplicationAddress;
  uintptr_t dwActiveProcessorMask;
  uint32_t dwNumberOfProcessors;
  uint32_t dwProcessorType;
  uint32_t dwAllocationGranularity;
  uint16_t wProcessorLevel;
  uint16_t wProcessorRevision;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMINFO_H_ */
