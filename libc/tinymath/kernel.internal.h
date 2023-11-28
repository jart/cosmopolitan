#ifndef COSMOPOLITAN_LIBC_TINYMATH_KERNEL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_KERNEL_INTERNAL_H_
COSMOPOLITAN_C_START_

double __cos(double, double);
double __sin(double, double, int);
double __tan(double, double, int);
float __cosdf(double);
float __sindf(double);
float __tandf(double, int);
long double __cosl(long double, long double);
long double __sinl(long double, long double, int);
long double __tanl(long double, long double, int);
int __rem_pio2(double, double *);
int __rem_pio2l(long double, long double *);
int __rem_pio2_large(double *, double *, int, int, int);
int __rem_pio2f(float, double *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TINYMATH_KERNEL_INTERNAL_H_ */
