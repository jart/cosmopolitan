#ifndef COSMOPOLITAN_LIBC_MATH_H_
#define COSMOPOLITAN_LIBC_MATH_H_
/*─────────────────────────────────────────────────────────────────────────────╗
│ cosmopolitan § mathematics                                                   │
╚─────────────────────────────────────────────────────────────────────────────*/

#define M_E        2.7182818284590452354  /* 𝑒 */
#define M_LOG2E    1.4426950408889634074  /* log₂𝑒 */
#define M_LOG10E   0.43429448190325182765 /* log₁₀𝑒 */
#define M_LN2      0.69314718055994530942 /* logₑ2 */
#define M_LN10     2.30258509299404568402 /* logₑ10 */
#define M_PI       3.14159265358979323846 /* 𝜋 */
#define M_PI_2     1.57079632679489661923 /* 𝜋/2 */
#define M_PI_4     0.78539816339744830962 /* 𝜋/4 */
#define M_1_PI     0.31830988618379067154 /* 1/𝜋 */
#define M_2_PI     0.63661977236758134308 /* 2/𝜋 */
#define M_2_SQRTPI 1.12837916709551257390 /* 2/sqrt(𝜋) */
#define M_SQRT2    1.41421356237309504880 /* sqrt(2) */
#define M_SQRT1_2  0.70710678118654752440 /* 1/sqrt(2) */

#if defined(_GNU_SOURCE) || defined(_COSMO_SOURCE)
#define M_Ef        2.7182818284590452354f  /* 𝑒 */
#define M_LOG2Ef    1.4426950408889634074f  /* log₂𝑒 */
#define M_LOG10Ef   0.43429448190325182765f /* log₁₀𝑒 */
#define M_LN2f      0.69314718055994530942f /* logₑ2 */
#define M_LN10f     2.30258509299404568402f /* logₑ10 */
#define M_PIf       3.14159265358979323846f /* 𝜋 */
#define M_PI_2f     1.57079632679489661923f /* 𝜋/2 */
#define M_PI_4f     0.78539816339744830962f /* 𝜋/4 */
#define M_1_PIf     0.31830988618379067154f /* 1/𝜋 */
#define M_2_PIf     0.63661977236758134308f /* 2/𝜋 */
#define M_2_SQRTPIf 1.12837916709551257390f /* 2/sqrt(𝜋) */
#define M_SQRT2f    1.41421356237309504880f /* sqrt(2) */
#define M_SQRT1_2f  0.70710678118654752440f /* 1/sqrt(2) */
#endif

#if defined(_GNU_SOURCE) || defined(_COSMO_SOURCE)
#define M_El        2.718281828459045235360287471352662498L /* 𝑒 */
#define M_LOG2El    1.442695040888963407359924681001892137L /* log₂𝑒 */
#define M_LOG10El   0.434294481903251827651128918916605082L /* log₁₀𝑒 */
#define M_LN2l      0.693147180559945309417232121458176568L /* logₑ2 */
#define M_LN10l     2.302585092994045684017991454684364208L /* logₑ10 */
#define M_PIl       3.141592653589793238462643383279502884L /* 𝜋 */
#define M_PI_2l     1.570796326794896619231321691639751442L /* 𝜋/2 */
#define M_PI_4l     0.785398163397448309615660845819875721L /* 𝜋/4 */
#define M_1_PIl     0.318309886183790671537767526745028724L /* 1/𝜋 */
#define M_2_PIl     0.636619772367581343075535053490057448L /* 2/𝜋 */
#define M_2_SQRTPIl 1.128379167095512573896158903121545172L /* 2/sqrt(𝜋) */
#define M_SQRT2l    1.414213562373095048801688724209698079L /* sqrt(2) */
#define M_SQRT1_2l  0.707106781186547524400844362104849039L /* 1/sqrt(2) */
#endif

