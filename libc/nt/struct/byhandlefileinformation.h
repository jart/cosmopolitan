#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_BYHANDLEFILEINFORMATION_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_BYHANDLEFILEINFORMATION_H_
#include "libc/nt/struct/filetime.h"

struct NtByHandleFileInformation {
  uint32_t dwFileAttributes; /* ←NtFileFlagAndAttributes */
  struct NtFileTime ftCreationFileTime;
  struct NtFileTime ftLastAccessFileTime;
  struct NtFileTime ftLastWriteFileTime;
  uint32_t dwVolumeSerialNumber;
  uint32_t nFileSizeHigh;
  uint32_t nFileSizeLow;
  uint32_t nNumberOfLinks;
  uint32_t nFileIndexHigh;
  uint32_t nFileIndexLow;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_BYHANDLEFILEINFORMATION_H_ */
