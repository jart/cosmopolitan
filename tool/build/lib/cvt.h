#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_CVT_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_CVT_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define kOpCvt0f2a  0
#define kOpCvtt0f2c 4
#define kOpCvt0f2d  8
#define kOpCvt0f5a  12
#define kOpCvt0f5b  16
#define kOpCvt0fE6  20

void OpCvt(struct Machine *, unsigned long);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_CVT_H_ */
