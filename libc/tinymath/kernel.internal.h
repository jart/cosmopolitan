#ifndef COSMOPOLITAN_LIBC_TINYMATH_KERNEL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_KERNEL_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

double __sin(double, double, int);
double __tan(double, double, int);
double __cos(double, double);
int __rem_pio2(double, double *);
int __rem_pio2_large(double *, double *, int, int, int);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TINYMATH_KERNEL_INTERNAL_H_ */
