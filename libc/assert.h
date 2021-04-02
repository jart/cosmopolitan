#ifndef COSMOPOLITAN_LIBC_ASSERT_H_
#define COSMOPOLITAN_LIBC_ASSERT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void __assert_fail(const char *, const char *, int) hidden wontreturn relegated;

#ifdef NDEBUG
#define assert(EXPR) ((void)0)
#else
#define assert(EXPR) \
  ((void)((EXPR) || (__assert_fail(#EXPR, __FILE__, __LINE__), 0)))
#endif

#define static_assert _Static_assert

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ASSERT_H_ */
