#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_HISTO_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_HISTO_H_
#include "libc/intrin/bsr.h"
#include "libc/macros.internal.h"
COSMOPOLITAN_C_START_

#define HISTO(H, X)                \
  do {                             \
    uint64_t x_ = X;               \
    x_ = x_ ? _bsrl(x_) + 1 : x_;  \
    ++H[MIN(x_, ARRAYLEN(H) - 1)]; \
  } while (0)

void PrintHistogram(int, const char *, const long *, size_t);
long GetLongSum(const long *, size_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_HISTO_H_ */
