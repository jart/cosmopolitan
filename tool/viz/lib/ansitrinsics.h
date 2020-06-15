#ifndef COSMOPOLITAN_TOOL_VIZ_LIB_ANSITRINSICS_H_
#define COSMOPOLITAN_TOOL_VIZ_LIB_ANSITRINSICS_H_
#include "libc/limits.h"
#include "libc/macros.h"
#include "libc/str/str.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

static unsigned char avgb(unsigned char a, unsigned char b) {
  return (a + b + 1) / 2;
}

static void pavgb(const unsigned char *restrict c,
                  const unsigned char *restrict b, unsigned char *restrict a) {
  unsigned i;
  for (i = 0; i < 16; ++i) a[i] = avgb(c[i], b[i]);
}

static void vpavgb(const unsigned char *restrict c,
                   const unsigned char *restrict b, unsigned char *restrict a) {
  unsigned i;
  for (i = 0; i < 32; ++i) a[i] = avgb(c[i], b[i]);
}

static void paddsw(const short c[8], const short b[8], short a[8]) {
  size_t j;
  for (j = 0; j < 8; ++j) {
    a[j] = MIN(SHRT_MAX, MAX(SHRT_MIN, b[j] + c[j]));
  }
}

static void vpaddsw(const short c[16], const short b[16], short a[16]) {
  size_t j;
  for (j = 0; j < 16; ++j) {
    a[j] = MAX(SHRT_MIN, MIN(SHRT_MAX, b[j] + c[j]));
  }
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_VIZ_LIB_ANSITRINSICS_H_ */
