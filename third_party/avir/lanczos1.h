#ifndef COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS1_H_
#define COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS1_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct lanczos1 {
  void *p;
};

void lanczos1init(struct lanczos1 *self);
void lanczos1free(struct lanczos1 *self);
void lanczos1(struct lanczos1 *self, size_t dyn, size_t dxn, void *dst,
              size_t dstsize, size_t syn, size_t sxn, size_t ssw,
              const void *src, size_t srcsize) paramsnonnull();

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS1_H_ */
