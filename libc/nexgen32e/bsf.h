#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_BSF_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_BSF_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

unsigned bsf(unsigned) libcesque pureconst;
unsigned bsfl(unsigned long) libcesque pureconst;
unsigned bsfmax(uintmax_t) libcesque pureconst;

#define bsf(X)                                   \
  ({                                             \
    unsigned Res;                                \
    typeof(X) Word;                              \
    asm("bsf\t%1,%0" : "=r,r"(Word) : "r,m"(X)); \
    Res = Word;                                  \
    Res;                                         \
  })

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_BSF_H_ */
