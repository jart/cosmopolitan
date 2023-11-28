#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_BSF_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_BSF_H_
COSMOPOLITAN_C_START_

int _bsf(int) pureconst;
int _bsfl(long) pureconst;
int _bsfll(long long) pureconst;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define _bsf(x)   __builtin_ctz(x)
#define _bsfl(x)  __builtin_ctzl(x)
#define _bsfll(x) __builtin_ctzll(x)
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_BSF_H_ */
