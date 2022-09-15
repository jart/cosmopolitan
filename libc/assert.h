#ifndef COSMOPOLITAN_LIBC_ASSERT_H_
#define COSMOPOLITAN_LIBC_ASSERT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern bool __assert_disable;
void __assert_fail(const char *, const char *, int) hidden relegated;

#ifdef NDEBUG
#define assert(x) ((void)0)
#else
#define assert(x) ((void)((x) || (__assert_fail(#x, __FILE__, __LINE__), 0)))
#endif

#ifndef __cplusplus
#define static_assert _Static_assert
#endif

#define _unassert(x)                 \
  do {                               \
    if (__builtin_expect(!(x), 0)) { \
      unreachable;                   \
    }                                \
  } while (0)

#define _npassert(x)                 \
  do {                               \
    if (__builtin_expect(!(x), 0)) { \
      notpossible;                   \
    }                                \
  } while (0)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_ASSERT_H_ */
