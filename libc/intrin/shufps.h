#ifndef COSMOPOLITAN_LIBC_INTRIN_SHUFPS_H_
#define COSMOPOLITAN_LIBC_INTRIN_SHUFPS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void shufps(float[4], const float[4], const float[4], uint8_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_SHUFPS_H_ */
