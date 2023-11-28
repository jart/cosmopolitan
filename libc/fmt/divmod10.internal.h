#ifndef COSMOPOLITAN_LIBC_FMT_DIVMOD10_H_
#define COSMOPOLITAN_LIBC_FMT_DIVMOD10_H_

forceinline uint64_t __divmod10(uint64_t __x, unsigned *__r) {
#if defined(__STRICT_ANSI__) || !defined(__GNUC__) || \
    (defined(__OPTIMIZE__) && !defined(__OPTIMIZE_SIZE__))
  *__r = __x % 10;
  return __x / 10;
#else
  unsigned __int128 __dw;
  unsigned long long __hi, __rm;
  __dw = __x;
  __dw *= 0xcccccccccccccccdull;
  __hi = __dw >> 64;
  __hi >>= 3;
  __rm = __hi;
  __rm += __rm << 2;
  __rm += __rm;
  *__r = __x - __rm;
  return __hi;
#endif
}

#endif /* COSMOPOLITAN_LIBC_FMT_DIVMOD10_H_ */
