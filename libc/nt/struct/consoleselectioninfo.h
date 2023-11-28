#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_CONSOLESELECTIONINFO_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_CONSOLESELECTIONINFO_H_
#include "libc/nt/struct/coord.h"
#include "libc/nt/struct/smallrect.h"

struct NtConsoleSelectionInfo {
  uint32_t dwFlags;
  struct NtCoord dwSelectionAnchor;
  struct NtSmallRect srSelection;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_CONSOLESELECTIONINFO_H_ */
