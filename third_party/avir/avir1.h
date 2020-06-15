#ifndef COSMOPOLITAN_THIRD_PARTY_AVIR_AVIR1_H_
#define COSMOPOLITAN_THIRD_PARTY_AVIR_AVIR1_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct avir1 {
  void *p;
};

void avir1init(struct avir1 *self);
void avir1free(struct avir1 *self);
void avir1(struct avir1 *resizer, size_t dyn, size_t dxn, void *dst,
           size_t dstsize, size_t syn, size_t sxn, size_t ssw, const void *src,
           size_t srcsize);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_AVIR_AVIR1_H_ */
