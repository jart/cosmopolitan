#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_OPENFILENAME_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_OPENFILENAME_H_
#include "libc/nt/struct/editmenu.h"

struct NtOpenFilename {
  uint32_t lStructSize;
  int64_t hwndOwner;
  int64_t hInstance;
  const char16_t *lpstrFilter;
  char16_t *lpstrCustomFilter;
  uint32_t nMaxCustFilter;
  uint32_t nFilterIndex;
  char16_t *lpstrFile;
  uint32_t nMaxFile;
  char16_t *lpstrFileTitle;
  uint32_t nMaxFileTitle;
  const char16_t *lpstrInitialDir;
  const char16_t *lpstrTitle;
  uint32_t Flags;
  uint16_t nFileOffset;
  uint16_t nFileExtension;
  const char16_t *lpstrDefExt;
  intptr_t lCustData;
  uint64_t (*lpfnHook)(int64_t, uint32_t, uint64_t, int64_t);
  const char16_t *lpTemplateName;
  void *pvReserved;
  uint32_t dwReserved;
  uint32_t FlagsEx;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_OPENFILENAME_H_ */
