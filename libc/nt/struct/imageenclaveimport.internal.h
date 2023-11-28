#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEENCLAVEIMPORT_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEENCLAVEIMPORT_H_
#include "libc/nt/pedef.internal.h"

struct NtImageEnclaveImport {
  uint32_t MatchType;
  uint32_t MinimumSecurityVersion;
  uint8_t UniqueOrAuthorID[kNtImageEnclaveLongIdLength];
  uint8_t FamilyID[kNtImageEnclaveShortIdLength];
  uint8_t ImageID[kNtImageEnclaveShortIdLength];
  uint32_t ImportName;
  uint32_t Reserved;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_IMAGEENCLAVEIMPORT_H_ */
