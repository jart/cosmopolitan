#ifndef COSMOPOLITAN_LIBC_INTRIN_PEXT_H_
#define COSMOPOLITAN_LIBC_INTRIN_PEXT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/* TODO(jart): Implement polyfill. */
#define pext(NUMBER, BITMASK)                                                \
  ({                                                                         \
    typeof(BITMASK) ShuffledBits, Number = (NUMBER);                         \
    asm("pext\t%2,%1,%0" : "=r"(ShuffledBits) : "r"(Number), "rm"(BITMASK)); \
    ShuffledBits;                                                            \
  })

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PEXT_H_ */
