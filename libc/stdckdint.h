#ifndef COSMOPOLITAN_LIBC_STDCKDINT_H_
#define COSMOPOLITAN_LIBC_STDCKDINT_H_
/* clang-format off */

/**
 * @fileoverview C23 Checked Arithmetic
 *
 * This header defines three type generic functions:
 *
 *   - `bool ckd_add(res, a, b)`
 *   - `bool ckd_sub(res, a, b)`
 *   - `bool ckd_mul(res, a, b)`
 *
 * Which allow integer arithmetic errors to be detected. There are many
 * kinds of integer errors, e.g. overflow, truncation, etc. These funcs
 * catch them all. Here's an example of how it works:
 *
 *     uint32_t c;
 *     int32_t a = 0x7fffffff;
 *     int32_t b = 2;
 *     assert(!ckd_add(&c, a, b));
 *     assert(c == 0x80000001u);
 *
 * Experienced C / C++ users should find this example counter-intuitive
 * because the expression `0x7fffffff + 2` not only overflows it's also
 * undefined behavior. However here we see it's specified, and does not
 * result in an error. That's because C23 checked arithmetic is not the
 * arithmetic you're used to. The new standard changes the mathematics.
 *
 * C23 checked arithmetic is defined as performing the arithmetic using
 * infinite precision and then checking if the resulting value will fit
 * in the output type. Our example above did not result in an error due
 * to `0x80000001` being a legal value for `uint32_t`.
 *
 * This implementation will use the GNU compiler builtins, when they're
 * available, only if you don't use build flags like `-std=c11` because
 * they define `__STRICT_ANSI__` and GCC extensions aren't really ANSI.
 * Instead, you'll get a pretty good pure C11 and C++11 implementation.
 *
 * @see https://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf
 * @version 0.1 (2023-07-22)
 */

#define __STDC_VERSION_STDCKDINT_H__ 202311L

#if ((defined(__llvm__) ||                                              \
      (defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 406)) && \
     !defined(__STRICT_ANSI__))
#define __ckd_have_int128
#define __ckd_intmax __int128
#elif ((defined(__cplusplus) && __cplusplus >= 201103L) ||              \
       (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L))
#define __ckd_intmax long long
#else
#define __ckd_intmax long
#endif

typedef signed __ckd_intmax __ckd_intmax_t;
typedef unsigned __ckd_intmax __ckd_uintmax_t;

#if (!defined(__STRICT_ANSI__) &&                       \
     ((defined(__GNUC__) && __GNUC__ >= 5 &&            \
       !defined(__chibicc__) && !defined(__ICC)) ||     \
      (__has_builtin(__builtin_add_overflow) &&         \
       __has_builtin(__builtin_sub_overflow) &&         \
       __has_builtin(__builtin_mul_overflow))))
#define ckd_add(res, x, y) __builtin_add_overflow((x), (y), (res))
#define ckd_sub(res, x, y) __builtin_sub_overflow((x), (y), (res))
#define ckd_mul(res, x, y) __builtin_mul_overflow((x), (y), (res))

#elif defined(__cplusplus) && __cplusplus >= 201103L
#include "third_party/libcxx/type_traits"
#include "third_party/libcxx/limits"

