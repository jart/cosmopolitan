#ifndef COSMOPOLITAN_LIBC_TINYMATH_FEVAL_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_FEVAL_INTERNAL_H_
COSMOPOLITAN_C_START_

static inline void fevalf(float x) {
  volatile float y = x;
  (void)y;
}

static inline void feval(double x) {
  volatile double y = x;
  (void)y;
}

static inline void fevall(long double x) {
  volatile long double y = x;
  (void)y;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_TINYMATH_FEVAL_INTERNAL_H_ */
