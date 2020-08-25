#ifndef COSMOPOLITAN_TEST_TOOL_BUILD_LIB_OPTEST_H_
#define COSMOPOLITAN_TEST_TOOL_BUILD_LIB_OPTEST_H_
#include "tool/build/lib/flags.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define FMASK (1 << FLAGS_CF | 1 << FLAGS_ZF | 1 << FLAGS_SF | 1 << FLAGS_OF)

void RunOpTests(const uint8_t *, size_t, const char *const *, const char *, int,
                const char *);

int64_t RunGolden(char, int, uint64_t, uint64_t, uint32_t *);
int64_t RunOpTest(char, int, uint64_t, uint64_t, uint32_t *);
void FixupUndefOpTestFlags(char, int, uint64_t, uint64_t, uint32_t, uint32_t *);

#define RunOpTests(ops, n, names) \
  RunOpTests(ops, n, names, __FILE__, __LINE__, __func__)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TEST_TOOL_BUILD_LIB_OPTEST_H_ */
