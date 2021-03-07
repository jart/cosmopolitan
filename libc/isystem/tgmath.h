#ifndef LIBC_ISYSTEM_TGMATH_H_
#define LIBC_ISYSTEM_TGMATH_H_

#include "libc/math.h"

#if !(__ASSEMBLER__ + __LINKER__ + 0)
#if __STDC_VERSION__ + 0 >= 201112
COSMOPOLITAN_C_START_

/* from https://en.cppreference.com/w/c/numeric/tgmath */

#define fabs(x) \
  _Generic((x), float : fabsf, default : fabs, long double : fabsl)(x)

#define exp(x) _Generic((x), float : expf, default : exp, long double : expl)(x)

#define log(x) _Generic((x), float : logf, default : log, long double : logl)(x)

#define pow(x, y) \
  _Generic((x), float : powf, default : pow, long double : powl)(x, y)

#define sqrt(x) \
  _Generic((x), float : sqrtf, default : sqrt, long double : sqrtl)(x)

#define sin(x) _Generic((x), float : sinf, default : sin, long double : sinl)(x)

#define cos(x) _Generic((x), float : cosf, default : cos, long double : cosl)(x)

#define tan(x) _Generic((x), float : tanf, default : tan, long double : tanl)(x)

#define asin(x) \
  _Generic((x), float : asinf, default : asin, long double : asinl)(x)

#define acos(x) \
  _Generic((x), float : acosf, default : acos, long double : acosl)(x)

#define atan(x) \
  _Generic((x), float : atanf, default : atan, long double : atanl)(x)

#define sinh(x) \
  _Generic((x), float : sinhf, default : sinh, long double : sinhl)(x)

#define cosh(x) \
  _Generic((x), float : coshf, default : cosh, long double : coshl)(x)

#define tanh(x) \
  _Generic((x), float : tanhf, default : tanh, long double : tanhl)(x)

#define asinh(x) \
  _Generic((x), float : asinhf, default : asinh, long double : asinhl)(x)

#define acosh(x) \
  _Generic((x), float : acoshf, default : acosh, long double : acoshl)(x)

#define atanh(x) \
  _Generic((x), float : atanhf, default : atanh, long double : atanhl)(x)

COSMOPOLITAN_C_END_

#endif /* C11 */
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */

#endif
