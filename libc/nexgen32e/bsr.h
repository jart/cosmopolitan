#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int bsr(int) pureconst;
int bsrl(long) pureconst;
int bsrll(long long) pureconst;
int bsr128(uint128_t) pureconst;

#if defined(__GNUC__) && defined(__x86_64__) && !defined(__STRICT_ANSI__)
#define bsr(u)                                                  \
  ({                                                            \
    unsigned BiTs;                                              \
    asm("bsr\t%0,%0" : "=r"(BiTs) : "0"((unsigned)(u)) : "cc"); \
    BiTs;                                                       \
  })
#define bsrl(u)                                                      \
  ({                                                                 \
    unsigned long BiTs;                                              \
    asm("bsr\t%0,%0" : "=r"(BiTs) : "0"((unsigned long)(u)) : "cc"); \
    (unsigned)BiTs;                                                  \
  })
#define bsrll(u) bsrl(u)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_ */