#define DBL_DECIMAL_DIG   __DBL_DECIMAL_DIG__
#define DBL_DIG           __DBL_DIG__
#define DBL_EPSILON       __DBL_EPSILON__
#define DBL_HAS_SUBNORM   __DBL_HAS_DENORM__
#define DBL_IS_IEC_60559  __DBL_IS_IEC_60559__
#define DBL_MANT_DIG      __DBL_MANT_DIG__
#define DBL_MANT_DIG      __DBL_MANT_DIG__
#define DBL_MAX           __DBL_MAX__
#define DBL_MAX_10_EXP    __DBL_MAX_10_EXP__
#define DBL_MAX_EXP       __DBL_MAX_EXP__
#define DBL_MIN           __DBL_MIN__ /* 2.23e–308 ↔ 1.79e308 */
#define DBL_MIN_10_EXP    __DBL_MIN_10_EXP__
#define DBL_MIN_EXP       __DBL_MIN_EXP__
#define DBL_NORM_MAX      __DBL_NORM_MAX__
#define DBL_TRUE_MIN      __DBL_DENORM_MIN__
#define DECIMAL_DIG       __LDBL_DECIMAL_DIG__
#define FLT_DECIMAL_DIG   __FLT_DECIMAL_DIG__
#define FLT_DIG           __FLT_DIG__
#define FLT_EPSILON       __FLT_EPSILON__
#define FLT_HAS_SUBNORM   __FLT_HAS_DENORM__
#define FLT_IS_IEC_60559  __FLT_IS_IEC_60559__
#define FLT_MANT_DIG      __FLT_MANT_DIG__
#define FLT_MANT_DIG      __FLT_MANT_DIG__
#define FLT_MAX           __FLT_MAX__
#define FLT_MAX_10_EXP    __FLT_MAX_10_EXP__
#define FLT_MAX_EXP       __FLT_MAX_EXP__
#define FLT_MIN           __FLT_MIN__ /* 1.18e–38 ↔ 3.40e38 */
#define FLT_MIN_10_EXP    __FLT_MIN_10_EXP__
#define FLT_MIN_EXP       __FLT_MIN_EXP__
#define FLT_NORM_MAX      __FLT_NORM_MAX__
#define FLT_RADIX         __FLT_RADIX__
#define FLT_TRUE_MIN      __FLT_DENORM_MIN__
#define HLF_MAX           6.50e4f
#define HLF_MIN           3.10e-5f
#define LDBL_DECIMAL_DIG  __LDBL_DECIMAL_DIG__
#define LDBL_DIG          __LDBL_DIG__
#define LDBL_EPSILON      __LDBL_EPSILON__
#define LDBL_HAS_SUBNORM  __LDBL_HAS_DENORM__
#define LDBL_IS_IEC_60559 __LDBL_IS_IEC_60559__
#define LDBL_MANT_DIG     __LDBL_MANT_DIG__
#define LDBL_MANT_DIG     __LDBL_MANT_DIG__
#define LDBL_MAX          __LDBL_MAX__
#define LDBL_MAX_10_EXP   __LDBL_MAX_10_EXP__
#define LDBL_MAX_EXP      __LDBL_MAX_EXP__
#define LDBL_MIN          __LDBL_MIN__ /* 3.37e–4932 ↔ 1.18e4932 */
#define LDBL_MIN_10_EXP   __LDBL_MIN_10_EXP__
#define LDBL_MIN_EXP      __LDBL_MIN_EXP__
#define LDBL_NORM_MAX     __LDBL_NORM_MAX__
#define LDBL_TRUE_MIN     __LDBL_DENORM_MIN__

#define FP_NAN       0
#define FP_INFINITE  1
#define FP_ZERO      2
#define FP_SUBNORMAL 3
#define FP_NORMAL    4
#define FP_ILOGB0    (-2147483647 - 1)
#define FP_ILOGBNAN  (-2147483647 - 1)

#define MATH_ERRNO     1
#define MATH_ERREXCEPT 2

#ifdef __FAST_MATH__
#define math_errhandling 0
#elif defined(__NO_MATH_ERRNO__)
#define math_errhandling (MATH_ERREXCEPT)
#else
#define math_errhandling (MATH_ERRNO | MATH_ERREXCEPT)
#endif

#ifdef __FP_FAST_FMA
#define FP_FAST_FMA 1
#endif
#ifdef __FP_FAST_FMAF
#define FP_FAST_FMAF 1
#endif
#ifdef __FP_FAST_FMAL
#define FP_FAST_FMAL 1
#endif

COSMOPOLITAN_C_START_

