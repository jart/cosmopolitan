#ifndef COSMOPOLITAN_DSP_CORE_C11_H_
#define COSMOPOLITAN_DSP_CORE_C11_H_

/**
 * Fixed-point 8-bit rounded mean kernel.
 *
 * @define (a + b) / 2
 */
__funline unsigned char C11(unsigned char al, unsigned char bl) {
  short ax;
  ax = al;
  ax += bl;
  ax += 1;
  ax /= 2;
  al = ax;
  return al;
}

#endif /* COSMOPOLITAN_DSP_CORE_C11_H_ */
