#ifndef COSMOPOLITAN_LIBC_INTRIN_LOCKCMPXCHG_H_
#define COSMOPOLITAN_LIBC_INTRIN_LOCKCMPXCHG_H_
#include "libc/bits/asmflag.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

bool _lockcmpxchg(void *, intptr_t, intptr_t, size_t);

#if defined(__GNUC__) && !defined(__STRICT_ANSI__) && defined(__x86__)
#define _lockcmpxchg(IFTHING, ISEQUALTOME, REPLACEITWITHME)                   \
  ({                                                                          \
    bool DidIt;                                                               \
    autotype(IFTHING) IfThing = (IFTHING);                                    \
    typeof(*IfThing) IsEqualToMe = (ISEQUALTOME);                             \
    typeof(*IfThing) ReplaceItWithMe = (REPLACEITWITHME);                     \
    asm volatile(ZFLAG_ASM("lock cmpxchg\t%3,%1")                             \
                 : ZFLAG_CONSTRAINT(DidIt), "+m"(*IfThing), "+a"(IsEqualToMe) \
                 : "r"(ReplaceItWithMe)                                       \
                 : "cc");                                                     \
    DidIt;                                                                    \
  })
#else
#define _lockcmpxchg(MEM, CMP, VAL) \
  _lockcmpxchg(MEM, (intptr_t)(CMP), (intptr_t)(VAL), sizeof(*(MEM)))
#endif /* GNUC && !ANSI && x86 */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_LOCKCMPXCHG_H_ */
