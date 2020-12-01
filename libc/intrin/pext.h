#ifndef COSMOPOLITAN_LIBC_INTRIN_PEXT_H_
#define COSMOPOLITAN_LIBC_INTRIN_PEXT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

uint64_t pext(uint64_t, uint64_t) pureconst;

#define PEXT(NUMBER, BITMASK)                                                \
  ({                                                                         \
    typeof(BITMASK) ShuffledBits, Number = (NUMBER);                         \
    asm("pext\t%2,%1,%0" : "=r"(ShuffledBits) : "r"(Number), "rm"(BITMASK)); \
    ShuffledBits;                                                            \
  })

#define pext(NUMBER, BITMASK) \
  (!X86_HAVE(BMI2) ? pext(NUMBER, BITMASK) : PEXT(NUMBER, BITMASK))

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PEXT_H_ */
