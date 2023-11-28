#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_OBJECTBASICINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_OBJECTBASICINFORMATION_H_
#include "libc/nt/enum/accessmask.h"

struct NtObjectBasicInformation {
  uint32_t Attributes;
  uint32_t GrantedAccess;
  uint32_t HandleCount;
  uint32_t PointerCount;
  uint32_t PagedPoolUsage;
  uint32_t NonPagedPoolUsage;
  uint32_t Reserved[3];
  uint32_t NameInformationLength;
  uint32_t TypeInformationLength;
  uint32_t SecurityDescriptorLength;
  int64_t CreateTime;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_OBJECTBASICINFORMATION_H_ */
