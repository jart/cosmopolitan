#ifndef COSMOPOLITAN_DSP_TTY_ITOA8_H_
#define COSMOPOLITAN_DSP_TTY_ITOA8_H_
#include "libc/str/str.h"
COSMOPOLITAN_C_START_

struct Itoa8 {
  uint8_t size[256];
  uint32_t data[256];
};

extern struct Itoa8 kItoa8;

forceinline char *itoa8(char *p, uint8_t c) {
  memcpy(p, &kItoa8.data[c], 4);
  return p + kItoa8.size[c];
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_DSP_TTY_ITOA8_H_ */
