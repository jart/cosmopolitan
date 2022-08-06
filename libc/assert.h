#ifndef COSMOPOLITAN_LIBC_ASSERT_H_
#define COSMOPOLITAN_LIBC_ASSERT_H_
#include "libc/bits/likely.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern bool __assert_disable;
void __assert_fail(const char *, const char *, int) hidden relegated;

#ifdef NDEBUG
#define assert(EXPR) ((void)0)
#else
#define assert(EXPR) \
  ((void)(LIKELY(EXPR) || (__assert_fail(#EXPR, __FILE__, __LINE__), 0)))
#endif

#ifndef __cplusplus
#define static_assert _Static_assert
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ASSERT_H_ */
