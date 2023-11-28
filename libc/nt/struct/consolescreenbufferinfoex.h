#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_CONSOLESCREENBUFFERINFOEX_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_CONSOLESCREENBUFFERINFOEX_H_
#include "libc/nt/struct/coord.h"
#include "libc/nt/struct/smallrect.h"

struct NtConsoleScreenBufferInfoEx {
  uint32_t cbSize; /* sizeof(struct NtConsoleScreenBufferInfoEx) */
  struct NtCoord dwSize;
  struct NtCoord dwCursorPosition;
  uint16_t wAttributes; /* kNt{Foreground,Background}... */
  struct NtSmallRect srWindow;
  struct NtCoord dwMaximumWindowSize;
  uint16_t wPopupAttributes;
  bool32 bFullscreenSupported;
  uint32_t ColorTable[16]; /* 0x00BBGGRR */
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_CONSOLESCREENBUFFERINFOEX_H_ */
