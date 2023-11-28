#ifndef COSMOPOLITAN_DSP_CORE_C1331S_H_
#define COSMOPOLITAN_DSP_CORE_C1331S_H_

/**
 * Byte sized kernel for resampling difference samples in half.
 *
 * @define (1*(a-128)+3*(a-128)+3*(a-128)+1*(a-128))/(1+3+3+1)+128
 * @see C1331(), Y420CbCr2RgbScale()
 */
__funline signed char C1331S(signed char al, signed char bl, signed char cl,
                             signed char dl) {
  short ax, bx;
  bx = bl;
  bx += cl;
  bx *= 3;
  ax = al;
  ax += dl;
  ax += bx;
  ax += 4;
  ax >>= 3;
  return ax;
}

#endif /* COSMOPOLITAN_DSP_CORE_C1331S_H_ */
