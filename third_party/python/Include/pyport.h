#ifndef Py_PYPORT_H
#define Py_PYPORT_H
#include "libc/limits.h"
#include "third_party/python/Include/pymath.h"
#include "third_party/python/pyconfig.h"

#define HAVE_LONG_LONG 1
#define PY_LONG_LONG long long
#define PY_LLONG_MIN  LLONG_MIN
#define PY_LLONG_MAX  LLONG_MAX
#define PY_ULLONG_MAX ULLONG_MAX

#define PY_UINT32_T uint32_t
#define PY_UINT64_T uint64_t

/* Signed variants of the above */
#define PY_INT32_T int32_t
#define PY_INT64_T int64_t

/* If PYLONG_BITS_IN_DIGIT is not defined then we'll use 30-bit digits if all
   the necessary integer types are available, and we're on a 64-bit platform
   (as determined by SIZEOF_VOID_P); otherwise we use 15-bit digits. */

#ifndef PYLONG_BITS_IN_DIGIT
#if SIZEOF_VOID_P >= 8
#define PYLONG_BITS_IN_DIGIT 30
#else
#define PYLONG_BITS_IN_DIGIT 15
#endif
#endif

/* uintptr_t is the C9X name for an unsigned integral type such that a
 * legitimate void* can be cast to uintptr_t and then back to void* again
 * without loss of information.  Similarly for intptr_t, wrt a signed
 * integral type.
 */
typedef uintptr_t Py_uintptr_t;
typedef intptr_t Py_intptr_t;

/* Py_ssize_t is a signed integral type such that sizeof(Py_ssize_t) ==
 * sizeof(size_t).  C99 doesn't define such a thing directly (size_t is an
 * unsigned integral type).  See PEP 353 for details.
 */
#ifdef HAVE_SSIZE_T
typedef ssize_t Py_ssize_t;
#elif SIZEOF_VOID_P == SIZEOF_SIZE_T
typedef Py_intptr_t Py_ssize_t;
#else
#error "Python needs a typedef for Py_ssize_t in pyport.h."
#endif

/* Py_hash_t is the same size as a pointer. */
#define SIZEOF_PY_HASH_T SIZEOF_SIZE_T
typedef Py_ssize_t Py_hash_t;
/* Py_uhash_t is the unsigned equivalent needed to calculate numeric hash. */
#define SIZEOF_PY_UHASH_T SIZEOF_SIZE_T
typedef size_t Py_uhash_t;

/* Only used for compatibility with code that may not be PY_SSIZE_T_CLEAN. */
#ifdef PY_SSIZE_T_CLEAN
typedef Py_ssize_t Py_ssize_clean_t;
#else
typedef int Py_ssize_clean_t;
#endif

#define PY_SIZE_MAX SIZE_MAX
#define PY_SSIZE_T_MAX ((Py_ssize_t)(((size_t)-1) >> 1))
#define PY_SSIZE_T_MIN (-PY_SSIZE_T_MAX - 1)

#ifdef USE_INLINE
#define Py_LOCAL(type)        static type
#define Py_LOCAL_INLINE(type) static inline type
#else
#define Py_LOCAL(type)        static type
#define Py_LOCAL_INLINE(type) static type
#endif

#define Py_ARITHMETIC_RIGHT_SHIFT(TYPE, I, J) ((I) >> (J))

/* Py_FORCE_EXPANSION(X)
 * "Simply" returns its argument.  However, macro expansions within the
 * argument are evaluated.  This unfortunate trickery is needed to get
 * token-pasting to work as desired in some cases.
 */
#define Py_FORCE_EXPANSION(X) X

/* Py_SAFE_DOWNCAST(VALUE, WIDE, NARROW)
 * Cast VALUE to type NARROW from type WIDE.  In Py_DEBUG mode, this
 * assert-fails if any information is lost.
 * Caution:
 *    VALUE may be evaluated more than once.
 */
#ifdef Py_DEBUG
#define Py_SAFE_DOWNCAST(VALUE, WIDE, NARROW) \
  (assert((WIDE)(NARROW)(VALUE) == (VALUE)), (NARROW)(VALUE))
#else
#define Py_SAFE_DOWNCAST(VALUE, WIDE, NARROW) (NARROW)(VALUE)
#endif

/* Py_SET_ERRNO_ON_MATH_ERROR(x)
 * If a libm function did not set errno, but it looks like the result
 * overflowed or not-a-number, set errno to ERANGE or EDOM.  Set errno
 * to 0 before calling a libm function, and invoke this macro after,
 * passing the function result.
 * Caution:
 *    This isn't reliable.  See Py_OVERFLOWED comments.
 *    X is evaluated more than once.
 */
