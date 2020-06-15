#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

unsigned bsr(unsigned) libcesque pureconst;
unsigned bsrl(unsigned long) libcesque pureconst;
unsigned bsrmax(uintmax_t) libcesque pureconst;

#ifdef __GNUC__
#define bsr(X)                                    \
  ({                                              \
    unsigned Word;                                \
    asm("bsrl\t%1,%0" : "=r,r"(Word) : "r,m"(X)); \
    Word;                                         \
  })
#define bsrl(X)                                   \
  ({                                              \
    unsigned Res;                                 \
    unsigned long Word;                           \
    asm("bsrq\t%1,%0" : "=r,r"(Word) : "r,m"(X)); \
    Res = Word;                                   \
    Res;                                          \
  })
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_BSR_H_ */
