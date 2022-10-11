#ifndef COSMOPOLITAN_LIBC_TINYMATH_KERNEL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_KERNEL_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

double __cos(double, double) hidden;
double __sin(double, double, int) hidden;
double __tan(double, double, int) hidden;
float __cosdf(double) hidden;
float __sindf(double) hidden;
float __tandf(double, int) hidden;
int __rem_pio2(double, double *) hidden;
int __rem_pio2_large(double *, double *, int, int, int) hidden;
int __rem_pio2f(float, double *) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TINYMATH_KERNEL_INTERNAL_H_ */
