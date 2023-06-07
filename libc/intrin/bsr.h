#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int _bsr(int) pureconst;
int _bsrl(long) pureconst;
int _bsrll(long long) pureconst;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define _bsr(x)   (__builtin_clz(x) ^ (sizeof(int) * CHAR_BIT - 1))
#define _bsrl(x)  (__builtin_clzl(x) ^ (sizeof(long) * CHAR_BIT - 1))
#define _bsrll(x) (__builtin_clzll(x) ^ (sizeof(long long) * CHAR_BIT - 1))
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_ */
