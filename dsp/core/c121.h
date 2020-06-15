#ifndef COSMOPOLITAN_DSP_CORE_C121_H_
#define COSMOPOLITAN_DSP_CORE_C121_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

forceinline pureconst artificial unsigned char C121(unsigned char al,
                                                    unsigned char bl,
                                                    unsigned char cl) {
  unsigned short ax, bx;
  ax = al;
  ax += bl;
  ax += bl;
  ax += cl;
  ax += 2;
  ax >>= 2;
  return ax;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_DSP_CORE_C121_H_ */
