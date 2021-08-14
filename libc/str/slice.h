#ifndef COSMOPOLITAN_LIBC_STR_SLICE_H_
#define COSMOPOLITAN_LIBC_STR_SLICE_H_
#include "libc/str/str.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

forceinline bool SlicesEqual(const char *a, size_t n, const char *b, size_t m) {
  return n == m && !memcmp(a, b, n);
}

forceinline bool SlicesEqualCase(const void *a, size_t n, const void *b,
                                 size_t m) {
  return n == m && !memcasecmp(a, b, n);
}

int CompareSlices(const char *, size_t, const char *, size_t);
int CompareSlicesCase(const char *, size_t, const char *, size_t);
bool StartsWithIgnoreCase(const char *, const char *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_SLICE_H_ */
