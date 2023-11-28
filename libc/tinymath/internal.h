#ifndef COSMOPOLITAN_LIBC_TINYMATH_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_INTERNAL_H_

#define WANT_ROUNDING 1

COSMOPOLITAN_C_START_

#define issignalingf_inline(x) 0
#define issignaling_inline(x)  0

// clang-format off
#define asuint(f) ((union{float _f; uint32_t _i;}){f})._i
#define asfloat(i) ((union{uint32_t _i; float _f;}){i})._f
#define asuint64(f) ((union{double _f; uint64_t _i;}){f})._i
#define asdouble(i) ((union{uint64_t _i; double _f;}){i})._f
// clang-format on

static inline float eval_as_float(float x) {
  float y = x;
  return y;
}

static inline double eval_as_double(double x) {
  double y = x;
  return y;
}

static inline void fp_force_evall(long double x) {
  volatile long double y;
  y = x;
  (void)y;
}

static inline void fp_force_evalf(float x) {
  volatile float y;
  y = x;
  (void)y;
}

static inline void fp_force_eval(double x) {
  volatile double y;
  y = x;
  (void)y;
}

static inline double fp_barrier(double x) {
  volatile double y = x;
  return y;
}

static inline float fp_barrierf(float x) {
  volatile float y = x;
  return y;
}

extern const uint16_t __rsqrt_tab[128];

double __math_divzero(uint32_t);
double __math_invalid(double);
double __math_oflow(uint32_t);
double __math_uflow(uint32_t);
double __math_xflow(uint32_t, double);
float __math_divzerof(uint32_t);
float __math_invalidf(float);
float __math_oflowf(uint32_t);
float __math_uflowf(uint32_t);
float __math_xflowf(uint32_t, float);
long double __polevll(long double, const long double *, int);
long double __p1evll(long double, const long double *, int);
long double __math_invalidl(long double);

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

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TINYMATH_INTERNAL_H_ */
