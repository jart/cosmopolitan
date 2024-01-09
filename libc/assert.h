#ifdef _ASSERT_H
#undef _ASSERT_H
#undef assert
#ifdef _COSMO_SOURCE
#undef unassert
#undef npassert
#ifndef NDEBUG
#undef __assert_macro
#endif /* NDEBUG */
#endif /* _COSMO_SOURCE */
#endif /* _ASSERT_H */

#ifndef _ASSERT_H
#define _ASSERT_H
COSMOPOLITAN_C_START_

void __assert_fail(const char *, const char *, int) libcesque;
void unassert(const char *, const char *, int) libcesque;

#ifdef NDEBUG
#define assert(x) ((void)0)
#else
#define assert(x) ((void)((x) || (__assert_fail(#x, __FILE__, __LINE__), 0)))
#endif

#if __STDC_VERSION__ >= 201112L && !defined(__cplusplus)
#undef static_assert
#define static_assert _Static_assert
#endif

#ifdef _COSMO_SOURCE
#ifndef NDEBUG
#define unassert(x) __assert_macro(x, #x)
#define npassert(x) __assert_macro(x, #x)
#define __assert_macro(x, s)             \
  ({                                     \
    if (__builtin_expect(!(x), 0)) {     \
      (unassert)(s, __FILE__, __LINE__); \
      __asm__("nop");                    \
      __builtin_unreachable();           \
    }                                    \
    (void)0;                             \
  })
#else
#define npassert(x)                  \
  ({                                 \
    if (__builtin_expect(!(x), 0)) { \
      __builtin_trap();              \
    }                                \
    (void)0;                         \
  })
#define unassert(x)                  \
  ({                                 \
    if (__builtin_expect(!(x), 0)) { \
      __builtin_unreachable();       \
    }                                \
    (void)0;                         \
  })
#endif /* NDEBUG */
#endif /* _COSMO_SOURCE */

COSMOPOLITAN_C_END_
#endif /* _ASSERT_H */
