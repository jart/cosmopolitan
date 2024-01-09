#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_
COSMOPOLITAN_C_START_

libcesque int _bsr(int) pureconst;
libcesque int _bsrl(long) pureconst;
libcesque int _bsrll(long long) pureconst;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define _bsr(x)   (__builtin_clz(x) ^ (sizeof(int) * 8 - 1))
#define _bsrl(x)  (__builtin_clzl(x) ^ (sizeof(long) * 8 - 1))
#define _bsrll(x) (__builtin_clzll(x) ^ (sizeof(long long) * 8 - 1))
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_ */
