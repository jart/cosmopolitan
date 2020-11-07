#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_XMMTYPE_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_XMMTYPE_H_
#include "tool/build/lib/machine.h"

#define kXmmIntegral 0
#define kXmmDouble   1
#define kXmmFloat    2

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct XmmType {
  uint8_t type[16];
  uint8_t size[16];
};

void UpdateXmmType(struct Machine *, struct XmmType *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_XMMTYPE_H_ */
