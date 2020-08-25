#ifndef COSMOPOLITAN_LIBC_INTRIN_PSHUFB_H_
#define COSMOPOLITAN_LIBC_INTRIN_PSHUFB_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void pshufb(uint8_t[16], const uint8_t[16], const uint8_t[16]);

#define pshufb(A, B, C) \
  INTRIN_SSEVEX_X_X_X_(pshufb, SSSE3, "pshufb", INTRIN_NONCOMMUTATIVE, A, B, C)

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_PSHUFB_H_ */
