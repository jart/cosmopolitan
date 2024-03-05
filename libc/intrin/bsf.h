#ifdef _COSMO_SOURCE
#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_BSF_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_BSF_H_
COSMOPOLITAN_C_START_

libcesque int bsf(int) pureconst;
libcesque int bsfl(long) pureconst;
libcesque int bsfll(long long) pureconst;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define bsf(x)   __builtin_ctz(x)
#define bsfl(x)  __builtin_ctzl(x)
#define bsfll(x) __builtin_ctzll(x)
#endif

/* deprecated */
#define _bsf(x)   bsf(x)
#define _bsfl(x)  bsfl(x)
#define _bsfll(x) bsfll(x)

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_BSF_H_ */
#endif /* _COSMO_SOURCE */
