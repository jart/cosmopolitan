#ifndef COSMOPOLITAN_LIBC_TINYMATH_TINYMATH_H_
#define COSMOPOLITAN_LIBC_TINYMATH_TINYMATH_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

float tinymath_powf(float, float) libcesque pureconst;
double tinymath_pow(double, double) libcesque pureconst;
long double tinymath_powl(long double, long double) libcesque pureconst;

float tinymath_expf(float) libcesque pureconst;
double tinymath_exp(double) libcesque pureconst;
long double tinymath_expl(long double) libcesque pureconst;

float tinymath_exp10f(float) libcesque pureconst;
double tinymath_exp10(double) libcesque pureconst;
long double tinymath_exp10l(long double) libcesque pureconst;

float tinymath_logf(float) libcesque pureconst;
double tinymath_log(double) libcesque pureconst;
long double tinymath_logl(long double) libcesque pureconst;

float tinymath_log10f(float) libcesque pureconst;
double tinymath_log10(double) libcesque pureconst;
long double tinymath_log10l(long double) libcesque pureconst;

float tinymath_ldexpf(float, int) libcesque pureconst;
double tinymath_ldexp(double, int) libcesque pureconst;
long double tinymath_ldexpl(long double, int) libcesque pureconst;

float tinymath_asinf(float) libcesque pureconst;
double tinymath_asin(double) libcesque pureconst;
long double tinymath_asinl(long double) libcesque pureconst;

float tinymath_acosf(float) libcesque pureconst;
double tinymath_acos(double) libcesque pureconst;
long double tinymath_acosl(long double) libcesque pureconst;

float tinymath_atanf(float) libcesque pureconst;
double tinymath_atan(double) libcesque pureconst;
long double tinymath_atanl(long double) libcesque pureconst;

float tinymath_atan2f(float, float) libcesque pureconst;
double tinymath_atan2(double, double) libcesque pureconst;
long double tinymath_atan2l(long double, long double) libcesque pureconst;

float tinymath_sinf(float) libcesque pureconst;
double tinymath_sin(double) libcesque pureconst;
long double tinymath_sinl(long double) libcesque pureconst;

float tinymath_cosf(float) libcesque pureconst;
double tinymath_cos(double) libcesque pureconst;
long double tinymath_cosl(long double) libcesque pureconst;

float tinymath_tanf(float) libcesque pureconst;
double tinymath_tan(double) libcesque pureconst;
long double tinymath_tanl(long double) libcesque pureconst;

void tinymath_sincosf(float, float *, float *) libcesque;
void tinymath_sincos(double, double *, double *) libcesque;
void tinymath_sincosl(long double, long double *, long double *) libcesque;

long tinymath_lroundf(float) libcesque pureconst;
long tinymath_lround(double) libcesque pureconst;
long tinymath_lroundl(long double) libcesque pureconst;

float tinymath_roundf(float) libcesque pureconst;
double tinymath_round(double) libcesque pureconst;
long double tinymath_roundl(long double) libcesque pureconst;

float tinymath_rintf(float) libcesque pureconst;
double tinymath_rint(double) libcesque pureconst;
long double tinymath_rintl(long double) libcesque pureconst;

float tinymath_nearbyintf(float) libcesque pureconst;
double tinymath_nearbyint(double) libcesque pureconst;
long double tinymath_nearbyintl(long double) libcesque pureconst;

float tinymath_truncf(float) libcesque pureconst;
double tinymath_trunc(double) libcesque pureconst;
long double tinymath_truncl(long double) libcesque pureconst;

float tinymath_floorf(float) libcesque pureconst;
double tinymath_floor(double) libcesque pureconst;
long double tinymath_floorl(long double) libcesque pureconst;

float tinymath_ceilf(float) libcesque pureconst;
double tinymath_ceil(double) libcesque pureconst;
long double tinymath_ceill(long double) libcesque pureconst;

float tinymath_remainderf(float, float) libcesque pureconst;
double tinymath_remainder(double, double) libcesque pureconst;
long double tinymath_remainderl(long double, long double) libcesque pureconst;

float tinymath_fmodf(float, float) libcesque pureconst;
double tinymath_fmod(double, double) libcesque pureconst;
long double tinymath_fmodl(long double, long double) libcesque pureconst;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TINYMATH_TINYMATH_H_ */
