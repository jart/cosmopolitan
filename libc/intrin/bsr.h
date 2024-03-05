#ifdef _COSMO_SOURCE
#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_
COSMOPOLITAN_C_START_

libcesque int bsr(int) pureconst;
libcesque int bsrl(long) pureconst;
libcesque int bsrll(long long) pureconst;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define bsr(x)   (__builtin_clz(x) ^ (sizeof(int) * 8 - 1))
#define bsrl(x)  (__builtin_clzl(x) ^ (sizeof(long) * 8 - 1))
#define bsrll(x) (__builtin_clzll(x) ^ (sizeof(long long) * 8 - 1))
#endif

/* deprecated */
#define _bsr(x)   bsr(x)
#define _bsrl(x)  bsrl(x)
#define _bsrll(x) bsrll(x)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_ */
#endif /* _COSMO_SOURCE */
