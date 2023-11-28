#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_FFS_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_FFS_H_
COSMOPOLITAN_C_START_

int ffs(int) pureconst;
int ffsl(long) pureconst;
int ffsll(long long) pureconst;

#ifdef __GNUC__
#define ffs(u)   __builtin_ffs(u)
#define ffsl(u)  __builtin_ffsl(u)
#define ffsll(u) __builtin_ffsll(u)
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_FFS_H_ */
