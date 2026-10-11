#ifndef COSMOPOLITAN_TOOL_PLINKO_LIB_HISTO_H_
#define COSMOPOLITAN_TOOL_PLINKO_LIB_HISTO_H_
#include <inttypes.h>
#include <stddef.h>
#include <wchar.h>

#define MIN_(X, Y) ((Y) > (X) ? (X) : (Y))
#define ARRAYLEN(A) \
  ((sizeof(A) / sizeof(*(A))) / ((unsigned)!(sizeof(A) % sizeof(*(A)))))

int bsrl(uint_least64_t);

#define HISTO(H, X)                 \
  do {                              \
    uint64_t x_ = X;                \
    x_ = x_ ? bsrl(x_) + 1 : x_;    \
    ++H[MIN_(x_, ARRAYLEN(H) - 1)]; \
  } while (0)

void PrintHistogram(int, const wchar_t *, const long *, size_t);
long GetLongSum(const long *, size_t);

#endif /* COSMOPOLITAN_TOOL_PLINKO_LIB_HISTO_H_ */
