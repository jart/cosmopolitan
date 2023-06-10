#ifndef COSMOPOLITAN_LIBC_STDCKDINT_H_
#define COSMOPOLITAN_LIBC_STDCKDINT_H_

#define ckd_add(R, A, B) __builtin_add_overflow((A), (B), (R))
#define ckd_sub(R, A, B) __builtin_sub_overflow((A), (B), (R))
#define ckd_mul(R, A, B) __builtin_mul_overflow((A), (B), (R))

#endif /* COSMOPOLITAN_LIBC_STDCKDINT_H_ */
