#ifndef _TGMATH_H
#define _TGMATH_H
#include "libc/complex.h"
#include "libc/imag.internal.h"
#include "libc/math.h"
#if __STDC_VERSION__ + 0 >= 201112

/* from https://en.cppreference.com/w/c/numeric/tgmath */

#define fabs(x)                        \
  _Generic((x), float                  \
           : fabsf, default            \
           : fabs, long double         \
           : fabsl, complex float      \
           : cabsf, complex double     \
           : cabs, complex long double \
           : cabsl)(x)

#define exp(x)                         \
  _Generic((x), float                  \
           : expf, default             \
           : exp, long double          \
           : expl, complex float       \
           : cexpf, complex double     \
           : cexp, complex long double \
           : cexpl)(x)

#define log(x)                         \
  _Generic((x), float                  \
           : logf, default             \
           : log, long double          \
           : logl, complex float       \
           : clogf, complex double     \
           : clog, complex long double \
           : clogl)(x)

#define pow(x, y)                      \
  _Generic((x), float                  \
           : powf, default             \
           : pow, long double          \
           : powl, complex float       \
           : cpowf, complex double     \
           : cpow, complex long double \
           : cpowl)(x, y)

#define sqrt(x)                         \
  _Generic((x), float                   \
           : sqrtf, default             \
           : sqrt, long double          \
           : sqrtl, complex float       \
           : csqrtf, complex double     \
           : csqrt, complex long double \
           : csqrtl)(x)

#define sin(x)                         \
  _Generic((x), float                  \
           : sinf, default             \
           : sin, long double          \
           : sinl, complex float       \
           : csinf, complex double     \
           : csin, complex long double \
           : csinl)(x)

#define cos(x)                         \
  _Generic((x), float                  \
           : cosf, default             \
           : cos, long double          \
           : cosl, complex float       \
           : ccosf, complex double     \
           : ccos, complex long double \
           : ccosl)(x)

#define tan(x)                         \
  _Generic((x), float                  \
           : tanf, default             \
           : tan, long double          \
           : tanl, complex float       \
           : ctanf, complex double     \
           : ctan, complex long double \
           : ctanl)(x)

#define asin(x)                         \
  _Generic((x), float                   \
           : asinf, default             \
           : asin, long double          \
           : asinl, complex float       \
           : casinf, complex double     \
           : casin, complex long double \
           : casinl)(x)

#define acos(x)                         \
  _Generic((x), float                   \
           : acosf, default             \
           : acos, long double          \
           : acosl, complex float       \
           : cacosf, complex double     \
           : cacos, complex long double \
           : cacosl)(x)

#define atan(x)                         \
  _Generic((x), float                   \
           : atanf, default             \
           : atan, long double          \
           : atanl, complex float       \
           : catanf, complex double     \
           : catan, complex long double \
           : catanl)(x)

#define sinh(x)                         \
  _Generic((x), float                   \
           : sinhf, default             \
           : sinh, long double          \
           : sinhl, complex float       \
           : csinhf, complex double     \
           : csinh, complex long double \
           : csinhl)(x)

#define cosh(x)                         \
  _Generic((x), float                   \
           : coshf, default             \
           : cosh, long double          \
           : coshl, complex float       \
           : ccoshf, complex double     \
           : ccosh, complex long double \
           : ccoshl)(x)

#define tanh(x)                         \
  _Generic((x), float                   \
           : tanhf, default             \
           : tanh, long double          \
           : tanhl, complex float       \
           : ctanhf, complex double     \
           : ctanh, complex long double \
           : ctanhl)(x)

#define asinh(x)                         \
  _Generic((x), float                    \
           : asinhf, default             \
           : asinh, long double          \
           : asinhl, complex float       \
           : casinhf, complex double     \
           : casinh, complex long double \
           : casinhl)(x)

#define acosh(x)                         \
  _Generic((x), float                    \
           : acoshf, default             \
           : acosh, long double          \
           : acoshl, complex float       \
           : cacoshf, complex double     \
           : cacosh, complex long double \
           : cacoshl)(x)

#define atanh(x)                         \
  _Generic((x), float                    \
           : atanhf, default             \
           : atanh, long double          \
           : atanhl, complex float       \
           : catanhf, complex double     \
           : catanh, complex long double \
           : catanhl)(x)

#define atan2(x, y) \
  _Generic((x), float : atan2f, default : atan2, long double : atan2l)(x, y)

#define cbrt(x) \
  _Generic((x), float : cbrtf, default : cbrt, long double : cbrtl)(x)

#define ceil(x) \
  _Generic((x), float : ceilf, default : ceil, long double : ceill)(x)

#define copysign(x, y)             \
  _Generic((x), float              \
           : copysignf, default    \
           : copysign, long double \
           : copysignl)(x, y)

