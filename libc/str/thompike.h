#ifndef COSMOPOLITAN_LIBC_STR_THOMPIKE_H_
#define COSMOPOLITAN_LIBC_STR_THOMPIKE_H_
#include "libc/nexgen32e/bsr.h"

#define ThomPikeCont(x) ((x & 0b11000000) == 0b10000000)
#define ThomPikeByte(x) (x & (((1 << (x < 252 ? bsr(~x & 0xff) : 1)) - 1) | 3))

#endif /* COSMOPOLITAN_LIBC_STR_THOMPIKE_H_ */