template <typename __T, typename __U, typename __V>
inline bool ckd_add(__T *__res, __U __a, __V __b) {
  static_assert(std::is_integral<__T>::value &&
                std::is_integral<__U>::value &&
                std::is_integral<__V>::value,
                "non-integral types not allowed");
  static_assert(!std::is_same<__T, bool>::value &&
                !std::is_same<__U, bool>::value &&
                !std::is_same<__V, bool>::value,
                "checked booleans not supported");
  static_assert(!std::is_same<__T, char>::value &&
                !std::is_same<__U, char>::value &&
                !std::is_same<__V, char>::value,
                "unqualified char type is ambiguous");
  __ckd_uintmax_t __x = __a;
  __ckd_uintmax_t __y = __b;
  __ckd_uintmax_t __z = __x + __y;
  *__res = __z;
  if (sizeof(__z) > sizeof(__U) && sizeof(__z) > sizeof(__V)) {
    if (sizeof(__z) > sizeof(__T) || std::is_signed<__T>::value) {
      return static_cast<__ckd_intmax_t>(__z) != static_cast<__T>(__z);
    } else if (!std::is_same<__T, __ckd_uintmax_t>::value) {
      return (__z != static_cast<__T>(__z) ||
              ((std::is_signed<__U>::value ||
                std::is_signed<__V>::value) &&
               static_cast<__ckd_intmax_t>(__z) < 0));
    }
  }
  bool __truncated = false;
  if (sizeof(__T) < sizeof(__ckd_intmax_t)) {
    __truncated = __z != static_cast<__ckd_uintmax_t>(static_cast<__T>(__z));
  }
  switch (std::is_signed<__T>::value << 2 |  //
          std::is_signed<__U>::value << 1 |  //
          std::is_signed<__V>::value) {
    case 0:  // u = u + u
      return __truncated | (__z < __x);
    case 1:  // u = u + s
      __y ^= std::numeric_limits<__ckd_intmax_t>::min();
      return __truncated |
          (static_cast<__ckd_intmax_t>((__z ^ __x) &
                                       (__z ^ __y)) < 0);
    case 2:  // u = s + u
      __x ^= std::numeric_limits<__ckd_intmax_t>::min();
      return __truncated |
          (static_cast<__ckd_intmax_t>((__z ^ __x) &
                                       (__z ^ __y)) < 0);
    case 3:  // u = s + s
      return __truncated |
          (static_cast<__ckd_intmax_t>(((__z | __x) &  __y) |
                                       ((__z & __x) & ~__y)) < 0);
    case 4:  // s = u + u
      return __truncated | (__z < __x) | (static_cast<__ckd_intmax_t>(__z) < 0);
    case 5:  // s = u + s
      __y ^= std::numeric_limits<__ckd_intmax_t>::min();
      return __truncated | (__x + __y < __y);
    case 6:  // s = s + u
      __x ^= std::numeric_limits<__ckd_intmax_t>::min();
      return __truncated | (__x + __y < __x);
    case 7:  // s = s + s
      return __truncated |
          (static_cast<__ckd_intmax_t>((__z ^ __x) &
                                       (__z ^ __y)) < 0);
    default:
      for (;;) (void)0;
  }
}

template <typename __T, typename __U, typename __V>
inline bool ckd_sub(__T *__res, __U __a, __V __b) {
  static_assert(std::is_integral<__T>::value &&
                std::is_integral<__U>::value &&
                std::is_integral<__V>::value,
                "non-integral types not allowed");
  static_assert(!std::is_same<__T, bool>::value &&
                !std::is_same<__U, bool>::value &&
                !std::is_same<__V, bool>::value,
                "checked booleans not supported");
  static_assert(!std::is_same<__T, char>::value &&
                !std::is_same<__U, char>::value &&
                !std::is_same<__V, char>::value,
                "unqualified char type is ambiguous");
  __ckd_uintmax_t __x = __a;
  __ckd_uintmax_t __y = __b;
  __ckd_uintmax_t __z = __x - __y;
  *__res = __z;
  if (sizeof(__z) > sizeof(__U) && sizeof(__z) > sizeof(__V)) {
    if (sizeof(__z) > sizeof(__T) || std::is_signed<__T>::value) {
      return static_cast<__ckd_intmax_t>(__z) != static_cast<__T>(__z);
    } else if (!std::is_same<__T, __ckd_uintmax_t>::value) {
      return (__z != static_cast<__T>(__z) ||
              ((std::is_signed<__U>::value ||
                std::is_signed<__V>::value) &&
               static_cast<__ckd_intmax_t>(__z) < 0));
    }
  }
  bool __truncated = false;
  if (sizeof(__T) < sizeof(__ckd_intmax_t)) {
    __truncated = __z != static_cast<__ckd_uintmax_t>(static_cast<__T>(__z));
  }
  switch (std::is_signed<__T>::value << 2 |  //
          std::is_signed<__U>::value << 1 |  //
          std::is_signed<__V>::value) {
    case 0:  // u = u - u
      return __truncated | (__x < __y);
    case 1:  // u = u - s
      __y ^= std::numeric_limits<__ckd_intmax_t>::min();
      return __truncated |
          (static_cast<__ckd_intmax_t>((__x ^ __y) &
                                       (__z ^ __x)) < 0);
    case 2:  // u = s - u
      return __truncated | (__y > __x) | (static_cast<__ckd_intmax_t>(__x) < 0);
    case 3:  // u = s - s
      return __truncated |
          (static_cast<__ckd_intmax_t>(((__z & __x) &  __y) |
                                       ((__z | __x) & ~__y)) < 0);
    case 4:  // s = u - u
      return __truncated |
          ((__x < __y) ^ (static_cast<__ckd_intmax_t>(__z) < 0));
    case 5:  // s = u - s
      __y ^= std::numeric_limits<__ckd_intmax_t>::min();
      return __truncated | (__x >= __y);
    case 6:  // s = s - u
      __x ^= std::numeric_limits<__ckd_intmax_t>::min();
      return __truncated | (__x < __y);
    case 7:  // s = s - s
      return __truncated |
          (static_cast<__ckd_intmax_t>((__x ^ __y) &
                                       (__z ^ __x)) < 0);
    default:
      for (;;) (void)0;
  }
}

