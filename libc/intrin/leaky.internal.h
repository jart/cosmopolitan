#ifndef COSMOPOLITAN_LIBC_INTRIN_LEAKY_INTERNAL_H_
#define COSMOPOLITAN_LIBC_INTRIN_LEAKY_INTERNAL_H_
#include "libc/dce.h"
COSMOPOLITAN_C_START_

#if IsAsan()
#define IGNORE_LEAKS(FUNC)                                        \
  __static_yoink("_leaky_start");                                 \
  void *_leaky_##FUNC[] _Section(".piro.relo.sort.leaky.2." #FUNC \
                                 ",\"aw\",@init_array #") = {FUNC};
#else
#define IGNORE_LEAKS(FUNC)
#endif

extern intptr_t _leaky_end[] __attribute__((__weak__));
extern intptr_t _leaky_start[] __attribute__((__weak__));

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_LEAKY_INTERNAL_H_ */
