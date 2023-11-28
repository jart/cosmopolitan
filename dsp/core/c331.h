#ifndef COSMOPOLITAN_DSP_CORE_C331_H_
#define COSMOPOLITAN_DSP_CORE_C331_H_
COSMOPOLITAN_C_START_

/**
 * Fixed-point 8-bit magic edge resampling kernel.
 *
 * @define (3*a + 3*b + 1*c) / 7
 * @see C1331()
 */
__funline unsigned char C331(unsigned char al, unsigned char bl,
                             unsigned char cl) {
  unsigned eax, ebx, ecx;
  eax = al;
  ebx = bl;
  ecx = cl;
  eax += ebx;
  eax *= 3 * 2350;
  ecx *= 1 * 2350;
  eax += ecx;
  eax >>= 14;
  al = eax;
  return al;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_DSP_CORE_C331_H_ */
