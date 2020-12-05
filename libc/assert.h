#ifndef COSMOPOLITAN_LIBC_ASSERT_H_
#define COSMOPOLITAN_LIBC_ASSERT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void __assert_fail(const char *, const char *, int) hidden wontreturn relegated;

#ifdef NDEBUG
#define __ASSERT_FAIL(EXPR, FILE, LINE)
#else
#define __ASSERT_FAIL(EXPR, FILE, LINE) __assert_fail(EXPR, FILE, LINE)
#endif

#define assert(EXPR)                            \
  do {                                          \
    if (!(EXPR)) {                              \
      __ASSERT_FAIL(#EXPR, __FILE__, __LINE__); \
      unreachable;                              \
    }                                           \
  } while (0)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ASSERT_H_ */
