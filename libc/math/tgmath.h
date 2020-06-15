#if __INCLUDE_LEVEL__ + 0 > 1
#error "jart asks please don't #include tgmath.h from header"
#endif
#ifndef COSMOPOLITAN_LIBC_MATH_TGMATH_H_
#define COSMOPOLITAN_LIBC_MATH_TGMATH_H_
#include "libc/math.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
#if __STDC_VERSION__ + 0 >= 201112

/*─────────────────────────────────────────────────────────────────────────────╗
│ cosmopolitan § lazy mathematics                                              │
╚─────────────────────────────────────────────────────────────────────────────*/

#define pow(X, Z)                   \
  _Generic((Z), complex long double \
           : cpowl, complex float   \
           : cpowf, complex double  \
           : cpow, long double      \
           : powl, float            \
           : powf, default          \
           : pow)(X, Z)

#define sqrt(X)                     \
  _Generic((X), complex long double \
           : csqrtl, complex float  \
           : csqrtf, complex double \
           : csqrt, long double     \
           : sqrtl, float           \
           : sqrtf, double          \
           : sqrt, default          \
           : sqrt)(X)

#define exp(X)                      \
  _Generic((X), complex long double \
           : cexpl, complex float   \
           : cexpf, complex double  \
           : cexp, long double      \
           : expl, float            \
           : expf, default          \
           : exp)(X)

#define exp2(X)                     \
  _Generic((X), complex long double \
           : cexp2l, complex float  \
           : cexp2f, complex double \
           : cexp2, long double     \
           : exp2l, float           \
           : exp2f, default         \
           : exp2)(X)

#define sin(X)                      \
  _Generic((X), complex long double \
           : csinl, complex float   \
           : csinf, complex double  \
           : csin, long double      \
           : sinl, float            \
           : sinf, default          \
           : sin)(X)

#define sinh(X)                     \
  _Generic((X), complex long double \
           : csinhl, complex float  \
           : csinhf, complex double \
           : csinh, long double     \
           : sinhl, float           \
           : sinhf, default         \
           : sinh)(X)

#define asin(X)                     \
  _Generic((X), complex long double \
           : casinl, complex float  \
           : casinf, complex double \
           : casin, long double     \
           : asinl, float           \
           : asinf, default         \
           : asin)(X)

#define asinh(X)                     \
  _Generic((X), complex long double  \
           : casinhl, complex float  \
           : casinhf, complex double \
           : casinh, long double     \
           : asinhl, float           \
           : asinhf, default         \
           : asinh)(X)

#define cos(X)                      \
  _Generic((X), complex long double \
           : ccosl, complex float   \
           : ccosf, complex double  \
           : ccos, long double      \
           : cosl, float            \
           : cosf, default          \
           : cos)(X)

#define cosh(X)                     \
  _Generic((X), complex long double \
           : ccoshl, complex float  \
           : ccoshf, complex double \
           : ccosh, long double     \
           : coshl, float           \
           : coshf, default         \
           : cosh)(X)

#define acos(X)                     \
  _Generic((X), complex long double \
           : cacosl, complex double \
           : cacos, complex float   \
           : cacosf, long double    \
           : acosl, float           \
           : acosf, double          \
           : acos)(X)

#define acosh(X)                     \
  _Generic((X), complex long double  \
           : cacoshl, complex float  \
           : cacoshf, complex double \
           : cacosh, long double     \
           : acoshl, float           \
           : acoshf, default         \
           : acosh)(X)

#define tan(X)                      \
  _Generic((X), complex long double \
           : ctanl, complex float   \
           : ctanf, complex double  \
           : ctan, long double      \
           : tanl, float            \
           : tanf, default          \
           : tan)(X)

#define tanh(X)                     \
  _Generic((X), complex long double \
           : ctanhl, complex float  \
           : ctanhf, complex double \
           : ctanh, long double     \
           : tanhl, float           \
           : tanhf, default         \
           : tanh)(X)

#define atan(X)                     \
  _Generic((X), complex long double \
           : catanl, complex float  \
           : catanf, complex double \
           : catan, long double     \
           : atanl, float           \
           : atanf, default         \
           : atan)(X)

#define atanh(X)                     \
  _Generic((X), complex long double  \
           : catanhl, complex float  \
           : catanhf, complex double \
           : catanh, long double     \
           : atanhl, float           \
           : atanhf, default         \
           : atanh)(X)

#define atan2(X, Y) \
  _Generic((X), long double : atan2l, float : atan2f, default : atan2)(X, Y)

#define log(X)                      \
  _Generic((X), complex long double \
           : clogl, complex float   \
           : clogf, complex double  \
           : clog, long double      \
           : logl, float            \
           : logf, default          \
           : log)(X)

#define onj(Z)                      \
  _Generic((Z), complex long double \
           : conjl, complex float   \
           : conjf, complex double  \
           : conj)(Z)

#define proj(Z)                     \
  _Generic((Z), complex long double \
           : cprojl, complex float  \
           : cprojf, complex double \
           : cproj)(Z)

#define real(Z)                     \
  _Generic((Z), complex long double \
           : creall, complex float  \
           : crealf, complex double \
           : creal)(Z)

#define carg(Z)                     \
  _Generic((Z), complex long double \
           : cargl, complex float   \
           : cargf, default         \
           : carg)(Z)

