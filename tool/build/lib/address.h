#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_ADDRESS_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_ADDRESS_H_
#include "libc/assert.h"
#include "third_party/xed/x86.h"
#include "tool/build/lib/machine.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

uint64_t AddressOb(struct Machine *, uint32_t);
uint64_t AddressDi(struct Machine *, uint32_t);
uint64_t AddressSi(struct Machine *, uint32_t);
uint64_t DataSegment(struct Machine *, uint32_t, uint64_t);
uint64_t AddSegment(struct Machine *, uint32_t, uint64_t, uint8_t[8]);
uint8_t *GetSegment(struct Machine *, uint32_t, int) nosideeffect;

forceinline uint64_t MaskAddress(uint32_t mode, uint64_t x) {
  if (mode != XED_MODE_LONG) {
    if (mode == XED_MODE_REAL) {
      x &= 0xffff;
    } else {
      x &= 0xffffffff;
    }
  }
  return x;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_ADDRESS_H_ */
