#ifndef COSMOPOLITAN_DSP_CORE_C11_H_
#define COSMOPOLITAN_DSP_CORE_C11_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Fixed-point 8-bit rounded mean kernel.
 *
 * @define (a + b) / 2
 */
static inline pureconst artificial unsigned char C11(unsigned char al,
                                                     unsigned char bl) {
  short ax;
  ax = al;
  ax += bl;
  ax += 1;
  ax /= 2;
  al = ax;
  return al;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_DSP_CORE_C11_H_ */
