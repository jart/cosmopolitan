#ifndef COSMOPOLITAN_LIBC_STR_SLICE_H_
#define COSMOPOLITAN_LIBC_STR_SLICE_H_
#include "libc/str/str.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define SlicesEqual(a, n, b, m)       \
  ({                                  \
    size_t __n = (n);                 \
    __n == (m) && !memcmp(a, b, __n); \
  })

#define SlicesEqualCase(a, n, b, m)       \
  ({                                      \
    size_t __n = (n);                     \
    __n == (m) && !memcasecmp(a, b, __n); \
  })

int CompareSlices(const char *, size_t, const char *, size_t);
int CompareSlicesCase(const char *, size_t, const char *, size_t);
bool StartsWithIgnoreCase(const char *, const char *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_SLICE_H_ */
