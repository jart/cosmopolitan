#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int _bsr(int) pureconst;
int _bsrl(long) pureconst;
int _bsrll(long long) pureconst;

#if defined(__GNUC__) && defined(__x86_64__) && !defined(__STRICT_ANSI__)
int _bsr128(uint128_t) pureconst;
#define _bsr(u)                                                 \
  ({                                                            \
    unsigned BiTs;                                              \
    asm("bsr\t%0,%0" : "=r"(BiTs) : "0"((unsigned)(u)) : "cc"); \
    BiTs;                                                       \
  })
#define _bsrl(u)                                                     \
  ({                                                                 \
    unsigned long BiTs;                                              \
    asm("bsr\t%0,%0" : "=r"(BiTs) : "0"((unsigned long)(u)) : "cc"); \
    (unsigned)BiTs;                                                  \
  })
#define _bsrll(u) _bsrl(u)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_ */