#define erf(x) _Generic((x), float : erff, default : erf, long double : erfl)(x)

#define erfc(x) \
  _Generic((x), float : erfcf, default : erfc, long double : erfcl)(x)

#define exp2(x) \
  _Generic((x), float : exp2f, default : exp2, long double : exp2l)(x)

#define expm1(x) \
  _Generic((x), float : expm1f, default : expm1, long double : expm1l)(x)

#define fdim(x, y) \
  _Generic((x), float : fdimf, default : fdim, long double : fdiml)(x, y)

#define floor(x) \
  _Generic((x), float : floorf, default : floor, long double : floorl)(x)

#define fma(x, y, z) \
  _Generic((x), float : fmaf, default : fma, long double : fmal)(x, y, z)

#define fmax(x, y) \
  _Generic((x), float : fmaxf, default : fmax, long double : fmaxl)(x, y)

#define fmin(x, y) \
  _Generic((x), float : fminf, default : fmin, long double : fminl)(x, y)

#define fmod(x, y) \
  _Generic((x), float : fmodf, default : fmod, long double : fmodl)(x, y)

#define frexp(x, y) \
  _Generic((x), float : frexpf, default : frexp, long double : frexpl)(x, y)

#define hypot(x, y) \
  _Generic((x), float : hypotf, default : hypot, long double : hypotl)(x, y)

#define ilogb(x) \
  _Generic((x), float : ilogbf, default : ilogb, long double : ilogbl)(x)

#define ldexp(x, y) \
  _Generic((x), float : ldexpf, default : ldexp, long double : ldexpl)(x, y)

#define lgamma(x) \
  _Generic((x), float : lgammaf, default : lgamma, long double : lgammal)(x)

#define llrint(x) \
  _Generic((x), float : llrintf, default : llrint, long double : llrintl)(x)

#define llround(x) \
  _Generic((x), float : llroundf, default : llround, long double : llroundl)(x)

#define log10(x) \
  _Generic((x), float : log10f, default : log10, long double : log10l)(x)

#define log1p(x) \
  _Generic((x), float : log1pf, default : log1p, long double : log1pl)(x)

#define log2(x) \
  _Generic((x), float : log2f, default : log2, long double : log2l)(x)

#define logb(x) \
  _Generic((x), float : logbf, default : logb, long double : logbl)(x)

#define lrint(x) \
  _Generic((x), float : lrintf, default : lrint, long double : lrintl)(x)

#define lround(x) \
  _Generic((x), float : lroundf, default : lround, long double : lroundl)(x)

#define nearbyint(x)                \
  _Generic((x), float               \
           : nearbyintf, default    \
           : nearbyint, long double \
           : nearbyintl)(x)

#define nextafter(x, y)             \
  _Generic((x), float               \
           : nextafterf, default    \
           : nextafter, long double \
           : nextafterl)(x, y)

#define nexttoward(x, y)             \
  _Generic((x), float                \
           : nexttowardf, default    \
           : nexttoward, long double \
           : nexttowardl)(x, y)

#define remainder(x, y)             \
  _Generic((x), float               \
           : remainderf, default    \
           : remainder, long double \
           : remainderl)(x, y)

#define remquo(x, y, z)          \
  _Generic((x), float            \
           : remquof, default    \
           : remquo, long double \
           : remquol)(x, y, z)

#define rint(x) \
  _Generic((x), float : rintf, default : rint, long double : rintl)(x)

#define round(x) \
  _Generic((x), float : roundf, default : round, long double : roundl)(x)

#define scalbln(x, y)             \
  _Generic((x), float             \
           : scalblnf, default    \
           : scalbln, long double \
           : scalblnl)(x, y)

#define scalbn(x, y) \
  _Generic((x), float : scalbnf, default : scalbn, long double : scalbnl)(x, y)

#define tgamma(x) \
  _Generic((x), float : tgammaf, default : tgamma, long double : tgammal)(x)

#define trunc(x) \
  _Generic((x), float : truncf, default : trunc, long double : truncl)(x)

#define carg(x)                        \
  _Generic((x), complex float          \
           : cargf, default            \
           : carg, complex long double \
           : cargl)(x)

#define conj(x)                        \
  _Generic((x), complex float          \
           : conjf, default            \
           : conj, complex long double \
           : conjl)(x)

#undef creal
#define creal(x)                        \
  _Generic((x), complex float           \
           : crealf, default            \
           : creal, complex long double \
           : creall)(x)

#undef cimag
#define cimag(x)                        \
  _Generic((x), complex float           \
           : cimagf, default            \
           : cimag, complex long double \
           : cimagl)(x)

#define cproj(x)                        \
  _Generic((x), complex float           \
           : cprojf, default            \
           : cproj, complex long double \
           : cprojl)(x)

#endif /* C11 */
#endif /* _TGMATH_H */
