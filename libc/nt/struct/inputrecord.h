#ifndef COSMOPOLITAN_LIBC_NT_STRUCT_INPUTRECORD_H_
#define COSMOPOLITAN_LIBC_NT_STRUCT_INPUTRECORD_H_
#include "libc/nt/struct/coord.h"

struct NtKeyEventRecord {
  bool32 bKeyDown;
  uint16_t wRepeatCount;
  uint16_t wVirtualKeyCode;
  uint16_t wVirtualScanCode;
  union {
    uint16_t UnicodeChar;
    char AsciiChar;
  } uChar;
  unsigned int dwControlKeyState;
#define kNtRightAltPressed  0x0001
#define kNtLeftAltPressed   0x0002
#define kNtRightCtrlPressed 0x0004
#define kNtLeftCtrlPressed  0x0008
#define kNtShiftPressed     0x0010
#define kNtNumlockOn        0x0020
#define kNtScrolllockOn     0x0040
#define kNtCapslockOn       0x0080
#define kNtEnhancedKey      0x0100
};

struct NtMouseEventRecord {
  struct NtCoord dwMousePosition;
  uint32_t dwButtonState;
#define kNtFromLeft1stButtonPressed 0x0001
#define kNtRightmostButtonPressed   0x0002
#define kNtFromLeft2ndButtonPressed 0x0004
#define kNtFromLeft3rdButtonPressed 0x0008
#define kNtFromLeft4thButtonPressed 0x0010
  uint32_t dwControlKeyState;
  uint32_t dwEventFlags;
#define kNtMouseMoved    0x0001
#define kNtDoubleClick   0x0002
#define kNtMouseWheeled  0x0004
#define kNtMouseHwheeled 0x0008
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
#define kNtKeyEvent              0x0001
#define kNtMouseEvent            0x0002
#define kNtWindowBufferSizeEvent 0x0004
#define kNtMenuEvent             0x0008
#define kNtFocusEvent            0x0010
  union {
    struct NtKeyEventRecord KeyEvent;
    struct NtMouseEventRecord MouseEvent;
    struct NtWindowBufferSizeRecord WindowBufferSizeEvent;
    struct NtMenuEventRecord MenuEvent;
    struct NtFocusEventRecord FocusEvent;
  } Event;
};

#endif /* COSMOPOLITAN_LIBC_NT_STRUCT_INPUTRECORD_H_ */
