#ifndef COSMOPOLITAN_LIBC_STR_KNUTHMULTIPLICATIVEHASH_H_
#define COSMOPOLITAN_LIBC_STR_KNUTHMULTIPLICATIVEHASH_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline uint32_t KnuthMultiplicativeHash32(const void *buf, size_t size) {
  size_t i;
  uint32_t h;
  const uint32_t kPhiPrime = 0x9e3779b1;
  const unsigned char *p = (const unsigned char *)buf;
  for (h = i = 0; i < size; i++) h = (p[i] + h) * kPhiPrime;
  return h;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_KNUTHMULTIPLICATIVEHASH_H_ */
