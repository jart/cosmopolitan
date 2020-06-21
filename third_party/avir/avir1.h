#ifndef COSMOPOLITAN_THIRD_PARTY_AVIR_AVIR1_H_
#define COSMOPOLITAN_THIRD_PARTY_AVIR_AVIR1_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct avir1 {
  void *p;
};

void avir1init(struct avir1 *);
void avir1free(struct avir1 *);
void avir1(struct avir1 *, size_t, size_t, void *, size_t, size_t, size_t,
           size_t, const void *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_AVIR_AVIR1_H_ */