#if defined(__FreeBSD__) || defined(__OpenBSD__) || \
    (defined(__hpux) && defined(__ia64))
#define _Py_SET_EDOM_FOR_NAN(X) \
  if (isnan(X)) errno = EDOM;
#else
#define _Py_SET_EDOM_FOR_NAN(X) ;
#endif
#define Py_SET_ERRNO_ON_MATH_ERROR(X)                \
  do {                                               \
    if (errno == 0) {                                \
      if ((X) == Py_HUGE_VAL || (X) == -Py_HUGE_VAL) \
        errno = ERANGE;                              \
      else                                           \
        _Py_SET_EDOM_FOR_NAN(X)                      \
    }                                                \
  } while (0)

/* Py_SET_ERANGE_ON_OVERFLOW(x)
 * An alias of Py_SET_ERRNO_ON_MATH_ERROR for backward-compatibility.
 */
#define Py_SET_ERANGE_IF_OVERFLOW(X) Py_SET_ERRNO_ON_MATH_ERROR(X)

/* Py_ADJUST_ERANGE1(x)
 * Py_ADJUST_ERANGE2(x, y)
 * Set errno to 0 before calling a libm function, and invoke one of these
 * macros after, passing the function result(s) (Py_ADJUST_ERANGE2 is useful
 * for functions returning complex results).  This makes two kinds of
 * adjustments to errno:  (A) If it looks like the platform libm set
 * errno=ERANGE due to underflow, clear errno. (B) If it looks like the
 * platform libm overflowed but didn't set errno, force errno to ERANGE.  In
 * effect, we're trying to force a useful implementation of C89 errno
 * behavior.
 * Caution:
 *    This isn't reliable.  See Py_OVERFLOWED comments.
 *    X and Y may be evaluated more than once.
 */
#define Py_ADJUST_ERANGE1(X)                                         \
  do {                                                               \
    if (errno == 0) {                                                \
      if ((X) == Py_HUGE_VAL || (X) == -Py_HUGE_VAL) errno = ERANGE; \
    } else if (errno == ERANGE && (X) == 0.0)                        \
      errno = 0;                                                     \
  } while (0)

#define Py_ADJUST_ERANGE2(X, Y)                                            \
  do {                                                                     \
    if ((X) == Py_HUGE_VAL || (X) == -Py_HUGE_VAL || (Y) == Py_HUGE_VAL || \
        (Y) == -Py_HUGE_VAL) {                                             \
      if (errno == 0) errno = ERANGE;                                      \
    } else if (errno == ERANGE)                                            \
      errno = 0;                                                           \
  } while (0)

/*  The functions _Py_dg_strtod and _Py_dg_dtoa in Python/dtoa.c (which are
 *  required to support the short float repr introduced in Python 3.1) require
 *  that the floating-point unit that's being used for arithmetic operations
 *  on C doubles is set to use 53-bit precision.  It also requires that the
 *  FPU rounding mode is round-half-to-even, but that's less often an issue.
 *
 *  If your FPU isn't already set to 53-bit precision/round-half-to-even, and
 *  you want to make use of _Py_dg_strtod and _Py_dg_dtoa, then you should
 *
 *     #define HAVE_PY_SET_53BIT_PRECISION 1
 *
 *  and also give appropriate definitions for the following three macros:
 *
 *    _PY_SET_53BIT_PRECISION_START : store original FPU settings, and
 *        set FPU to 53-bit precision/round-half-to-even
 *    _PY_SET_53BIT_PRECISION_END : restore original FPU settings
 *    _PY_SET_53BIT_PRECISION_HEADER : any variable declarations needed to
 *        use the two macros above.
 *
 * The macros are designed to be used within a single C function: see
 * Python/pystrtod.c for an example of their use.
 */

/* get and set x87 control word for gcc/x86 */
#ifdef HAVE_GCC_ASM_FOR_X87
#define HAVE_PY_SET_53BIT_PRECISION 1
/* _Py_get/set_387controlword functions are defined in Python/pymath.c */
#define _Py_SET_53BIT_PRECISION_HEADER \
  unsigned short old_387controlword, new_387controlword