#define NAN       __builtin_nanf("")
#define INFINITY  __builtin_inff()
#define HUGE_VAL  __builtin_inf()
#define HUGE_VALF __builtin_inff()
#define HUGE_VALL __builtin_infl()

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
#define isgreater(x, y)      __builtin_isgreater(x, y)
#define isgreaterequal(x, y) __builtin_isgreaterequal(x, y)
#define isless(x, y)         __builtin_isless(x, y)
#define islessequal(x, y)    __builtin_islessequal(x, y)
#define islessgreater(x, y)  __builtin_islessgreater(x, y)
#define isunordered(x, y)    __builtin_isunordered(x, y)

#define fpclassify(x) \
  __builtin_fpclassify(FP_NAN, FP_INFINITE, FP_NORMAL, FP_SUBNORMAL, FP_ZERO, x)

#define signbit(x) __builtin_signbit(x)

extern int signgam;

double acos(double) libcesque;
double acosh(double) libcesque;
double asin(double) libcesque;
double asinh(double) libcesque;
double atan(double) libcesque;
double atan2(double, double) libcesque;
double atanh(double) libcesque;
double cbrt(double) libcesque;
double ceil(double) libcesque;
double copysign(double, double) libcesque;
double cos(double) libcesque;
double cosh(double) libcesque;
double drem(double, double) libcesque;
double erf(double) libcesque;
double erfc(double) libcesque;
double exp(double) libcesque;
double exp10(double) libcesque;
double exp2(double) libcesque;
double expm1(double) libcesque;
double fabs(double) libcesque;
double fdim(double, double) libcesque;
double floor(double) libcesque;
double fma(double, double, double) libcesque;
double fmax(double, double) libcesque;
double fmin(double, double) libcesque;
double fmod(double, double) libcesque;
double hypot(double, double) libcesque;
double ldexp(double, int) libcesque;
double log(double) libcesque;
double log10(double) libcesque;
double log1p(double) libcesque;
double log2(double) libcesque;
double logb(double) libcesque;
double nearbyint(double) libcesque;
double nextafter(double, double) libcesque;
double nexttoward(double, long double) libcesque;
double pow(double, double) libcesque;
double pow10(double) libcesque;
double powi(double, int) libcesque;
double remainder(double, double) libcesque;
double rint(double) libcesque;
double round(double) libcesque;
double scalb(double, double) libcesque;
double scalbln(double, long int) libcesque;
double scalbn(double, int) libcesque;
double significand(double) libcesque;
double sin(double) libcesque;
double sinh(double) libcesque;
double sqrt(double) libcesque;
double tan(double) libcesque;
double tanh(double) libcesque;
double trunc(double) libcesque;
double tgamma(double) libcesque;
double lgamma(double) libcesque;
double lgamma_r(double, int *) libcesque;
int finite(double) libcesque;

float acosf(float) libcesque;
float acoshf(float) libcesque;
float asinf(float) libcesque;
float asinhf(float) libcesque;
float atan2f(float, float) libcesque;
float atanf(float) libcesque;
float atanhf(float) libcesque;
float cbrtf(float) libcesque;
float ceilf(float) libcesque;
float copysignf(float, float) libcesque;
float cosf(float) libcesque;
float coshf(float) libcesque;
float dremf(float, float) libcesque;
float erfcf(float) libcesque;
float erff(float) libcesque;
float exp10f(float) libcesque;
float exp2f(float) libcesque;
float expf(float) libcesque;
float expm1f(float) libcesque;
float fabsf(float) libcesque;
float fdimf(float, float) libcesque;
float floorf(float) libcesque;
float fmaf(float, float, float) libcesque;
float fmaxf(float, float) libcesque;
float fminf(float, float) libcesque;
float fmodf(float, float) libcesque;
float hypotf(float, float) libcesque;
float ldexpf(float, int) libcesque;
float lgammaf(float) libcesque;
float lgammaf_r(float, int *) libcesque;
float log10f(float) libcesque;
float log1pf(float) libcesque;
float log2f(float) libcesque;
float logbf(float) libcesque;
float logf(float) libcesque;
float nearbyintf(float) libcesque;
float nextafterf(float, float) libcesque;
float nexttowardf(float, long double) libcesque;
float pow10f(float) libcesque;
float powf(float, float) libcesque;
float powif(float, int) libcesque;
float remainderf(float, float) libcesque;
float rintf(float) libcesque;
float roundf(float) libcesque;
float scalbf(float, float) libcesque;
float scalblnf(float, long int) libcesque;
float scalbnf(float, int) libcesque;
float significandf(float) libcesque;
float sinf(float) libcesque;
float sinhf(float) libcesque;
float sqrtf(float) libcesque;
float tanf(float) libcesque;
float tanhf(float) libcesque;
float tgammaf(float) libcesque;
float truncf(float) libcesque;
int finitef(float) libcesque;

