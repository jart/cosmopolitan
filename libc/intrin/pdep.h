#ifndef COSMOPOLITAN_LIBC_INTRIN_PDEP_H_
#define COSMOPOLITAN_LIBC_INTRIN_PDEP_H_
#include "libc/nexgen32e/x86feature.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

uint64_t pdep(uint64_t, uint64_t) pureconst;

#define PDEP(NUMBER, BITMASK)                                                \
  ({                                                                         \
    typeof(BITMASK) ShuffledBits, Number = (NUMBER);                         \
    asm("pdep\t%2,%1,%0" : "=r"(ShuffledBits) : "r"(Number), "rm"(BITMASK)); \
    ShuffledBits;                                                            \
  })

#define pdep(NUMBER, BITMASK) \
  (!X86_HAVE(BMI2) ? pdep(NUMBER, BITMASK) : PDEP(NUMBER, BITMASK))

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PDEP_H_ */
