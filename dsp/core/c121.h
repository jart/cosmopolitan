#ifndef COSMOPOLITAN_DSP_CORE_C121_H_
#define COSMOPOLITAN_DSP_CORE_C121_H_
COSMOPOLITAN_C_START_

__funline unsigned char C121(unsigned char al, unsigned char bl,
                             unsigned char cl) {
  unsigned short ax;
  ax = al;
  ax += bl;
  ax += bl;
  ax += cl;
  ax += 2;
  ax >>= 2;
  return ax;
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_DSP_CORE_C121_H_ */
