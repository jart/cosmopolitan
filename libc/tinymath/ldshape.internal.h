#ifndef COSMOPOLITAN_LIBC_TINYMATH_LDSHAPE_INTERNAL_H_
#define COSMOPOLITAN_LIBC_TINYMATH_LDSHAPE_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

union ldshape {
  long double f;
  struct {
    uint64_t m;
    uint16_t se;
  } i;
};

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_TINYMATH_LDSHAPE_INTERNAL_H_ */
