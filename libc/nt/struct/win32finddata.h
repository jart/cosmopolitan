#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_WIN32FINDDATA_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_WIN32FINDDATA_H_
#include "libc/nt/struct/filetime.h"

struct NtWin32FindData {
  uint32_t dwFileAttributes;
  struct NtFileTime ftCreationTime;
  struct NtFileTime ftLastAccessTime;
  struct NtFileTime ftLastWriteTime;
  uint32_t nFileSizeHigh;
  uint32_t nFileSizeLow;
  uint32_t dwReserved0;
  uint32_t dwReserved1;
  char16_t cFileName[260];
  char16_t cAlternateFileName[14];
  uint32_t dwFileType;    /* obsolete */
  uint32_t dwCreatorType; /* obsolete */
  uint16_t wFinderFlags;  /* obsolete */
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_WIN32FINDDATA_H_ */
