#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEENCLAVECONFIG_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEENCLAVECONFIG_H_
#include "libc/nt/pedef.internal.h"

struct NtImageEnclaveConfig {
  uint32_t Size;
  uint32_t MinimumRequiredConfigSize;
  uint32_t PolicyFlags;
  uint32_t NumberOfImports;
  uint32_t ImportList;
  uint32_t ImportEntrySize;
  uint8_t FamilyID[kNtImageEnclaveShortIdLength];
  uint8_t ImageID[kNtImageEnclaveShortIdLength];
  uint32_t ImageVersion;
  uint32_t SecurityVersion;
  uint64_t EnclaveSize;
  uint32_t NumberOfThreads;
  uint32_t EnclaveFlags;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEENCLAVECONFIG_H_ */