#define cimag(Z)                    \
  _Generic((Z), complex long double \
           : cimagl, complex float  \
           : cimagf, complex double \
           : cimag)(Z)

#define nearbyint(X)             \
  _Generic((X), long double      \
           : nearbyintl, float   \
           : nearbyintf, default \
           : nearbyint)(X)

#define nextafter(X, Y)          \
  _Generic((X), long double      \
           : nextafterl, float   \
           : nextafterf, default \
           : nextafter)(X, Y)

#define nexttoward(X, Y)          \
  _Generic((X), long double       \
           : nexttowardl, float   \
           : nexttowardf, default \
           : nexttoward)(X, Y)

#define remainder(X, Y)          \
  _Generic((X), long double      \
           : remainderl, float   \
           : remainderf, default \
           : remainder)(X, Y)

#define remquo(X, Y, Q)       \
  _Generic((X), long double   \
           : remquol, float   \
           : remquof, default \
           : remquo)(X, Y, Q)

#define pow10(X) \
  _Generic((X), long double : pow10l, float : pow10f, default : pow10)(X)
#define powi(X, Y) \
  _Generic((X), long double : powil, float : powif, default : powi)(X, Y)
#define exp10(X) \
  _Generic((X), long double : exp10l, float : exp10f, default : exp10)(X)
#define log10(X) \
  _Generic((X), long double : log10l, float : log10f, default : log10)(X)
#define log1p(X) \
  _Generic((X), long double : log1pl, float : log1pf, default : log1p)(X)
#define log2(X) \
  _Generic((X), long double : log2l, float : log2f, default : log2)(X)
#define logb(X) \
  _Generic((X), long double : logbl, float : logbf, default : logb)(X)
#define expm1(X) \
  _Generic((X), long double : expm1l, float : expm1f, default : expm1)(X)
#define cbrt(X) \
  _Generic((X), long double : cbrtl, float : cbrtf, default : cbrt)(X)

#define fabs(X) \
  _Generic((X), long double : fabsl, float : fabsf, default : fabs)(X)

#define fmax(X, Y) \
  _Generic((X), long double : fmaxl, float : fmaxf, default : fmax)(X, Y)
#define fmin(X, Y) \
  _Generic((X), long double : fminl, float : fminf, default : fmin)(X, Y)
#define fmod(X, Y) \
  _Generic((X), long double : fmodl, float : fmodf, default : fmod)(X, Y)
#define frexp(X, Y) \
  _Generic((X), long double : frexpl, float : frexpf, default : frexp)(X, Y)
#define gamma(X) \
  _Generic((X), long double : gammal, float : gammaf, default : gamma)(X)
#define hypot(X, Y) \
  _Generic((X), long double : hypotl, float : hypotf, default : hypot)(X, Y)
#define modf(X, Y) \
  _Generic((X), long double : modfl, float : modff, default : modf)(X, Y)

#define rint(X) \
  _Generic((X), long double : rintl, float : rintf, default : rint)(X)
#define ceil(X) \
  _Generic((X), long double : ceill, float : ceilf, default : ceil)(X)
#define floor(X) \
  _Generic((X), long double : floorl, float : floorf, default : floor)(X)
#define drem(X, Y) \
  _Generic((X), long double : dreml, float : dremf, default : drem)(X, Y)
#define erf(X) _Generic((X), long double : erfl, float : erff, default : erf)(X)
#define erfc(X) \
  _Generic((X), long double : erfcl, float : erfcf, default : erfc)(X)
#define fdim(X, Y) \
  _Generic((X), long double : fdiml, float : fdimf, default : fdim)(X, Y)

#define j0(X) _Generic((X), long double : j0l, float : j0f, default : j0)(X)
#define j1(X) _Generic((X), long double : j1l, float : j1f, default : j1)(X)
#define jn(N, X) \
  _Generic((X), long double : jnl, float : jnf, default : jn)(N, X)
#define ldexp(X, N) \
  _Generic((X), long double : ldexpl, float : ldexpf, default : ldexp)(X, N)
#define lgamma(X) \
  _Generic((X), long double : lgammal, float : lgammaf, default : lgamma)(X)
#define round(X) \
  _Generic((X), long double : roundl, float : roundf, default : round)(X)
#define scalb(X, Y) \
  _Generic((X), long double : scalbl, float : scalbf, default : scalb)(X, Y)
#define scalbln(X, Y)          \
  _Generic((X), long double    \
           : scalblnl, default \
           : scalbln, float    \
           : scalblnf)(X, Y)
#define scalbn(X, Y) \
  _Generic((X), long double : scalbnl, float : scalbnf, default : scalbn)(X, Y)
#define significand(X)             \
  _Generic((X), long double        \
           : significandl, default \
           : significand, float    \
           : significandf)(X)
#define copysign(X)             \
  _Generic((X), long double     \
           : copysignl, default \
           : copysign, float    \
           : copysignf)(X)
#define tgamma(X) \
  _Generic((X), long double : tgammal, float : tgammaf, default : tgamma)(X)
#define trunc(X) \
  _Generic((X), long double : truncl, float : truncf, default : trunc)(X)
#define y0(X) _Generic((X), long double : y0l, float : y0f, default : y0)(X)
#define y1(X) _Generic((X), long double : y1l, float : y1f, default : y1)(X)
#define yn(N, X) \
  _Generic((X), long double : ynl, float : ynf, default : yn)(N, X)

#endif /* C11 */
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_MATH_TGMATH_H_ */
