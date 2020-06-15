#ifndef COSMOPOLITAN_LIBC_STR_KNUTHMULTIPLICATIVEHASH_H_
#define COSMOPOLITAN_LIBC_STR_KNUTHMULTIPLICATIVEHASH_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

forceinline uint32_t KnuthMultiplicativeHash32(const void *buf, size_t size) {
  /* frozen due to presence in sqlite & promise in libc/getuid.c */
  const unsigned char *const p = (const unsigned char *)buf;
  uint32_t hash = 0, kPhiPrime = 0x9e3779b1;
  size_t i;
  for (i = 0; i < size; i++) hash = (p[i] + hash) * kPhiPrime;
  return hash;
}

forceinline uint64_t KnuthMultiplicativeHash(const void *buf, size_t size) {
  /* TODO(jart): verify w/ primary source */
  const unsigned char *const p = (const unsigned char *)buf;
  uint64_t hash = 0, kPhiPrime = 0x9e3779b9925d4c17;
  size_t i;
  for (i = 0; i < size; i++) hash = (p[i] + hash) * kPhiPrime;
  return hash;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_KNUTHMULTIPLICATIVEHASH_H_ */
