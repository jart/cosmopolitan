#ifndef COSMOPOLITAN_LIBC_BITS_WEAKEN_H_
#define COSMOPOLITAN_LIBC_BITS_WEAKEN_H_

#ifndef __chibicc__
#define _weaken(symbol)                                         \
  __extension__({                                               \
    extern __typeof__(symbol) symbol __attribute__((__weak__)); \
    &symbol;                                                    \
  })
#else
#define _weaken(symbol) (&(symbol))
#endif

#endif /* COSMOPOLITAN_LIBC_BITS_WEAKEN_H_ */
