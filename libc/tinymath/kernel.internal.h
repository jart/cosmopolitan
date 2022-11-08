#ifndef COSMOPOLITAN_LIBC_TINYMATH_KERNEL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_KERNEL_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

double __cos(double, double) _Hide;
double __sin(double, double, int) _Hide;
double __tan(double, double, int) _Hide;
float __cosdf(double) _Hide;
float __sindf(double) _Hide;
float __tandf(double, int) _Hide;
int __rem_pio2(double, double *) _Hide;
int __rem_pio2_large(double *, double *, int, int, int) _Hide;
int __rem_pio2f(float, double *) _Hide;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TINYMATH_KERNEL_INTERNAL_H_ */
