#ifndef COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS1B_H_
#define COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS1B_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void lanczos1b(size_t dyn, size_t dxn, unsigned char *restrict dst, size_t syn,
               size_t sxn, const unsigned char *restrict src);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS1B_H_ */
