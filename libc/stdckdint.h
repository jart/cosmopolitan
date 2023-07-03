#ifndef COSMOPOLITAN_LIBC_STDCKDINT_H_
#define COSMOPOLITAN_LIBC_STDCKDINT_H_

#define __STDC_VERSION_STDCKDINT_H__ 202311L

#if !defined(MODE_DBG) &&                                                 \
    ((defined(__GNUC__) && __GNUC__ >= 5 && !defined(__ICC)) ||           \
     (defined(__has_builtin) && (__has_builtin(__builtin_add_overflow) && \
                                 __has_builtin(__builtin_sub_overflow) && \
                                 __has_builtin(__builtin_mul_overflow))))

#define ckd_add(res, x, y) __builtin_add_overflow((x), (y), (res))
#define ckd_sub(res, x, y) __builtin_sub_overflow((x), (y), (res))
#define ckd_mul(res, x, y) __builtin_mul_overflow((x), (y), (res))

#else

#define ckd_add(res, x, y) __ckd_arithmetic(add, res, x, y)
#define ckd_sub(res, x, y) __ckd_arithmetic(sub, res, x, y)
#define ckd_mul(res, x, y) __ckd_arithmetic(mul, res, x, y)

/*
 * implementation details
 */

#if defined(__SIZEOF_LONG__) && __SIZEOF_LONG__ == 8 &&               \
    ((defined(__GNUC__) && __GNUC__ * 100 + __GNUC_MINOR__ >= 406) || \
     defined(__llvm__)) &&                                            \
    !defined(__STRICT_ANSI__)
#define __ckd_dword __int128
#else
#define __ckd_dword long long
#endif

#if defined(__cplusplus) && __cplusplus >= 201103L
#include "third_party/libcxx/type_traits"
#define __ckd_is_unsigned(res) std::is_unsigned<decltype(*(res))>::value
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define __ckd_is_unsigned(res)                                              \
  _Generic(*(res), unsigned : 1, unsigned long : 1, unsigned long long : 1, \
           unsigned __ckd_dword : 1, default : 0)
#else
#define __ckd_is_unsigned(res) ((__typeof(*(res)))-1 >= 0)
#endif

#define __ckd_arithmetic(op, res, x, y)                                  \
  (__ckd_is_unsigned(res)                                                \
       ? (sizeof(*(res)) == sizeof(int)                                  \
              ? __ckd_##op##u((unsigned *)(res), (x), (y))               \
          : sizeof(*(res)) == sizeof(long)                               \
              ? __ckd_##op##ul((unsigned long *)(res), (x), (y))         \
          : sizeof(*(res)) == sizeof(__ckd_dword)                        \
              ? __ckd_##op##ull((unsigned __ckd_dword *)(res), (x), (y)) \
              : __ckd_trap())                                            \
       : (sizeof(*(res)) == sizeof(int)                                  \
              ? __ckd_##op((signed int *)(res), (x), (y))                \
          : sizeof(*(res)) == sizeof(long)                               \
              ? __ckd_##op##l((long *)(res), (x), (y))                   \
          : sizeof(*(res)) == sizeof(__ckd_dword)                        \
              ? __ckd_##op##ll((__ckd_dword *)(res), (x), (y))           \
              : __ckd_trap()))

__funline wontreturn int __ckd_trap(void) {
  *(volatile int *)0 = 0;
}

/*
 * unsigned checked arithmetic
 */

__funline int __ckd_addu(unsigned *__z, unsigned __x, unsigned __y) {
  *__z = __x + __y;
  return *__z < __x;
}
__funline int __ckd_addul(unsigned long *__z, unsigned long __x,
                          unsigned long __y) {
  *__z = __x + __y;
  return *__z < __x;
}
__funline int __ckd_addull(unsigned __ckd_dword *__z, unsigned __ckd_dword __x,
                           unsigned __ckd_dword __y) {
  *__z = __x + __y;
  return *__z < __x;
}

