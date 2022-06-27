#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_BSF_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_BSF_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int bsf(int) pureconst;
int bsfl(long) pureconst;
int bsfll(long long) pureconst;
int bsf128(uintmax_t) pureconst;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define bsf(u)                                                  \
  ({                                                            \
    unsigned BiTs;                                              \
    asm("bsf\t%0,%0" : "=r"(BiTs) : "0"((unsigned)(u)) : "cc"); \
    BiTs;                                                       \
  })
#define bsfl(u)                                                      \
  ({                                                                 \
    unsigned long BiTs;                                              \
    asm("bsf\t%0,%0" : "=r"(BiTs) : "0"((unsigned long)(u)) : "cc"); \
    (unsigned)BiTs;                                                  \
  })
#define bsfll(u) bsfl(u)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_BSF_H_ */