template <typename __T, typename __U, typename __V>
inline bool ckd_mul(__T *__res, __U __a, __V __b) {
  static_assert(std::is_integral<__T>::value &&
                std::is_integral<__U>::value &&
                std::is_integral<__V>::value,
                "non-integral types not allowed");
  static_assert(!std::is_same<__T, bool>::value &&
                !std::is_same<__U, bool>::value &&
                !std::is_same<__V, bool>::value,
                "checked booleans not supported");
  static_assert(!std::is_same<__T, char>::value &&
                !std::is_same<__U, char>::value &&
                !std::is_same<__V, char>::value,
                "unqualified char type is ambiguous");
  __ckd_uintmax_t __x = __a;
  __ckd_uintmax_t __y = __b;
  if ((sizeof(__U) * 8 - std::is_signed<__U>::value) +
      (sizeof(__V) * 8 - std::is_signed<__V>::value) <=
      (sizeof(__T) * 8 - std::is_signed<__T>::value)) {
    if (sizeof(__ckd_uintmax_t) > sizeof(__T) || std::is_signed<__T>::value) {
      __ckd_intmax_t __z = __x * __y;
      return __z != (*__res = __z);
    } else if (!std::is_same<__T, __ckd_uintmax_t>::value) {
      __ckd_uintmax_t __z = __x * __y;
      *__res = __z;
      return (__z != static_cast<__T>(__z) ||
              ((std::is_signed<__U>::value ||
                std::is_signed<__V>::value) &&
               static_cast<__ckd_intmax_t>(__z) < 0));
    }
  }
  switch (std::is_signed<__T>::value << 2 |  //
          std::is_signed<__U>::value << 1 |  //
          std::is_signed<__V>::value) {
    case 0: {  // u = u * u
      __ckd_uintmax_t __z = __x * __y;
      int __o = __x && __z / __x != __y;
      *__res = __z;
      return __o | (sizeof(__T) < sizeof(__z) &&
                    __z != static_cast<__ckd_uintmax_t>(*__res));
    }
    case 1: {  // u = u * s
      __ckd_uintmax_t __z = __x * __y;
      int __o = __x && __z / __x != __y;
      *__res = __z;
      return (__o | ((static_cast<__ckd_intmax_t>(__y) < 0) & !!__x) |
              (sizeof(__T) < sizeof(__z) &&
               __z != static_cast<__ckd_uintmax_t>(*__res)));
    }
    case 2: {  // u = s * u
      __ckd_uintmax_t __z = __x * __y;
      int __o = __x && __z / __x != __y;
      *__res = __z;
      return (__o | ((static_cast<__ckd_intmax_t>(__x) < 0) & !!__y) |
              (sizeof(__T) < sizeof(__z) &&
               __z != static_cast<__ckd_uintmax_t>(*__res)));
    }
    case 3: { // u = s * s
      int __o = false;
      if (static_cast<__ckd_intmax_t>(__x & __y) < 0) {
        __x = -__x;
        __y = -__y;
      } else if (static_cast<__ckd_intmax_t>(__x ^ __y) < 0) {
        __o = __x && __y;
      }
      __ckd_uintmax_t __z = __x * __y;
      __o |= __x && __z / __x != __y;
      *__res = __z;
      return __o | (sizeof(__T) < sizeof(__z) &&
                    __z != static_cast<__ckd_uintmax_t>(*__res));
    }
    case 4: {  // s = u * u
      __ckd_uintmax_t __z = __x * __y;
      int __o = __x && __z / __x != __y;
      *__res = __z;
      return (__o | (static_cast<__ckd_intmax_t>(__z) < 0) |
              (sizeof(__T) < sizeof(__z) &&
               __z != static_cast<__ckd_uintmax_t>(*__res)));
    }
    case 5: {  // s = u * s
      __ckd_uintmax_t __t = -__y;
      __t = static_cast<__ckd_intmax_t>(__t) < 0 ? __y : __t;
      __ckd_uintmax_t __p = __t * __x;
      int __o = __t && __p / __t != __x;
      int __n = static_cast<__ckd_intmax_t>(__y) < 0;
      __ckd_uintmax_t __z = __n ? -__p : __p;
      *__res = __z;
      __ckd_uintmax_t __m = std::numeric_limits<__ckd_intmax_t>::max();
      return (__o | (__p > __m + __n) |
              (sizeof(__T) < sizeof(__z) &&
               __z != static_cast<__ckd_uintmax_t>(*__res)));
    }
    case 6: {  // s = s * u
      __ckd_uintmax_t __t = -__x;
      __t = static_cast<__ckd_intmax_t>(__t) < 0 ? __x : __t;
      __ckd_uintmax_t __p = __t * __y;
      int __o = __t && __p / __t != __y;
      int __n = static_cast<__ckd_intmax_t>(__x) < 0;
      __ckd_uintmax_t __z = __n ? -__p : __p;
      *__res = __z;
      __ckd_uintmax_t __m = std::numeric_limits<__ckd_intmax_t>::max();
      return (__o | (__p > __m + __n) |
              (sizeof(__T) < sizeof(__z) &&
               __z != static_cast<__ckd_uintmax_t>(*__res)));
    }
    case 7: {  // s = s * s
      __ckd_uintmax_t __z = __x * __y;
      *__res = __z;
      return ((((static_cast<__ckd_intmax_t>(__y) < 0) &&
                (static_cast<__ckd_intmax_t>(__x) ==
                 std::numeric_limits<__ckd_intmax_t>::min())) ||
               (__y && ((static_cast<__ckd_intmax_t>(__z) /
                         static_cast<__ckd_intmax_t>(__y)) !=
                        static_cast<__ckd_intmax_t>(__x)))) |
              (sizeof(__T) < sizeof(__z) &&
               __z != static_cast<__ckd_uintmax_t>(*__res)));
    }
    default:
      for (;;) (void)0;
  }
}

