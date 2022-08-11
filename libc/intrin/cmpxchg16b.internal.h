#ifndef COSMOPOLITAN_LIBC_BITS_CMPXCHG16B_INTERNAL_H_
#define COSMOPOLITAN_LIBC_BITS_CMPXCHG16B_INTERNAL_H_
#include "libc/intrin/asmflag.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

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
 * Please note that Intel Architecture doesn't guarantee 16-byte memory
 * accesses to be atomic on their own. Therefore _lockcmpxchg16b should
 * be considered instead for both thread and asynchronous signal safety
 *
 * @param IfThing should point to aligned memory
 * @param IsEqualToMe should point to in/out local variable
 * @param ReplaceItWithMe might become the new memory value
 * @return true if *IfThing was changed
 */
static inline bool _cmpxchg16b(uint128_t *IfThing, uint128_t *IsEqualToMe,
                               uint128_t ReplaceItWithMe) {
  bool DidIt;
  uint64_t ax, bx, cx, dx;
  ax = *IsEqualToMe;
  dx = *IsEqualToMe >> 64;
  bx = ReplaceItWithMe;
  cx = ReplaceItWithMe >> 64;
  asm volatile(ZFLAG_ASM("cmpxchg16b\t%1")
               : ZFLAG_CONSTRAINT(DidIt), "+m"(*IfThing), "+a"(ax), "+d"(dx)
               : "b"(bx), "c"(cx));
  if (!DidIt) {
    *IsEqualToMe = ax | (uint128_t)dx << 64;
  }
  return DidIt;
}

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_CMPXCHG16B_INTERNAL_H_ */
