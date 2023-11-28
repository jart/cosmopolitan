#ifndef COSMOPOLITAN_LIBC_INTRIN_CMPXCHG_H_
#define COSMOPOLITAN_LIBC_INTRIN_CMPXCHG_H_
#include "libc/intrin/asmflag.h"
COSMOPOLITAN_C_START_

#if defined(__GNUC__) && !defined(__STRICT_ANSI__) && defined(__x86__)
#define _cmpxchg(IFTHING, ISEQUALTOME, REPLACEITWITHME)                       \
  ({                                                                          \
    bool32 DidIt;                                                             \
    autotype(IFTHING) IfThing = (IFTHING);                                    \
    typeof(*IfThing) IsEqualToMe = (ISEQUALTOME);                             \
    typeof(*IfThing) ReplaceItWithMe = (REPLACEITWITHME);                     \
    asm volatile(ZFLAG_ASM("cmpxchg\t%3,%1")                                  \
                 : ZFLAG_CONSTRAINT(DidIt), "+m"(*IfThing), "+a"(IsEqualToMe) \
                 : "r"(ReplaceItWithMe)                                       \
                 : "cc");                                                     \
    DidIt;                                                                    \
  })
#else
#define _cmpxchg(IFTHING, ISEQUALTOME, REPLACEITWITHME) \
  (*(IFTHING) == (ISEQUALTOME) ? (*(IFTHING) = (REPLACEITWITHME), 1) : 0)
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_INTRIN_CMPXCHG_H_ */
