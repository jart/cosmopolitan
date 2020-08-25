#ifndef COSMOPOLITAN_LIBC_BITS_WEAKEN_H_
#define COSMOPOLITAN_LIBC_BITS_WEAKEN_H_
#include "libc/bits/ezlea.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef __STRICT_ANSI__

#define weaken(symbol) ((const typeof(&(symbol)))weakaddr(#symbol))

#define strongaddr(symbolstr)                \
  ({                                         \
    intptr_t waddr;                          \
    asm(ezlea(symbolstr) "0" : "=r"(waddr)); \
    waddr;                                   \
  })

#define weakaddr(symbolstr)                                             \
  ({                                                                    \
    intptr_t waddr;                                                     \
    asm(".weak\t" symbolstr "\n\t" ezlea(symbolstr) "0" : "=r"(waddr)); \
    waddr;                                                              \
  })

#else
#define weaken(symbol)      symbol
#define weakaddr(symbolstr) &(symbolstr)
#endif /* __STRICT_ANSI__ */
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_WEAKEN_H_ */
