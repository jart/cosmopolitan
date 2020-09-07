#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_PTY_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_PTY_H_
#include "tool/build/lib/buffer.h"

#define kMachinePtyFg    0x01
#define kMachinePtyBg    0x02
#define kMachinePtyTrue  0x04
#define kMachinePtyBold  0x08
#define kMachinePtyFaint 0x10
#define kMachinePtyFlip  0x20

#define kMachinePtyNocursor 0x01

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct MachinePty {
  uint32_t yn;
  uint32_t xn;
  uint32_t y;
  uint32_t x;
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
};

void MachinePtyFree(struct MachinePty *);
struct MachinePty *MachinePtyNew(void) nodiscard;
void MachinePtyResize(struct MachinePty *, int, int);
ssize_t MachinePtyWrite(struct MachinePty *, const void *, size_t);
void MachinePtyAppendLine(struct MachinePty *, struct Buffer *, unsigned);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_PTY_H_ */
