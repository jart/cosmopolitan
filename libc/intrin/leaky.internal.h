#ifndef COSMOPOLITAN_LIBC_INTRIN_LEAKY_INTERNAL_H_
#define COSMOPOLITAN_LIBC_INTRIN_LEAKY_INTERNAL_H_
#include "libc/dce.h"
COSMOPOLITAN_C_START_

#define IGNORE_LEAKS(FUNC)

extern intptr_t _leaky_end[] __attribute__((__weak__));
extern intptr_t _leaky_start[] __attribute__((__weak__));

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_LEAKY_INTERNAL_H_ */
