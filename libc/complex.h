#ifndef COSMOPOLITAN_LIBC_COMPLEX_H_
#define COSMOPOLITAN_LIBC_COMPLEX_H_
COSMOPOLITAN_C_START_
#if __STDC_VERSION__ + 0 >= 201112 && !defined(__STDC_NO_COMPLEX__)

#define complex   _Complex
#define imaginary _Imaginary

double cabs(complex double) libcesque;
double carg(complex double) libcesque;
double cimag(complex double) libcesque;
double creal(complex double) libcesque;

float cabsf(complex float) libcesque;
float cargf(complex float) libcesque;
float cimagf(complex float) libcesque;
float crealf(complex float) libcesque;

long double cabsl(complex long double) libcesque;
long double cargl(complex long double) libcesque;
long double cimagl(complex long double) libcesque;
long double creall(complex long double) libcesque;

complex double cacos(complex double) libcesque;
complex double cacosh(complex double) libcesque;
complex double casin(complex double) libcesque;
complex double casinh(complex double) libcesque;
complex double catan(complex double) libcesque;
complex double catanh(complex double) libcesque;
complex double ccos(complex double) libcesque;
complex double ccosh(complex double) libcesque;
complex double cexp(complex double) libcesque;
complex double cexp2(complex double) libcesque;
complex double clog(complex double) libcesque;
complex double conj(complex double) libcesque;
complex double cpow(complex double, complex double) libcesque;
complex double cproj(complex double) libcesque;
complex double csin(complex double) libcesque;
complex double csinh(complex double) libcesque;
complex double csqrt(complex double) libcesque;
complex double ctan(complex double) libcesque;
complex double ctanh(complex double) libcesque;

complex float cacosf(complex float) libcesque;
complex float cacoshf(complex float) libcesque;
complex float casinf(complex float) libcesque;
complex float casinhf(complex float) libcesque;
complex float catanf(complex float) libcesque;
complex float catanhf(complex float) libcesque;
complex float ccosf(complex float) libcesque;
complex float ccoshf(complex float) libcesque;
complex float cexpf(complex float) libcesque;
complex float cexp2f(complex float) libcesque;
complex float clogf(complex float) libcesque;
complex float conjf(complex float) libcesque;
complex float cpowf(complex float, complex float) libcesque;
complex float cprojf(complex float) libcesque;
complex float csinf(complex float) libcesque;
complex float csinhf(complex float) libcesque;
complex float csqrtf(complex float) libcesque;
complex float ctanf(complex float) libcesque;
complex float ctanhf(complex float) libcesque;

complex long double cprojl(complex long double) libcesque;
complex long double csinhl(complex long double) libcesque;
complex long double csinl(complex long double) libcesque;
complex long double csqrtl(complex long double) libcesque;
complex long double ctanhl(complex long double) libcesque;
complex long double ctanl(complex long double) libcesque;
complex long double cacoshl(complex long double) libcesque;
complex long double cacosl(complex long double) libcesque;
complex long double casinhl(complex long double) libcesque;
complex long double casinl(complex long double) libcesque;
complex long double catanhl(complex long double) libcesque;
complex long double catanl(complex long double) libcesque;
complex long double ccoshl(complex long double) libcesque;
complex long double ccosl(complex long double) libcesque;
complex long double cexpl(complex long double) libcesque;
complex long double cexp2l(complex long double) libcesque;
complex long double clogl(complex long double) libcesque;
complex long double conjl(complex long double) libcesque;
complex long double cpowl(complex long double, complex long double) libcesque;

#ifndef __cplusplus
#define __CIMAG(x, t)   \
  (+(union {            \
      _Complex t __z;   \
      t __xy[2];        \
    }){(_Complex t)(x)} \
        .__xy[1])
#define creal(x)  ((double)(x))
#define crealf(x) ((float)(x))
#define creall(x) ((long double)(x))
#define cimag(x)  __CIMAG(x, double)
#define cimagf(x) __CIMAG(x, float)
#define cimagl(x) __CIMAG(x, long double)
#endif

#ifdef __GNUC__
#define _Complex_I (__extension__(0.0f + 1.0fi))
#else
#define _Complex_I (0.0f + 1.0fi)
#endif

#ifdef _Imaginary_I
#define __CMPLX(x, y, t) ((t)(x) + _Imaginary_I * (t)(y))
#elif defined(__clang__)
#define __CMPLX(x, y, t) (+(_Complex t){(t)(x), (t)(y)})
#else
#define __CMPLX(x, y, t) (__builtin_complex((t)(x), (t)(y)))
#endif

#define CMPLX(x, y)  __CMPLX(x, y, double)
#define CMPLXF(x, y) __CMPLX(x, y, float)
#define CMPLXL(x, y) __CMPLX(x, y, long double)

#endif /* C11 */
COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_COMPLEX_H_ */
