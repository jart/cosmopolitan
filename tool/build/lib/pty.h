#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_PTY_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_PTY_H_
#include "tool/build/lib/buffer.h"

#define kMachinePtyFg      0x0001
#define kMachinePtyBg      0x0002
#define kMachinePtyBold    0x0004
#define kMachinePtyFlip    0x0008
#define kMachinePtyFaint   0x0010
#define kMachinePtyUnder   0x0020
#define kMachinePtyDunder  0x0040
#define kMachinePtyTrue    0x0080
#define kMachinePtyBlink   0x0100
#define kMachinePtyItalic  0x0200
#define kMachinePtyFraktur 0x0400
#define kMachinePtyStrike  0x0800
#define kMachinePtyConceal 0x1000

#define kMachinePtyBell        0x001
#define kMachinePtyRedzone     0x002
#define kMachinePtyNocursor    0x004
#define kMachinePtyBlinkcursor 0x008
#define kMachinePtyNocanon     0x010
#define kMachinePtyNoecho      0x020
#define kMachinePtyNoopost     0x040
#define kMachinePtyLed1        0x080
#define kMachinePtyLed2        0x100
#define kMachinePtyLed3        0x200
#define kMachinePtyLed4        0x400

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct MachinePty {
  int y;
  int x;
  int yn;
  int xn;
  uint32_t u8;
  uint32_t n8;
  uint32_t pr;
  uint32_t fg;
  uint32_t bg;
  uint32_t conf;
  uint32_t save;
  uint32_t *wcs;
  uint32_t *prs;
  uint32_t *fgs;
  uint32_t *bgs;
  wchar_t *xlat;
  enum MachinePtyState {
    kMachinePtyAscii,
    kMachinePtyUtf8,
    kMachinePtyEsc,
    kMachinePtyCsi,
  } state;
  struct MachinePtyEsc {
    unsigned i;
    char s[64];
  } esc;
  struct MachinePtyInput {
    size_t i, n;
    char *p;
  } input;
};

void MachinePtyFree(struct MachinePty *);
struct MachinePty *MachinePtyNew(void) nodiscard;
void MachinePtyResize(struct MachinePty *, int, int);
ssize_t MachinePtyRead(struct MachinePty *, void *, size_t);
ssize_t MachinePtyWrite(struct MachinePty *, const void *, size_t);
ssize_t MachinePtyWriteInput(struct MachinePty *, const void *, size_t);
void MachinePtyAppendLine(struct MachinePty *, struct Buffer *, unsigned);
void MachinePtyFullReset(struct MachinePty *);
void MachinePtyMemmove(struct MachinePty *, long, long, long);
void MachinePtyErase(struct MachinePty *, long, long);
void MachinePtySetY(struct MachinePty *, int);
void MachinePtySetX(struct MachinePty *, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_PTY_H_ */
