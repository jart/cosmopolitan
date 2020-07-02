#define COSMOPOLITAN_CXX_START_ namespace cosmo {
#define COSMOPOLITAN_CXX_END_   }
#define COSMOPOLITAN_CXX_USING_ using namespace cosmo;
#define COSMOPOLITAN_C_START_   extern "C" {
#define COSMOPOLITAN_C_END_     }

#if !defined(__builtin_types_compatible_p) && !__has_builtin(types_compatible_p)
#if 0 /* todo jart whyyyy */
#include "libc/integral/cxxtypescompat.inc"
#define __builtin_types_compatible_p(A, B) \
  (__cxx_types_compatible<A, B>::_value)
#else
#define __builtin_types_compatible_p(A, B) 0
#endif
#endif

#if !defined(__builtin_choose_expr) && !__has_builtin(choose_expr)
#if 1
template <bool _P, typename _T, typename _U>
struct __cxx_choose_expr {
  __cxx_choose_expr(_T _a, _U _b) : _value(_a) {}
  const _T _value;
};
template <typename _T, typename _U>
struct __cxx_choose_expr<false, _T, _U> {
  __cxx_choose_expr(_T _a, _U _b) : _value(_b) {}
  const _U _value;
};
#define __builtin_choose_expr(X, A, B) \
  (__cxx_choose_expr<X, typeof(A), typeof(B)>(A, B)._value)
#else
#define __builtin_choose_expr(X, A, B) ((X) ? (A) : (B))
#endif
#endif
