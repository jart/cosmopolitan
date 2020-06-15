#ifndef COSMOPOLITAN_LIBC_MATH_LIBM_H_
#define COSMOPOLITAN_LIBC_MATH_LIBM_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#include "libc/math/math.h"

asm(".ident\t\"\\n\\n\
libm (MIT License)\\n\
Copyright 2017-2018 Arm Limited\\n\
Copyright 2005-2019 Rich Felker\\n\
Copyright 2008-2011, Bruce D. Evans, Steven G. Kargl, David Schultz\\n\
Copyright 2008 Stephen L. Moshier <steve@moshier.net>\\n\
Copyright 1993 Sun Microsystems\"");
asm(".include \"libc/disclaimer.inc\"");

#define weak   __attribute__((__weak__))
#define hidden __attribute__((__visibility__("hidden")))
#define weak_alias(old, new) \
  extern __typeof(old) new __attribute__((__weak__, __alias__(#old)))

#define __BYTE_ORDER    __BYTE_ORDER__
#define __LITTLE_ENDIAN __ORDER_LITTLE_ENDIAN__

union ldshape {
  long double f;
  struct {
    uint64_t m;
    uint16_t se;
  } i;
};

/* Support non-nearest rounding mode.  */
#define WANT_ROUNDING 1
/* Support signaling NaNs.  */
#define WANT_SNAN 0

#if WANT_SNAN
#error SNaN is unsupported
#else
#define issignalingf_inline(x) 0
#define issignaling_inline(x)  0
#endif

#ifndef TOINT_INTRINSICS
#define TOINT_INTRINSICS 0
#endif

/* Helps static branch prediction so hot path can be better optimized.  */
#define predict_true(x)  __builtin_expect(!!(x), 1)
#define predict_false(x) __builtin_expect(x, 0)

/* Evaluate an expression as the specified type. With standard excess
   precision handling a type cast or assignment is enough (with
   -ffloat-store an assignment is required, in old compilers argument
   passing and return statement may not drop excess precision).  */

static inline float eval_as_float(float x) {
  float y = x;
  return y;
}

static inline double eval_as_double(double x) {
  double y = x;
  return y;
}

/* fp_barrier returns its input, but limits code transformations
   as if it had a side-effect (e.g. observable io) and returned
   an arbitrary value.  */

#ifndef fp_barrierf
#define fp_barrierf fp_barrierf
static inline float fp_barrierf(float x) {
  volatile float y = x;
  return y;
}
#endif

#ifndef fp_barrier
#define fp_barrier fp_barrier
static inline double fp_barrier(double x) {
  volatile double y = x;
  return y;
}
#endif

#ifndef fp_barrierl
#define fp_barrierl fp_barrierl
static inline long double fp_barrierl(long double x) {
  volatile long double y = x;
  return y;
}
#endif

/* fp_force_eval ensures that the input value is computed when that's
   otherwise unused.  To prevent the constant folding of the input
   expression, an additional fp_barrier may be needed or a compilation
   mode that does so (e.g. -frounding-math in gcc). Then it can be
   used to evaluate an expression for its fenv side-effects only.   */

#ifndef fp_force_evalf
#define fp_force_evalf fp_force_evalf
static inline void fp_force_evalf(float x) {
  volatile float y;
  y = x;
}
#endif

#ifndef fp_force_eval
#define fp_force_eval fp_force_eval
static inline void fp_force_eval(double x) {
  volatile double y;
  y = x;
}
#endif

#ifndef fp_force_evall
#define fp_force_evall fp_force_evall
static inline void fp_force_evall(long double x) {
  volatile long double y;
  y = x;
}
#endif

#define FORCE_EVAL(x)                         \
  do {                                        \
    if (sizeof(x) == sizeof(float)) {         \
      fp_force_evalf(x);                      \
    } else if (sizeof(x) == sizeof(double)) { \
      fp_force_eval(x);                       \
    } else {                                  \
      fp_force_evall(x);                      \
    }                                         \
  } while (0)

#define asuint(f) \
  ((union {       \
    float _f;     \
    uint32_t _i;  \
  }){f})          \
      ._i
#define asfloat(i) \
  ((union {        \
    uint32_t _i;   \
    float _f;      \
  }){i})           \
      ._f
#define asuint64(f) \
  ((union {         \
    double _f;      \
    uint64_t _i;    \
  }){f})            \
      ._i
#define asdouble(i) \
  ((union {         \
    uint64_t _i;    \
    double _f;      \
  }){i})            \
      ._f

#define EXTRACT_WORDS(hi, lo, d) \
  do {                           \
    uint64_t __u = asuint64(d);  \
    (hi) = __u >> 32;            \
    (lo) = (uint32_t)__u;        \
  } while (0)

#define GET_HIGH_WORD(hi, d)  \
  do {                        \
    (hi) = asuint64(d) >> 32; \
  } while (0)

#define GET_LOW_WORD(lo, d)       \
  do {                            \
    (lo) = (uint32_t)asuint64(d); \
  } while (0)

#define INSERT_WORDS(d, hi, lo)                              \
  do {                                                       \
    (d) = asdouble(((uint64_t)(hi) << 32) | (uint32_t)(lo)); \
  } while (0)

#define SET_HIGH_WORD(d, hi) INSERT_WORDS(d, hi, (uint32_t)asuint64(d))

#define SET_LOW_WORD(d, lo) INSERT_WORDS(d, asuint64(d) >> 32, lo)

#define GET_FLOAT_WORD(w, d) \
  do {                       \
    (w) = asuint(d);         \
  } while (0)

#define SET_FLOAT_WORD(d, w) \
  do {                       \
    (d) = asfloat(w);        \
  } while (0)

hidden int __rem_pio2_large(double *, double *, int, int, int);

hidden int __rem_pio2(double, double *);
hidden double __sin(double, double, int);
hidden double __cos(double, double);
hidden double __tan(double, double, int);
hidden double __expo2(double);

hidden int __rem_pio2f(float, double *);
hidden float __sindf(double);
hidden float __cosdf(double);
hidden float __tandf(double, int);
hidden float __expo2f(float);

hidden int __rem_pio2l(long double, long double *);
hidden long double __sinl(long double, long double, int);
hidden long double __cosl(long double, long double);
hidden long double __tanl(long double, long double, int);

hidden long double __polevll(long double, const long double *, int);
hidden long double __p1evll(long double, const long double *, int);

extern int __signgam;
hidden double __lgamma_r(double, int *);
hidden float __lgammaf_r(float, int *);

/* error handling functions */
hidden float __math_xflowf(uint32_t, float);
hidden float __math_uflowf(uint32_t);
hidden float __math_oflowf(uint32_t);
hidden float __math_divzerof(uint32_t);
hidden float __math_invalidf(float);
hidden double __math_xflow(uint32_t, double);
hidden double __math_uflow(uint32_t);
hidden double __math_oflow(uint32_t);
hidden double __math_divzero(uint32_t);
hidden double __math_invalid(double);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_MATH_LIBM_H_ */
