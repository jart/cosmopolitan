#ifndef COSMOPOLITAN_LIBC_BITS_BSWAP_H_
#define COSMOPOLITAN_LIBC_BITS_BSWAP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

uint16_t bswap_16(uint16_t) pureconst;
uint32_t bswap_32(uint32_t) pureconst;
uint32_t bswap_64(uint32_t) pureconst;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define bswap_16(x) __builtin_bswap16(x)
#define bswap_32(x) __builtin_bswap32(x)
#define bswap_64(x) __builtin_bswap64(x)
#endif /* defined(__GNUC__) && !defined(__STRICT_ANSI__) */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_BSWAP_H_ */
