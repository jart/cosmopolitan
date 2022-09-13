#ifndef COSMOPOLITAN_LIBC_BITS_WEAKEN_H_
#define COSMOPOLITAN_LIBC_BITS_WEAKEN_H_
#include "libc/intrin/ezlea.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
#ifndef __STRICT_ANSI__

#define _weaken(symbol) ((const typeof(&(symbol)))_weakaddr(#symbol))

#define _strongaddr(symbolstr)                \
  ({                                          \
    intptr_t waddr;                           \
    asm(_ezlea(symbolstr) "0" : "=r"(waddr)); \
    waddr;                                    \
  })

#define _weakaddr(symbolstr)                                             \
  ({                                                                     \
    intptr_t waddr;                                                      \
    asm(".weak\t" symbolstr "\n\t" _ezlea(symbolstr) "0" : "=r"(waddr)); \
    waddr;                                                               \
  })

#else
#define _weaken(symbol)      symbol
#define _weakaddr(symbolstr) &(symbolstr)
#endif /* __STRICT_ANSI__ */
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_WEAKEN_H_ */
