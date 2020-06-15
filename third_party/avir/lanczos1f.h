#ifndef COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS1F_H_
#define COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS1F_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct lanczos1f {
  void *p;
};

void lanczos1finit(struct lanczos1f *);
void lanczos1ffree(struct lanczos1f *);
void lanczos1f(struct lanczos1f *, size_t, size_t, void *, size_t, size_t,
               size_t, const void *, double, double, double, double)
    paramsnonnull();

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_AVIR_LANCZOS1F_H_ */
