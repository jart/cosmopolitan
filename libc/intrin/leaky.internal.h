#ifndef COSMOPOLITAN_LIBC_INTRIN_LEAKY_INTERNAL_H_
#define COSMOPOLITAN_LIBC_INTRIN_LEAKY_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define IGNORE_LEAKS(FUNC)                                        \
  STATIC_YOINK("_leaky_start");                                   \
  void *_leaky_##FUNC[] _Section(".piro.relo.sort.leaky.2." #FUNC \
                                 ",\"aw\",@init_array #") = {FUNC}

extern intptr_t _leaky_end[] __attribute__((__weak__));
extern intptr_t _leaky_start[] __attribute__((__weak__));

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_LEAKY_INTERNAL_H_ */