__funline int __ckd_subu(unsigned *__z, unsigned __x, unsigned __y) {
  *__z = __x - __y;
  return __x < __y;
}
__funline int __ckd_subul(unsigned long *__z, unsigned long __x,
                          unsigned long __y) {
  *__z = __x - __y;
  return __x < __y;
}
__funline int __ckd_subull(unsigned __ckd_dword *__z, unsigned __ckd_dword __x,
                           unsigned __ckd_dword __y) {
  *__z = __x - __y;
  return __x < __y;
}

__funline int __ckd_mulu(unsigned *__z, unsigned __x, unsigned __y) {
  *__z = __x * __y;
  return __x && *__z / __x != __y;
}
__funline int __ckd_mulul(unsigned long *__z, unsigned long __x,
                          unsigned long __y) {
  *__z = __x * __y;
  return __x && *__z / __x != __y;
}
__funline int __ckd_mulull(unsigned __ckd_dword *__z, unsigned __ckd_dword __x,
                           unsigned __ckd_dword __y) {
  *__z = __x * __y;
  return __x && *__z / __x != __y;
}

/*
 * signed checked arithmetic
 */

int __mulosi4(int, int, int *);
long __mulodi4(long, long, int *);
__ckd_dword __muloti4(__ckd_dword, __ckd_dword, int *);

__funline int __ckd_add(int *__z, int __x, int __y) {
  unsigned int __a, __b, __c;
  *__z = __c = (__a = __x) + (__b = __y);
  return ((__c ^ __a) & (__c ^ __b)) >> (sizeof(int) * CHAR_BIT - 1);
}
__funline int __ckd_addl(long *__z, long __x, long __y) {
  unsigned long __a, __b, __c;
  *__z = __c = (__a = __x) + (__b = __y);
  return ((__c ^ __a) & (__c ^ __b)) >> (sizeof(long) * CHAR_BIT - 1);
}
__funline int __ckd_addll(__ckd_dword *__z, __ckd_dword __x, __ckd_dword __y) {
  unsigned __ckd_dword __a, __b, __c;
  *__z = __c = (__a = __x) + (__b = __y);
  return ((__c ^ __a) & (__c ^ __b)) >> (sizeof(__ckd_dword) * CHAR_BIT - 1);
}

__funline int __ckd_sub(int *__z, int __x, int __y) {
  unsigned int __a, __b, __c;
  *__z = __c = (__a = __x) - (__b = __y);
  return ((__a ^ __b) & (__c ^ __a)) >> (sizeof(int) * CHAR_BIT - 1);
}
__funline int __ckd_subl(long *__z, long __x, long __y) {
  unsigned long __a, __b, __c;
  *__z = __c = (__a = __x) - (__b = __y);
  return ((__a ^ __b) & (__c ^ __a)) >> (sizeof(long) * CHAR_BIT - 1);
}
__funline int __ckd_subll(__ckd_dword *__z, __ckd_dword __x, __ckd_dword __y) {
  unsigned __ckd_dword __a, __b, __c;
  *__z = __c = (__a = __x) - (__b = __y);
  return ((__a ^ __b) & (__c ^ __a)) >> (sizeof(__ckd_dword) * CHAR_BIT - 1);
}

__funline int __ckd_mul(int *__z, int __x, int __y) {
  int __o;
  *__z = __mulosi4(__x, __y, &__o);
  return __o;
}
__funline int __ckd_mull(long *__z, long __x, long __y) {
  int __o;
  *__z = __mulodi4(__x, __y, &__o);
  return __o;
}
__funline int __ckd_mulll(__ckd_dword *__z, __ckd_dword __x, __ckd_dword __y) {
  int __o;
  *__z = __muloti4(__x, __y, &__o);
  return __o;
}

#endif
#endif /* COSMOPOLITAN_LIBC_STDCKDINT_H_ */
