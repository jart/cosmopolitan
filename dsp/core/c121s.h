#ifndef COSMOPOLITAN_DSP_CORE_C121S_H_
#define COSMOPOLITAN_DSP_CORE_C121S_H_

__funline signed char C121S(signed char al, signed char bl, signed char cl) {
  short ax;
  ax = al;
  ax += bl;
  ax += bl;
  ax += cl;
  ax += 2;
  ax >>= 2;
  return ax;
}

#endif /* COSMOPOLITAN_DSP_CORE_C121S_H_ */