int finitel(long double) libcesque;
long double acoshl(long double) libcesque;
long double acosl(long double) libcesque;
long double asinhl(long double) libcesque;
long double asinl(long double) libcesque;
long double atan2l(long double, long double) libcesque;
long double atanhl(long double) libcesque;
long double atanl(long double) libcesque;
long double cbrtl(long double) libcesque;
long double ceill(long double) libcesque;
long double copysignl(long double, long double) libcesque;
long double coshl(long double) libcesque;
long double cosl(long double) libcesque;
long double dreml(long double, long double) libcesque;
long double erfcl(long double) libcesque;
long double erfl(long double) libcesque;
long double exp10l(long double) libcesque;
long double exp2l(long double) libcesque;
long double expl(long double) libcesque;
long double expm1l(long double) libcesque;
long double fabsl(long double) libcesque;
long double fdiml(long double, long double) libcesque;
long double floorl(long double) libcesque;
long double fmal(long double, long double, long double) libcesque;
long double fmaxl(long double, long double) libcesque;
long double fminl(long double, long double) libcesque;
long double fmodl(long double, long double) libcesque;
long double hypotl(long double, long double) libcesque;
long double ldexpl(long double, int) libcesque;
long double lgammal(long double) libcesque;
long double lgammal_r(long double, int *) libcesque;
long double log10l(long double) libcesque;
long double log1pl(long double) libcesque;
long double log2l(long double) libcesque;
long double logbl(long double) libcesque;
long double logl(long double) libcesque;
long double nearbyintl(long double) libcesque;
long double nextafterl(long double, long double) libcesque;
long double nexttowardl(long double, long double) libcesque;
long double pow10l(long double) libcesque;
long double powl(long double, long double) libcesque;
long double remainderl(long double, long double) libcesque;
long double rintl(long double) libcesque;
long double roundl(long double) libcesque;
long double scalbl(long double, long double) libcesque;
long double scalblnl(long double, long int) libcesque;
long double scalbnl(long double, int) libcesque;
long double significandl(long double) libcesque;
long double sinhl(long double) libcesque;
long double sinl(long double) libcesque;
long double sqrtl(long double) libcesque;
long double tanhl(long double) libcesque;
long double tanl(long double) libcesque;
long double tgammal(long double) libcesque;
long double truncl(long double) libcesque;

long lrint(double) libcesque;
long lrintf(float) libcesque;
long lrintl(long double) libcesque;
long lround(double) libcesque;
long lroundf(float) libcesque;
long lroundl(long double) libcesque;

int ilogbf(float) libcesque;
int ilogb(double) libcesque;
int ilogbl(long double) libcesque;

long long llrint(double) libcesque;
long long llrintf(float) libcesque;
long long llrintl(long double) libcesque;
long long llround(double) libcesque;
long long llroundf(float) libcesque;
long long llroundl(long double) libcesque;

double frexp(double, int *) libcesque;
double modf(double, double *) libcesque;
double nan(const char *) libcesque;
double remquo(double, double, int *) libcesque;
float frexpf(float, int *) libcesque;
float modff(float, float *) libcesque;
float nanf(const char *) libcesque;
float remquof(float, float, int *) libcesque;
long double frexpl(long double, int *) libcesque;
long double modfl(long double, long double *) libcesque;
long double nanl(const char *) libcesque;
long double remquol(long double, long double, int *) libcesque;
void sincos(double, double *, double *) libcesque;
void sincosf(float, float *, float *) libcesque;
void sincosl(long double, long double *, long double *) libcesque;

double fsumf(const float *, size_t) libcesque;
double fsum(const double *, size_t) libcesque;

double j0(double) libcesque;
double j1(double) libcesque;
double jn(int, double) libcesque;
float j0f(float) libcesque;
float j1f(float) libcesque;
float jnf(int, float) libcesque;

double y0(double) libcesque;
double y1(double) libcesque;
double yn(int, double) libcesque;
float y0f(float) libcesque;
float y1f(float) libcesque;
float ynf(int, float) libcesque;

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_MATH_H_ */
