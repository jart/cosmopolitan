#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_PROCESSMEMORYCOUNTERS_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_PROCESSMEMORYCOUNTERS_H_
COSMOPOLITAN_C_START_

struct NtProcessMemoryCountersEx {
  uint32_t cb; /* count bytes */
  uint32_t PageFaultCount;
  uint64_t PeakWorkingSetSize;
  uint64_t WorkingSetSize;
  uint64_t QuotaPeakPagedPoolUsage;
  uint64_t QuotaPagedPoolUsage;
  uint64_t QuotaPeakNonPagedPoolUsage;
  uint64_t QuotaNonPagedPoolUsage;
  uint64_t PagefileUsage;
  uint64_t PeakPagefileUsage;
  uint64_t PrivateUsage;
};

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_PROCESSMEMORYCOUNTERS_H_ */
