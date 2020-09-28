#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_PTY_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_PTY_H_
#include "tool/build/lib/buffer.h"

#define kMachinePtyFg    0x01
#define kMachinePtyBg    0x02
#define kMachinePtyTrue  0x04
#define kMachinePtyBold  0x08
#define kMachinePtyFaint 0x10
#define kMachinePtyFlip  0x20
#define kMachinePtyBlink 0x40

#define kMachinePtyNocursor    0x01
#define kMachinePtyBlinkcursor 0x02
#define kMachinePtyNocanon     0x04
#define kMachinePtyNoecho      0x08
#define kMachinePtyNoopost     0x10

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct MachinePty {
  int y;
  int x;
  int yn;
  int xn;
  uint32_t pr;
  uint32_t fg;
  uint32_t bg;
  uint32_t u8;
  uint32_t *wcs;
  uint32_t *fgs;
  uint32_t *bgs;
  uint32_t *prs;
  uint32_t conf;
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

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_PTY_H_ */
