#ifndef COSMOPOLITAN_LIBC_TINYMATH_COMPLEX_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_COMPLEX_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

_Complex double __ldexp_cexp(_Complex double, int) hidden;
_Complex float __ldexp_cexpf(_Complex float, int) hidden;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TINYMATH_COMPLEX_INTERNAL_H_ */
