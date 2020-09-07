#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_WIN32FINDDATA_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_WIN32FINDDATA_H_
#include "libc/nt/struct/filetime.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

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
  uint32_t dwFileType;
  uint32_t dwCreatorType;
  uint16_t wFinderFlags;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_WIN32FINDDATA_H_ */
