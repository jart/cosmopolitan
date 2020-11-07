#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_PML4T_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_PML4T_H_
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define IsValidPage(x)    ((x)&1)
#define MaskPageAddr(x)   ((x)&0x00007ffffffff000)
#define UnmaskPageAddr(x) SignExtendAddr(MaskPageAddr(x))
#define SignExtendAddr(x) ((int64_t)((uint64_t)(x) << 16) >> 16)

struct ContiguousMemoryRanges {
  size_t i, n;
  struct ContiguousMemoryRange {
    int64_t a;
    int64_t b;
  } * p;
};

void FindContiguousMemoryRanges(struct Machine *,
                                struct ContiguousMemoryRanges *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_PML4T_H_ */
