#ifndef COSMOPOLITAN_DSP_CORE_C121S_H_
#define COSMOPOLITAN_DSP_CORE_C121S_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

forceinline pureconst artificial signed char C121S(signed char al,
                                                   signed char bl,
                                                   signed char cl) {
  short ax, bx;
  ax = al;
  ax += bl;
  ax += bl;
  ax += cl;
  ax += 2;
  ax >>= 2;
  return ax;
}

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_DSP_CORE_C121S_H_ */
