#ifndef COSMOPOLITAN_LIBC_INTERNAL_OUTOFSIGHT_H_
#define COSMOPOLITAN_LIBC_INTERNAL_OUTOFSIGHT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#if 0
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § standard library » little concerns                        ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#endif

#if 0
/**
 * Hardware-Accellerated 32-Bit Castagnoli Cyclic Redundancy Check.
 * Introduced w/ Intel Nehalem c. 2008 & AMD Bulldozer c. 2011
 * @see crc32c()
 */
#endif
#define __crc32c(out_Gdqp, in_Eb_Evqp) \
  asm("crc32\t%1,%0" : "+r"(*(r)) : "rm"(p))

#if 0
/**
 * Works around GCC's poorly implemented -Waddress feature.
 * Used for the well-known divide-by-zero static assertion.
 */
#endif
#define __NOTNOT(ADDR) ((uintptr_t)(ADDR) != (uintptr_t)NULL ? 1 : 0)
#define __LITERALLY_STRING(S) (typeof(&S[0]))((uintptr_t)(S) / __NOTNOT(S))

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTERNAL_OUTOFSIGHT_H_ */
