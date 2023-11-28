#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_MEMORYSTATUSEX_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_MEMORYSTATUSEX_H_

struct NtMemoryStatusEx {
  uint32_t dwLength;
  uint32_t dwMemoryLoad;
  uint64_t ullTotalPhys;
  uint64_t ullAvailPhys;
  uint64_t ullTotalPageFile;
  uint64_t ullAvailPageFile;
  uint64_t ullTotalVirtual;
  uint64_t ullAvailVirtual;
  uint64_t ullAvailExtendedVirtual;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_MEMORYSTATUSEX_H_ */
