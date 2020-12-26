#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_FFS_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_FFS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/*
 * BIT SCANNING 101
 *                          ctz(𝑥)         31^clz(𝑥)   clz(𝑥)
 *      uint32 𝑥   bsf(𝑥) tzcnt(𝑥)   ffs(𝑥)   bsr(𝑥) lzcnt(𝑥)
 *    0x00000000      wut       32        0      wut       32
 *    0x00000001        0        0        1        0       31
 *    0x80000001        0        0        1       31        0
 *    0x80000000       31       31       32       31        0
 *    0x00000010        4        4        5        4       27
 *    0x08000010        4        4        5       27        4
 *    0x08000000       27       27       28       27        4
 *    0xffffffff        0        0        1       31        0
 */

int ffs(int) pureconst;
int ffsl(long) pureconst;
int ffsll(long long) pureconst;

#ifdef __GNUC__
#define ffs(u)   __builtin_ffs(u)
#define ffsl(u)  __builtin_ffsl(u)
#define ffsll(u) __builtin_ffsll(u)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_FFS_H_ */
