#ifndef COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS_H_
#define COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

void lanczos(unsigned, unsigned, void *, unsigned, unsigned, const void *,
             unsigned);
void lanczos3(unsigned, unsigned, void *, unsigned, unsigned, const void *,
              unsigned);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS_H_ */
