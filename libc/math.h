#ifndef COSMOPOLITAN_LIBC_MATH_H_
#define COSMOPOLITAN_LIBC_MATH_H_
/*─────────────────────────────────────────────────────────────────────────────╗
│ cosmopolitan § mathematics                                                   │
╚─────────────────────────────────────────────────────────────────────────────*/

#define M_E        2.7182818284590452354 /* 𝑒 */
#define M_LOG2_10  0xd.49a784bcd1b8afep-2 /* log₂10 ≈ 3.3219280948873623478 */
#define M_LOG10_2  0x9.a209a84fbcff799p-5 /* log₁₀2 ≈ 0.301029995663981195 */
#define M_LOG2E    0xb.8aa3b295c17f0bcp-3 /* log₂𝑒 ≈ 1.4426950408889634074 */
#define M_LOG10E   0.43429448190325182765  /* log₁₀𝑒 */
#define M_LN2      0xb.17217f7d1cf79acp-4  /* logₑ2 ≈ */
#define M_LN10     2.30258509299404568402  /* logₑ10 */
#define M_TAU      0x1.921fb54442d1846ap+2 /* τ = 2π */
#define M_PI       0x1.921fb54442d1846ap+1 /* π ≈ 3.14159265358979323846 */
#define M_PI_2     1.57079632679489661923  /* π/2 */
#define M_PI_4     0.78539816339744830962  /* π/4 */
#define M_1_PI     0.31830988618379067154  /* 1/π */
#define M_2_PI     0.63661977236758134308  /* 2/π */
#define M_2_SQRTPI 1.12837916709551257390  /* 2/sqrtπ */
#define M_SQRT2    1.41421356237309504880  /* sqrt2 */
#define M_SQRT1_2  0.70710678118654752440  /* 1/sqrt2 */

#define DBL_DECIMAL_DIG  __DBL_DECIMAL_DIG__
#define DBL_DIG          __DBL_DIG__
#define DBL_EPSILON      __DBL_EPSILON__
#define DBL_MANT_DIG     __DBL_MANT_DIG__
#define DBL_MANT_DIG     __DBL_MANT_DIG__
#define DBL_MAX          __DBL_MAX__
#define DBL_MAX_10_EXP   __DBL_MAX_10_EXP__
#define DBL_MAX_EXP      __DBL_MAX_EXP__
#define DBL_MIN          __DBL_MIN__ /* 2.23e–308 ↔ 1.79e308 */
#define DBL_MIN_10_EXP   __DBL_MIN_10_EXP__
#define DBL_MIN_EXP      __DBL_MIN_EXP__
#define DECIMAL_DIG      __LDBL_DECIMAL_DIG__
#define FLT_DECIMAL_DIG  __FLT_DECIMAL_DIG__
#define FLT_RADIX        __FLT_RADIX__
#define FLT_DIG          __FLT_DIG__
#define FLT_EPSILON      __FLT_EPSILON__
#define FLT_MANT_DIG     __FLT_MANT_DIG__
#define FLT_MANT_DIG     __FLT_MANT_DIG__
#define FLT_MAX          __FLT_MAX__
#define FLT_MAX_10_EXP   __FLT_MAX_10_EXP__
#define FLT_MAX_EXP      __FLT_MAX_EXP__
#define FLT_MIN          __FLT_MIN__ /* 1.18e–38 ↔ 3.40e38 */
#define FLT_MIN_10_EXP   __FLT_MIN_10_EXP__
#define FLT_MIN_EXP      __FLT_MIN_EXP__
#define HLF_MAX          6.50e4f
#define HLF_MIN          3.10e-5f
#define LDBL_DECIMAL_DIG __LDBL_DECIMAL_DIG__
#define LDBL_DIG         __LDBL_DIG__
#define LDBL_EPSILON     __LDBL_EPSILON__
#define LDBL_MANT_DIG    __LDBL_MANT_DIG__
#define LDBL_MANT_DIG    __LDBL_MANT_DIG__
#define LDBL_MAX         __LDBL_MAX__
#define LDBL_MAX_10_EXP  __LDBL_MAX_10_EXP__
#define LDBL_MAX_EXP     __LDBL_MAX_EXP__
#define LDBL_MIN         __LDBL_MIN__ /* 3.37e–4932 ↔ 1.18e4932 */
#define LDBL_MIN_10_EXP  __LDBL_MIN_10_EXP__
#define LDBL_MIN_EXP     __LDBL_MIN_EXP__

#define FP_NAN       0
#define FP_INFINITE  1
#define FP_ZERO      2
#define FP_SUBNORMAL 3
#define FP_NORMAL    4

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define NAN      __builtin_nanf("")
#define INFINITY __builtin_inff()
#define HUGE_VAL __builtin_inff()

#if __FLT_EVAL_METHOD__ + 0 == 2
typedef long double float_t;
typedef long double double_t;
#else
typedef float float_t;
typedef double double_t;
#endif

