#ifndef COSMOPOLITAN_THIRD_PARTY_AVIR_RESIZE_H_
#define COSMOPOLITAN_THIRD_PARTY_AVIR_RESIZE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Resizer {
  void *p;
};

void FreeResizer(struct Resizer *) paramsnonnull();
void NewResizer(struct Resizer *, int, int) paramsnonnull();
void ResizeImage(struct Resizer *, float *, int, int, const float *, int, int)
    paramsnonnull();

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_AVIR_RESIZE_H_ */
