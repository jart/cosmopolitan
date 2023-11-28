#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMBASICINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMBASICINFORMATION_H_

struct NtSystemBasicInformation {
  unsigned char Reserved1[4];
  uint32_t MaximumIncrement;
  uint32_t PhysicalPageSize;
  uint32_t NumberOfPhysicalPages;
  uint32_t LowestPhysicalPage;
  uint32_t HighestPhysicalPage;
  uint32_t AllocationGranularity;
  uint32_t LowestUserAddress;
  uint32_t HighestUserAddress;
  uint32_t ActiveProcessors;
  char NumberOfProcessors;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_SYSTEMBASICINFORMATION_H_ */
