#ifndef COSMOPOLITAN_LIBC_STR_THOMPIKE_H_
#define COSMOPOLITAN_LIBC_STR_THOMPIKE_H_
#include "libc/intrin/bsr.h"

#define ThomPikeCont(x)     (0200 == (0300 & (x)))
#define ThomPikeByte(x)     ((x) & (((1 << ThomPikeMsb(x)) - 1) | 3))
#define ThomPikeLen(x)      (7 - ThomPikeMsb(x))
#define ThomPikeMsb(x)      ((255 & (x)) < 252 ? _bsr(255 & ~(x)) : 1)
#define ThomPikeMerge(x, y) ((x) << 6 | (077 & (y)))

#endif /* COSMOPOLITAN_LIBC_STR_THOMPIKE_H_ */
