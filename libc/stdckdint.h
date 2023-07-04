#ifndef COSMOPOLITAN_LIBC_STDCKDINT_H_
#define COSMOPOLITAN_LIBC_STDCKDINT_H_

#define __STDC_VERSION_STDCKDINT_H__ 202311L

#define ckd_add(res, x, y) __builtin_add_overflow((x), (y), (res))
#define ckd_sub(res, x, y) __builtin_sub_overflow((x), (y), (res))
#define ckd_mul(res, x, y) __builtin_mul_overflow((x), (y), (res))

#endif /* COSMOPOLITAN_LIBC_STDCKDINT_H_ */
