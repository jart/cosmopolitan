#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_BSF_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_BSF_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

int _bsf(int) pureconst;
int _bsfl(long) pureconst;
int _bsfll(long long) pureconst;
int _bsf128(uintmax_t) pureconst;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define _bsf(u)                                                 \
  ({                                                            \
    unsigned BiTs;                                              \
    asm("bsf\t%0,%0" : "=r"(BiTs) : "0"((unsigned)(u)) : "cc"); \
    BiTs;                                                       \
  })
#define _bsfl(u)                                                     \
  ({                                                                 \
    unsigned long BiTs;                                              \
    asm("bsf\t%0,%0" : "=r"(BiTs) : "0"((unsigned long)(u)) : "cc"); \
    (unsigned)BiTs;                                                  \
  })
#define _bsfll(u) _bsfl(u)
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_BSF_H_ */