#define isinf(x)             __builtin_isinf(x)
#define isnan(x)             __builtin_isnan(x)
#define isfinite(x)          __builtin_isfinite(x)
#define isnormal(x)          __builtin_isnormal(x)
#define signbit(x)           __builtin_signbit(x)
#define isgreater(x, y)      __builtin_isgreater(x, y)
#define isgreaterequal(x, y) __builtin_isgreaterequal(x, y)
#define isless(x, y)         __builtin_isless(x, y)
#define islessequal(x, y)    __builtin_islessequal(x, y)
#define islessgreater(x, y)  __builtin_islessgreater(x, y)
#define isunordered(x, y)    __builtin_isunordered(x, y)

#define fpclassify(x) \
  __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x)

double acos(double);
double acosh(double);
double asin(double);
double asinh(double);
double atan(double);
double atan2(double, double);
double atanh(double);
double cbrt(double);
double ceil(double);
double copysign(double, double);
double cos(double);
double cosh(double);
double drem(double, double);
double erf(double);
double erfc(double);
double exp(double);
double exp10(double);
double exp2(double);
double expm1(double);
double fabs(double);
double fdim(double, double);
double floor(double);
double fma(double, double, double);
double fmax(double, double);
double fmin(double, double);
double fmod(double, double);
double hypot(double, double);
double ldexp(double, int);
double log(double);
double log10(double);
double log1p(double);
double log2(double);
double logb(double);
double nearbyint(double);
double nextafter(double, double);
double nexttoward(double, long double);
double pow(double, double);
double pow10(double);
double powi(double, int);
double remainder(double, double);
double rint(double);
double round(double);
double scalb(double, double);
double scalbln(double, long int);
double scalbn(double, int);
double significand(double);
double sin(double);
double sinh(double);
double sqrt(double);
double tan(double);
double tanh(double);
double trunc(double);
double lgamma(double);
double lgamma_r(double, int *);

float acosf(float);
float acoshf(float);
float asinf(float);
float asinhf(float);
float atan2f(float, float);
float atanf(float);
float atanhf(float);
float cbrtf(float);
float ceilf(float);
float copysignf(float, float);
float cosf(float);
float coshf(float);
float dremf(float, float);
float erfcf(float);
float erff(float);
float exp10f(float);
float exp2f(float);
float expf(float);
float expm1f(float);
float fabsf(float);
float fdimf(float, float);
float floorf(float);
float fmaf(float, float, float);
float fmaxf(float, float);
float fminf(float, float);
float fmodf(float, float);
float hypotf(float, float);
float ldexpf(float, int);
float log10f(float);
float log1pf(float);
float log2f(float);
float logbf(float);
float logf(float);
float nearbyintf(float);
float nextafterf(float, float);
float nexttowardf(float, long double);
float pow10f(float);
float powf(float, float);
float powif(float, int);
float remainderf(float, float);
float rintf(float);
float roundf(float);
float scalbf(float, float);
float scalblnf(float, long int);
float scalbnf(float, int);
float significandf(float);
float sinf(float);
float sinhf(float);
float sqrtf(float);
float tanf(float);
float tanhf(float);
float truncf(float);

long double acoshl(long double);
long double acosl(long double);
long double asinhl(long double);
long double asinl(long double);
long double atan2l(long double, long double);
long double atanhl(long double);
long double atanl(long double);
long double cbrtl(long double);
long double ceill(long double);
long double copysignl(long double, long double);
long double coshl(long double);
long double cosl(long double);
long double dreml(long double, long double);
long double erfcl(long double);
long double erfl(long double);
long double exp10l(long double);
long double exp2l(long double);
long double expl(long double);
long double expm1l(long double);
long double fabsl(long double);
long double fdiml(long double, long double);
long double floorl(long double);
long double fmal(long double, long double, long double);
long double fmaxl(long double, long double);
long double fminl(long double, long double);
long double fmodl(long double, long double);
long double hypotl(long double, long double);
long double ldexpl(long double, int);
long double log10l(long double);
long double log1pl(long double);
long double log2l(long double);
long double logbl(long double);
long double logl(long double);
long double nearbyintl(long double);
long double nextafterl(long double, long double);
long double nexttowardl(long double, long double);
long double pow10l(long double);
long double powil(long double, int);
long double powl(long double, long double);
long double remainderl(long double, long double);
long double rintl(long double);
long double roundl(long double);
long double scalbl(long double, long double);
long double scalblnl(long double, long int);
long double scalbnl(long double, int);
long double significandl(long double);
long double sinhl(long double);
long double sinl(long double);
long double sqrtl(long double);
long double tanhl(long double);
long double tanl(long double);
long double truncl(long double);

long lrint(double);
long lrintf(float);
long lrintl(long double);
long lround(double);
long lroundf(float);
long lroundl(long double);

int ilogbf(float);
int ilogb(double);
int ilogbl(long double);

long long llrint(double);
long long llrintf(float);
long long llrintl(long double);
long long llround(double);
long long llroundf(float);
long long llroundl(long double);

double frexp(double, int *);
double modf(double, double *);
double remquo(double, double, int *);
float frexpf(float, int *);
float modff(float, float *);
float remquof(float, float, int *);
long double frexpl(long double, int *);
long double modfl(long double, long double *);
long double remquol(long double, long double, int *);
void sincos(double, double *, double *);
void sincosf(float, float *, float *);
void sincosl(long double, long double *, long double *);

float fsumf(const float *, size_t);
double fsum(const double *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_MATH_H_ */
