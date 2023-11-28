#ifndef COSMOPOLITAN_TOOL_VIZ_LIB_FORMATSTRINGTABLE_TESTLIB_H_
#define COSMOPOLITAN_TOOL_VIZ_LIB_FORMATSTRINGTABLE_TESTLIB_H_
#include "libc/macros.internal.h"
#include "libc/str/str.h"
#include "libc/testlib/testlib.h"
#include "tool/viz/lib/formatstringtable.h"

/**
 * Tests matrix contains expected content w/ string compare.
 *
 * @param M is 2D double array
 * @param WANT is a multi-line string of formatted matrix
 */
#define EXPECT_DBLMATRIXEQ(DIGS, RND, YN, XN, M, WANT, ...)               \
  EXPECT_MATRIXEQ_(__FILE__, __LINE__, __FUNCTION__, YN, XN, M, #M, WANT, \
                   StringifyMatrixDouble, NULL, NULL, NULL, DIGS, RND)

/**
 * Tests matrix contains expected content w/ string compare.
 *
 * @param M is 2D float array
 * @param WANT is a multi-line string of formatted matrix
 */
#define EXPECT_FLTMATRIXEQ(DIGS, RND, YN, XN, M, WANT, ...)               \
  EXPECT_MATRIXEQ_(__FILE__, __LINE__, __FUNCTION__, YN, XN, M, #M, WANT, \
                   StringifyMatrixFloat, NULL, NULL, NULL, DIGS, RND)

/**
 * Tests matrix contains expected content w/ string compare.
 *
 * @param M is 2D unsigned char array
 * @param WANT is a multi-line string of formatted matrix
 */
#define EXPECT_BYTEMATRIXEQ(YN, XN, XW, M, WANT, ...)                     \
  EXPECT_MATRIXEQ_(__FILE__, __LINE__, __FUNCTION__, YN, XN, M, #M, WANT, \
                   StringifyMatrixByte, NULL, NULL, NULL)

/**
 * Tests matrix contains expected content w/ string compare.
 *
 * @param M is 2D unsigned short array
 * @param WANT is a multi-line string of formatted matrix
 */
#define EXPECT_SHRTMATRIXEQ(YN, XN, M, WANT, ...)                         \
  EXPECT_MATRIXEQ_(__FILE__, __LINE__, __FUNCTION__, YN, XN, M, #M, WANT, \
                   StringifyMatrixShort, NULL, NULL, NULL)

#define EXPECT_MATRIXEQ_(FILE, LINE, FUNC, YN, XN, M, MC, WANT, F, ...) \
  do {                                                                  \
    char *Got;                                                          \
    const char *Want;                                                   \
    Want = (WANT);                                                      \
    Got = F(YN, XN, M, FormatStringTableForAssertion, __VA_ARGS__);     \
    if (testlib_strequals(sizeof(char), Want, Got)) {                   \
      testlib_free(Got);                                                \
    } else {                                                            \
      testlib_showerror_expect_matrixeq(                                \
          LINE, "...", MC, testlib_formatstr(sizeof(char), Want, -1),   \
          testlib_formatstr(sizeof(char), Got, -1), "");                \
    }                                                                   \
  } while (0)

void testlib_showerror_expect_matrixeq(int, const char *, const char *, char *,
                                       char *, const char *, ...);

#endif /* COSMOPOLITAN_TOOL_VIZ_LIB_FORMATSTRINGTABLE_TESTLIB_H_ */
