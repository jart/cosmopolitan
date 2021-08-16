#ifndef COSMOPOLITAN_LIBC_TINYMATH_FEVAL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_FEVAL_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

static inline void fevalf(float x) {
  volatile float y = x;
}

static inline void feval(double x) {
  volatile double y = x;
}

static inline void fevall(long double x) {
  volatile long double y = x;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TINYMATH_FEVAL_INTERNAL_H_ */
