#ifndef COSMOPOLITAN_LIBC_BITS_BSWAP_H_
#define COSMOPOLITAN_LIBC_BITS_BSWAP_H_
COSMOPOLITAN_C_START_

libcesque uint16_t bswap_16(uint16_t) pureconst;
libcesque uint32_t bswap_32(uint32_t) pureconst;
libcesque uint64_t bswap_64(uint64_t) pureconst;

#ifdef _COSMO_SOURCE
#if (__GNUC__ + 0) * 100 + (__GNUC_MINOR__ + 0) >= 406 || defined(__llvm__)
libcesque uint128_t bswap_128(uint128_t) pureconst;
#if !defined(__STRICT_ANSI__)
#define bswap_128(x) __builtin_bswap128(x)
#endif /* !defined(__STRICT_ANSI__) */
#endif /* GCC 4.6+ or LLVM */
#endif /* _COSMO_SOURCE */

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define bswap_16(x) __builtin_bswap16(x)
#define bswap_32(x) __builtin_bswap32(x)
#define bswap_64(x) __builtin_bswap64(x)
#endif /* defined(__GNUC__) && !defined(__STRICT_ANSI__) */

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_BITS_BSWAP_H_ */
