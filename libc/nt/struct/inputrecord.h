#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_INPUTRECORD_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_INPUTRECORD_H_
#include "libc/nt/struct/coord.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)

struct NtKeyEventRecord {
  bool32 bKeyDown;
  uint16_t wRepeatCount;
  uint16_t wVirtualKeyCode;
  uint16_t wVirtualScanCode;
  union {
    int16_t UnicodeChar;
    char AsciiChar;
  } uChar;
  unsigned int dwControlKeyState;
};

struct NtMouseEventRecord {
  struct NtCoord dwMousePosition;
  uint32_t dwButtonState;
  uint32_t dwControlKeyState;
  uint32_t dwEventFlags;
};

struct NtWindowBufferSizeRecord {
  struct NtCoord dwSize;
};

struct NtMenuEventRecord {
  uint32_t dwCommandId;
};

struct NtFocusEventRecord {
  bool32 bSetFocus;
};

struct NtInputRecord {
  uint16_t EventType;
  union {
    struct NtKeyEventRecord KeyEvent;
    struct NtMouseEventRecord MouseEvent;
    struct NtWindowBufferSizeRecord WindowBufferSizeEvent;
    struct NtMenuEventRecord MenuEvent;
    struct NtFocusEventRecord FocusEvent;
  } Event;
};

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_INPUTRECORD_H_ */
