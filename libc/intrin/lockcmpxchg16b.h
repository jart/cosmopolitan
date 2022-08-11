#ifndef COSMOPOLITAN_LIBC_BITS_LOCKCMPXCHG16B_H_
#define COSMOPOLITAN_LIBC_BITS_LOCKCMPXCHG16B_H_
#include "libc/intrin/asmflag.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#if defined(__GNUC__) && !defined(__STRICT_ANSI__) && defined(__x86__)
/**
 * Compares and exchanges 128-bit value, i.e.
 *
 *     if (*IfThing == *IsEqualToMe) {
 *       *IfThing = ReplaceItWithMe;
 *       return true;
 *     } else {
 *       *IsEqualToMe = *IfThing;
 *       return false;
 *     }
 *
 * @param IfThing should point to aligned memory
 * @param IsEqualToMe should point to in/out local variable
 * @param ReplaceItWithMe might become the new memory value
 * @return true if *IfThing was changed
 * @asyncsignalsafe
 * @threadsafe
 */
static inline bool _lockcmpxchg16b(uint128_t *IfThing, uint128_t *IsEqualToMe,
                                   uint128_t ReplaceItWithMe) {
  bool DidIt;
  uint64_t ax, bx, cx, dx;
  ax = *IsEqualToMe;
  dx = *IsEqualToMe >> 64;
  bx = ReplaceItWithMe;
  cx = ReplaceItWithMe >> 64;
  asm volatile(ZFLAG_ASM("lock cmpxchg16b\t%1")
               : ZFLAG_CONSTRAINT(DidIt), "+m"(*IfThing), "+a"(ax), "+d"(dx)
               : "b"(bx), "c"(cx));
  if (!DidIt) {
    *IsEqualToMe = ax | (uint128_t)dx << 64;
  }
  return DidIt;
}
#endif /* __GNUC__ && !__STRICT_ANSI__ */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_LOCKCMPXCHG16B_H_ */
