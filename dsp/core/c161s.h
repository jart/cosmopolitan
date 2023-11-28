#ifndef COSMOPOLITAN_DSP_CORE_C161S_H_
#define COSMOPOLITAN_DSP_CORE_C161S_H_
#include "dsp/core/c161.h"
#include "libc/macros.internal.h"

__funline signed char C161S(signed char al, signed char bl, signed char cl) {
  short ax, bx, cx;
  ax = al;
  bx = bl;
  cx = cl;
  ax *= -1 * EXTRA_SHARP;
  bx *= 6 * EXTRA_SHARP;
  cx *= -1 * EXTRA_SHARP;
  ax += bx;
  ax += cx;
  ax += 2 * EXTRA_SHARP;
  ax /= 4 * EXTRA_SHARP;
  al = MIN(112, MAX(-112, ax));
  return al;
}

#endif /* COSMOPOLITAN_DSP_CORE_C161S_H_ */
