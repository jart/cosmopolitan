#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_CONSOLESCREENBUFFERINFO_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_CONSOLESCREENBUFFERINFO_H_
#include "libc/nt/struct/coord.h"
#include "libc/nt/struct/smallrect.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtConsoleScreenBufferInfo {
  struct NtCoord dwSize;
  struct NtCoord dwCursorPosition;
  uint16_t wAttributes;
  struct NtSmallRect srWindow;
  struct NtCoord dwMaximumWindowSize;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_CONSOLESCREENBUFFERINFO_H_ */
