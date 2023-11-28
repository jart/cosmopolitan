#ifndef COSMOPOLITAN_DSP_CORE_C161_H_
#define COSMOPOLITAN_DSP_CORE_C161_H_
#include "libc/macros.internal.h"

#define EXTRA_SHARP 2

/**
 * Byte sized kernel for restoring sharpness of resampled memory.
 *
 * @define CLAMP[(-1*𝑎 + 6*𝑏 + -1*𝑐) / (-1 + 6 + -1)]
 * @limit [0..255] → [-510..1,532] → [-127..383] → [0..255]
 * @see C1331()
 */
__funline unsigned char C161(unsigned char al, unsigned char bl,
                             unsigned char cl) {
  short ax, bx, cx;
  ax = al;
  bx = bl;
  cx = cl;
  ax *= -1;
  bx *= +6;
  cx *= -1;
  ax += bx;
  ax += cx;
  ax += 2;
  ax >>= 2;
  return MIN(255, MAX(0, ax));
}

#endif /* COSMOPOLITAN_DSP_CORE_C161_H_ */
