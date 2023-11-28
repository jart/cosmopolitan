#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_OBJECTTYPEINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_OBJECTTYPEINFORMATION_H_
#include "libc/nt/struct/genericmapping.h"
#include "libc/nt/struct/unicodestring.h"

struct NtObjectTypeInformation {
  struct NtUnicodeString TypeName;
  uint32_t TotalNumberOfObjects;
  uint32_t TotalNumberOfHandles;
  uint32_t TotalPagedPoolUsage;
  uint32_t TotalNonPagedPoolUsage;
  uint32_t TotalNamePoolUsage;
  uint32_t TotalHandleTableUsage;
  uint32_t HighWaterNumberOfObjects;
  uint32_t HighWaterNumberOfHandles;
  uint32_t HighWaterPagedPoolUsage;
  uint32_t HighWaterNonPagedPoolUsage;
  uint32_t HighWaterNamePoolUsage;
  uint32_t HighWaterHandleTableUsage;
  uint32_t InvalidAttributes;
  struct NtGenericMapping GenericMapping;
  uint32_t ValidAccessMask;
  bool32 SecurityRequired;
  bool32 MaintainHandleCount;
  uint32_t PoolType;
  uint32_t DefaultPagedPoolCharge;
  uint32_t DefaultNonPagedPoolCharge;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_OBJECTTYPEINFORMATION_H_ */
