#ifndef COSMOPOLITAN_THIRD_PARTY_MBEDTLS_SELECT_H_
#define COSMOPOLITAN_THIRD_PARTY_MBEDTLS_SELECT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

static inline uint64_t Select(uint64_t a, uint64_t b, uint64_t mask) {
  return (CONCEAL("r", mask) & a) | (CONCEAL("r", ~mask) & b);
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_MBEDTLS_SELECT_H_ */
