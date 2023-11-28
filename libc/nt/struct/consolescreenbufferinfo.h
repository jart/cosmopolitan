#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_CONSOLESCREENBUFFERINFO_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_CONSOLESCREENBUFFERINFO_H_
#include "libc/nt/struct/coord.h"
#include "libc/nt/struct/smallrect.h"

struct NtConsoleScreenBufferInfo {
  struct NtCoord dwSize;
  struct NtCoord dwCursorPosition;
  uint16_t wAttributes;
  struct NtSmallRect srWindow;
  struct NtCoord dwMaximumWindowSize;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_CONSOLESCREENBUFFERINFO_H_ */
