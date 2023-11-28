#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_MEMORYBASICINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_MEMORYBASICINFORMATION_H_

struct NtMemoryBasicInformation {
  void *BaseAddress;
  void *AllocationBase;
  uint32_t AllocationProtect;
  uint64_t RegionSize;
  uint32_t State;
  uint32_t Protect;
  uint32_t Type;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_MEMORYBASICINFORMATION_H_ */