#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L

#define ckd_add(res, a, b) __ckd_expr(add, (res), (a), (b))
#define ckd_sub(res, a, b) __ckd_expr(sub, (res), (a), (b))
#define ckd_mul(res, a, b) __ckd_expr(mul, (res), (a), (b))

#if defined(__GNUC__) || defined(__llvm__)
#define __ckd_inline                                    \
  extern __inline __attribute__((__gnu_inline__,        \
                                 __always_inline__,     \
                                 __artificial__))
#else
#define __ckd_inline static inline
#endif

#ifdef __ckd_have_int128
#define __ckd_generic_int128(x, y) , signed __int128: x, unsigned __int128: y
#else
#define __ckd_generic_int128(x, y)
#endif

#define __ckd_sign(T)                           \
  ((T)1 << (sizeof(T) * 8 - 1))

#define __ckd_is_signed(x)                      \
  _Generic(x,                                   \
           signed char: 1,                      \
           unsigned char: 0,                    \
           signed short: 1,                     \
           unsigned short: 0,                   \
           signed int: 1,                       \
           unsigned int: 0,                     \
           signed long: 1,                      \
           unsigned long: 0,                    \
           signed long long: 1,                 \
           unsigned long long: 0                \
           __ckd_generic_int128(1, 0))

#define __ckd_expr(op, res, a, b)                       \
  (_Generic(*res,                                       \
            signed char: __ckd_##op##_schar,            \
            unsigned char: __ckd_##op##_uchar,          \
            signed short: __ckd_##op##_sshort,          \
            unsigned short: __ckd_##op##_ushort,        \
            signed int: __ckd_##op##_sint,              \
            unsigned int: __ckd_##op##_uint,            \
            signed long: __ckd_##op##_slong,            \
            unsigned long: __ckd_##op##_ulong,          \
            signed long long: __ckd_##op##_slonger,     \
            unsigned long long: __ckd_##op##_ulonger    \
            __ckd_generic_int128(                       \
                __ckd_##op##_sint128,                   \
                __ckd_##op##_uint128))(                 \
                    res, a, b,                          \
                    __ckd_is_signed(a),                 \
                    __ckd_is_signed(b)))

#define __ckd_declare_add(S, T)                                         \
  __ckd_inline char S(void *__res,                                      \
                      __ckd_uintmax_t __x,                              \
                      __ckd_uintmax_t __y,                              \
                      char __a_signed,                                  \
                      char __b_signed) {                                \
    __ckd_uintmax_t __z = __x + __y;                                    \
    *(T *)__res = __z;                                                  \
    char __truncated = 0;                                               \
    if (sizeof(T) < sizeof(__ckd_intmax_t)) {                           \
      __truncated = __z != (__ckd_uintmax_t)(T)__z;                     \
    }                                                                   \
    switch (__ckd_is_signed((T)0) << 2 |                                \
            __a_signed << 1 | __b_signed) {                             \
      case 0:  /* u = u + u */                                          \
        return __truncated | (__z < __x);                               \
      case 1:  /* u = u + s */                                          \
        __y ^= __ckd_sign(__ckd_uintmax_t);                             \
        return __truncated |                                            \
            ((__ckd_intmax_t)((__z ^ __x) &                             \
                              (__z ^ __y)) < 0);                        \
      case 2:  /* u = s + u */                                          \
        __x ^= __ckd_sign(__ckd_uintmax_t);                             \
        return __truncated |                                            \
            ((__ckd_intmax_t)((__z ^ __x) &                             \
                              (__z ^ __y)) < 0);                        \
      case 3:  /* u = s + s */                                          \
        return __truncated |                                            \
            ((__ckd_intmax_t)(((__z | __x) &  __y) |                    \
                              ((__z & __x) & ~__y)) < 0);               \
      case 4:  /* s = u + u */                                          \
        return __truncated | (__z < __x) | ((__ckd_intmax_t)__z < 0);   \
      case 5:  /* s = u + s */                                          \
        __y ^= __ckd_sign(__ckd_uintmax_t);                             \
        return __truncated | (__x + __y < __y);                         \
      case 6:  /* s = s + u */                                          \
        __x ^= __ckd_sign(__ckd_uintmax_t);                             \
        return __truncated | (__x + __y < __x);                         \
      case 7:  /* s = s + s */                                          \
        return __truncated |                                            \
            ((__ckd_intmax_t)((__z ^ __x) &                             \
                              (__z ^ __y)) < 0);                        \
      default:                                                          \
        for (;;) (void)0;                                               \
    }                                                                   \
  }

__ckd_declare_add(__ckd_add_schar, signed char)
__ckd_declare_add(__ckd_add_uchar, unsigned char)
__ckd_declare_add(__ckd_add_sshort, signed short)
__ckd_declare_add(__ckd_add_ushort, unsigned short)
__ckd_declare_add(__ckd_add_sint, signed int)
__ckd_declare_add(__ckd_add_uint, unsigned int)
__ckd_declare_add(__ckd_add_slong, signed long)
__ckd_declare_add(__ckd_add_ulong, unsigned long)
__ckd_declare_add(__ckd_add_slonger, signed long long)
__ckd_declare_add(__ckd_add_ulonger, unsigned long long)
#ifdef __ckd_have_int128
__ckd_declare_add(__ckd_add_sint128, signed __int128)
__ckd_declare_add(__ckd_add_uint128, unsigned __int128)
#endif

#define __ckd_declare_sub(S, T)                                         \
  __ckd_inline char S(void *__res,                                      \
                      __ckd_uintmax_t __x,                              \
                      __ckd_uintmax_t __y,                              \
                      char __a_signed,                                  \
                      char __b_signed) {                                \
    __ckd_uintmax_t __z = __x - __y;                                    \
    *(T *)__res = __z;                                                  \
    char __truncated = 0;                                               \
    if (sizeof(T) < sizeof(__ckd_intmax_t)) {                           \
      __truncated = __z != (__ckd_uintmax_t)(T)__z;                     \
    }                                                                   \
    switch (__ckd_is_signed((T)0) << 2 |                                \
            __a_signed << 1 | __b_signed) {                             \
      case 0:  /* u = u - u */                                          \
        return __truncated | (__x < __y);                               \
      case 1:  /* u = u - s */                                          \
        __y ^= __ckd_sign(__ckd_uintmax_t);                             \
        return __truncated |                                            \
            ((__ckd_intmax_t)((__x ^ __y) &                             \
                              (__z ^ __x)) < 0);                        \
      case 2:  /* u = s - u */                                          \
        return __truncated | (__y > __x) | ((__ckd_intmax_t)__x < 0);   \
      case 3:  /* u = s - s */                                          \
        return __truncated |                                            \
            ((__ckd_intmax_t)(((__z & __x) &  __y) |                    \
                              ((__z | __x) & ~__y)) < 0);               \
      case 4:  /* s = u - u */                                          \
        return __truncated | ((__x < __y) ^ ((__ckd_intmax_t)__z < 0)); \
      case 5:  /* s = u - s */                                          \
        __y ^= __ckd_sign(__ckd_uintmax_t);                             \
        return __truncated | (__x >= __y);                              \
      case 6:  /* s = s - u */                                          \
        __x ^= __ckd_sign(__ckd_uintmax_t);                             \
        return __truncated | (__x < __y);                               \
      case 7:  /* s = s - s */                                          \
        return __truncated |                                            \
            ((__ckd_intmax_t)((__x ^ __y) &                             \
                              (__z ^ __x)) < 0);                        \
      default:                                                          \
        for (;;) (void)0;                                               \
    }                                                                   \
  }

__ckd_declare_sub(__ckd_sub_schar, signed char)
__ckd_declare_sub(__ckd_sub_uchar, unsigned char)
__ckd_declare_sub(__ckd_sub_sshort, signed short)
__ckd_declare_sub(__ckd_sub_ushort, unsigned short)
__ckd_declare_sub(__ckd_sub_sint, signed int)
__ckd_declare_sub(__ckd_sub_uint, unsigned int)
__ckd_declare_sub(__ckd_sub_slong, signed long)
__ckd_declare_sub(__ckd_sub_ulong, unsigned long)
__ckd_declare_sub(__ckd_sub_slonger, signed long long)
__ckd_declare_sub(__ckd_sub_ulonger, unsigned long long)
#ifdef __ckd_have_int128
__ckd_declare_sub(__ckd_sub_sint128, signed __int128)
__ckd_declare_sub(__ckd_sub_uint128, unsigned __int128)
#endif

#define __ckd_declare_mul(S, T)                                 \
  __ckd_inline char S(void *__res,                              \
                      __ckd_uintmax_t __x,                      \
                      __ckd_uintmax_t __y,                      \
                      char __a_signed,                          \
                      char __b_signed) {                        \
    switch (__ckd_is_signed((T)0) << 2 |                        \
            __a_signed << 1 | __b_signed) {                     \
      case 0: {  /* u = u * u */                                \
        __ckd_uintmax_t __z = __x * __y;                        \
        int __o = __x && __z / __x != __y;                      \
        *(T *)__res = __z;                                      \
        return __o | (sizeof(T) < sizeof(__z) &&                \
                      __z != (__ckd_uintmax_t)*(T *)__res);     \
      }                                                         \
      case 1: {  /* u = u * s */                                \
        __ckd_uintmax_t __z = __x * __y;                        \
        int __o = __x && __z / __x != __y;                      \
        *(T *)__res = __z;                                      \
        return (__o | (((__ckd_intmax_t)__y < 0) & !!__x) |     \
                (sizeof(T) < sizeof(__z) &&                     \
                 __z != (__ckd_uintmax_t)*(T *)__res));         \
      }                                                         \
      case 2: {  /* u = s * u */                                \
        __ckd_uintmax_t __z = __x * __y;                        \
        int __o = __x && __z / __x != __y;                      \
        *(T *)__res = __z;                                      \
        return (__o | (((__ckd_intmax_t)__x < 0) & !!__y) |     \
                (sizeof(T) < sizeof(__z) &&                     \
                 __z != (__ckd_uintmax_t)*(T *)__res));         \
      }                                                         \
      case 3: {  /* u = s * s */                                \
        int __o = 0;                                            \
        if ((__ckd_intmax_t)(__x & __y) < 0) {                  \
          __x = -__x;                                           \
          __y = -__y;                                           \
        } else if ((__ckd_intmax_t)(__x ^ __y) < 0) {           \
          __o = __x && __y;                                     \
        }                                                       \
        __ckd_uintmax_t __z = __x * __y;                        \
        __o |= __x && __z / __x != __y;                         \
        *(T *)__res = __z;                                      \
        return __o | (sizeof(T) < sizeof(__z) &&                \
                      __z != (__ckd_uintmax_t)*(T *)__res);     \
      }                                                         \
      case 4: {  /* s = u * u */                                \
        __ckd_uintmax_t __z = __x * __y;                        \
        int __o = __x && __z / __x != __y;                      \
        *(T *)__res = __z;                                      \
        return (__o | ((__ckd_intmax_t)(__z) < 0) |             \
                (sizeof(T) < sizeof(__z) &&                     \
                 __z != (__ckd_uintmax_t)*(T *)__res));         \
      }                                                         \
      case 5: {  /* s = u * s */                                \
        __ckd_uintmax_t __t = -__y;                             \
        __t = (__ckd_intmax_t)(__t) < 0 ? __y : __t;            \
        __ckd_uintmax_t __p = __t * __x;                        \
        int __o = __t && __p / __t != __x;                      \
        int __n = (__ckd_intmax_t)__y < 0;                      \
        __ckd_uintmax_t __z = __n ? -__p : __p;                 \
        *(T *)__res = __z;                                      \
        __ckd_uintmax_t __m = __ckd_sign(__ckd_uintmax_t) - 1;  \
        return (__o | (__p > __m + __n) |                       \
                (sizeof(T) < sizeof(__z) &&                     \
                 __z != (__ckd_uintmax_t)*(T *)__res));         \
      }                                                         \
      case 6: {  /* s = s * u */                                \
        __ckd_uintmax_t __t = -__x;                             \
        __t = (__ckd_intmax_t)(__t) < 0 ? __x : __t;            \
        __ckd_uintmax_t __p = __t * __y;                        \
        int __o = __t && __p / __t != __y;                      \
        int __n = (__ckd_intmax_t)__x < 0;                      \
        __ckd_uintmax_t __z = __n ? -__p : __p;                 \
        *(T *)__res = __z;                                      \
        __ckd_uintmax_t __m = __ckd_sign(__ckd_uintmax_t) - 1;  \
        return (__o | (__p > __m + __n) |                       \
                (sizeof(T) < sizeof(__z) &&                     \
                 __z != (__ckd_uintmax_t)*(T *)__res));         \
      }                                                         \
      case 7: {  /* s = s * s */                                \
        __ckd_uintmax_t __z = __x * __y;                        \
        *(T *)__res = __z;                                      \
        return (((((__ckd_intmax_t)__y < 0) &&                  \
                  (__x == __ckd_sign(__ckd_uintmax_t))) ||      \
                 (__y && (((__ckd_intmax_t)__z /                \
                           (__ckd_intmax_t)__y) !=              \
                          (__ckd_intmax_t)__x))) |              \
                (sizeof(T) < sizeof(__z) &&                     \
                 __z != (__ckd_uintmax_t)*(T *)__res));         \
      }                                                         \
      default:                                                  \
        for (;;) (void)0;                                       \
    }                                                           \
  }

__ckd_declare_mul(__ckd_mul_schar, signed char)
__ckd_declare_mul(__ckd_mul_uchar, unsigned char)
__ckd_declare_mul(__ckd_mul_sshort, signed short)
__ckd_declare_mul(__ckd_mul_ushort, unsigned short)
__ckd_declare_mul(__ckd_mul_sint, signed int)
__ckd_declare_mul(__ckd_mul_uint, unsigned int)
__ckd_declare_mul(__ckd_mul_slong, signed long)
__ckd_declare_mul(__ckd_mul_ulong, unsigned long)
__ckd_declare_mul(__ckd_mul_slonger, signed long long)
__ckd_declare_mul(__ckd_mul_ulonger, unsigned long long)
#ifdef __ckd_have_int128
__ckd_declare_mul(__ckd_mul_sint128, signed __int128)
__ckd_declare_mul(__ckd_mul_uint128, unsigned __int128)
#endif

#else
#pragma message "checked integer arithmetic unsupported in this environment"

#define ckd_add(res, x, y) (*(res) = (x) + (y), 0)
#define ckd_sub(res, x, y) (*(res) = (x) - (y), 0)
#define ckd_mul(res, x, y) (*(res) = (x) * (y), 0)

#endif /* GNU */
#endif /* COSMOPOLITAN_LIBC_STDCKDINT_H_ */
