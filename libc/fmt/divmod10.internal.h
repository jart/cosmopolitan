#ifndef COSMOPOLITAN_LIBC_FMT_DIVMOD10_H_
#define COSMOPOLITAN_LIBC_FMT_DIVMOD10_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline uint64_t DivMod10(uint64_t x, unsigned *r) {
#if defined(__STRICT_ANSI__) || !defined(__GNUC__) || \
    (defined(__OPTIMIZE__) && !defined(__OPTIMIZE_SIZE__))
  *r = x % 10;
  return x / 10;
#else
  uint128_t dw;
  unsigned long long hi, rm;
  dw = x;
  dw *= 0xcccccccccccccccdull;
  hi = dw >> 64;
  hi >>= 3;
  rm = hi;
  rm += rm << 2;
  rm += rm;
  *r = x - rm;
  return hi;
#endif
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_FMT_DIVMOD10_H_ */