#define _Py_SET_53BIT_PRECISION_START                             \
  do {                                                            \
    old_387controlword = _Py_get_387controlword();                \
    new_387controlword = (old_387controlword & ~0x0f00) | 0x0200; \
    if (new_387controlword != old_387controlword)                 \
      _Py_set_387controlword(new_387controlword);                 \
  } while (0)
#define _Py_SET_53BIT_PRECISION_END             \
  if (new_387controlword != old_387controlword) \
  _Py_set_387controlword(old_387controlword)
#endif

/* default definitions are empty */
#ifndef HAVE_PY_SET_53BIT_PRECISION
#define _Py_SET_53BIT_PRECISION_HEADER
#define _Py_SET_53BIT_PRECISION_START
#define _Py_SET_53BIT_PRECISION_END
#endif

#if !defined(DOUBLE_IS_LITTLE_ENDIAN_IEEE754) && \
    !defined(DOUBLE_IS_BIG_ENDIAN_IEEE754) &&    \
    !defined(DOUBLE_IS_ARM_MIXED_ENDIAN_IEEE754)
#define PY_NO_SHORT_FLOAT_REPR
#endif

/* double rounding is symptomatic of use of extended precision on x86.  If
   we're seeing double rounding, and we don't have any mechanism available for
   changing the FPU rounding precision, then don't use Python/dtoa.c. */
#if defined(X87_DOUBLE_ROUNDING) && !defined(HAVE_PY_SET_53BIT_PRECISION)
#define PY_NO_SHORT_FLOAT_REPR
#endif

/* Py_DEPRECATED(version)
 * Declare a variable, type, or function deprecated.
 * Usage:
 *    extern int old_var Py_DEPRECATED(2.3);
 *    typedef int T1 Py_DEPRECATED(2.4);
 *    extern int x() Py_DEPRECATED(2.5);
 */
#if defined(__GNUC__) && \
    ((__GNUC__ >= 4) || (__GNUC__ == 3) && (__GNUC_MINOR__ >= 1))
#define Py_DEPRECATED(VERSION_UNUSED) __attribute__((__deprecated__))
#else
#define Py_DEPRECATED(VERSION_UNUSED)
#endif

/* _Py_HOT_FUNCTION
 * The hot attribute on a function is used to inform the compiler that the
 * function is a hot spot of the compiled program. The function is optimized
 * more aggressively and on many target it is placed into special subsection of
 * the text section so all hot functions appears close together improving
 * locality.
 *
 * Usage:
 *    int _Py_HOT_FUNCTION x(void) { return 3; }
 *
 * Issue #28618: This attribute must not be abused, otherwise it can have a
 * negative effect on performance. Only the functions were Python spend most of
 * its time must use it. Use a profiler when running performance benchmark
 * suite to find these functions.
 */
#if !IsModeDbg() && defined(__GNUC__) \
    && ((__GNUC__ >= 5) || (__GNUC__ == 4) && (__GNUC_MINOR__ >= 3))
#define _Py_HOT_FUNCTION __attribute__((hot))
#else
#define _Py_HOT_FUNCTION
#endif

#define RTYPE RTYPE
#ifdef __cplusplus
#define PyMODINIT_FUNC extern "C" PyObject *
#else
#define PyMODINIT_FUNC PyObject *
#endif

#ifndef LONG_BIT
#define LONG_BIT (8 * SIZEOF_LONG)
#endif

#if LONG_BIT != 8 * SIZEOF_LONG
#error "LONG_BIT wrong"
#endif

#if (!defined(__GNUC__) || __GNUC__ < 2 || \
     (__GNUC__ == 2 && __GNUC_MINOR__ < 7))
#define Py_GCC_ATTRIBUTE(x)
#else
#define Py_GCC_ATTRIBUTE(x) __attribute__(x)
#endif

#if defined(__GNUC__) && __GNUC__ >= 3
#define Py_ALIGNED(x) __attribute__((__aligned__(x)))
#else
#define Py_ALIGNED(x)
#endif

#ifndef Py_LL
#define Py_LL(x) x##LL
#endif

#ifndef Py_ULL
#define Py_ULL(x) Py_LL(x##U)
#endif

#define Py_VA_COPY va_copy

#ifdef WORDS_BIGENDIAN
#define PY_BIG_ENDIAN    1
#define PY_LITTLE_ENDIAN 0
#else
#define PY_BIG_ENDIAN    0
#define PY_LITTLE_ENDIAN 1
#endif

#define _Py_BEGIN_SUPPRESS_IPH
#define _Py_END_SUPPRESS_IPH

#endif /* Py_PYPORT_H */
