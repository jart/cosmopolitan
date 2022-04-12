#ifndef COSMOPOLITAN_LIBC_INTRIN_LOCKXCHG_H_
#define COSMOPOLITAN_LIBC_INTRIN_LOCKXCHG_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

intptr_t lockxchg(void *, void *, size_t);

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
/**
 * Exchanges *MEMORY into *LOCALVAR w/ one operation.
 *
 * @param MEMORY is uint𝑘_t[hasatleast 1] where 𝑘 ∈ {8,16,32,64}
 * @param LOCALVAR is uint𝑘_t[hasatleast 1]
 * @return LOCALVAR[0]
 * @see xchg()
 */
#define lockxchg(MEMORY, LOCALVAR)                            \
  ({                                                          \
    asm("xchg\t%0,%1" : "+%m"(*(MEMORY)), "+r"(*(LOCALVAR))); \
    *(LOCALVAR);                                              \
  })
#else
#define lockxchg(MEM, VAR) \
  lockxchg(MEM, VAR, sizeof(*(MEM)) / (sizeof(*(MEM)) == sizeof(*(VAR))))
#endif /* __GNUC__ && !__STRICT_ANSI__ */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_LOCKXCHG_H_ */
