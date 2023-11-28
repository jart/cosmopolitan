#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_VMCOUNTERS_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_VMCOUNTERS_H_

struct NtVmCounters {
  size_t PeakVirtualSize;
  size_t VirtualSize;
  uint32_t PageFaultCount;
  size_t PeakWorkingSetSize;
  size_t WorkingSetSize;
  size_t QuotaPeakPagedPoolUsage;
  size_t QuotaPagedPoolUsage;
  size_t QuotaPeakNonPagedPoolUsage;
  size_t QuotaNonPagedPoolUsage;
  size_t PagefileUsage;
  size_t PeakPagefileUsage;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_VMCOUNTERS_H_ */
