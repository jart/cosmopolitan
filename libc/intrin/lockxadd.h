#ifndef COSMOPOLITAN_LIBC_BITS_LOCKXADD_H_
#define COSMOPOLITAN_LIBC_BITS_LOCKXADD_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

intptr_t _lockxadd(void *, intptr_t, size_t);

#if defined(__GNUC__) && !defined(__STRICT_ANSI__) && defined(__x86__)
#define _lockxadd(PTR, VAL)                                                \
  ({                                                                       \
    typeof(*(PTR)) Res;                                                    \
    typeof(Res) Val = (VAL);                                               \
    asm volatile("lock xadd\t%0,%1" : "=r"(Res), "+m"(*(PTR)) : "0"(Val)); \
    Res; /* contains *PTR before addition cf. InterlockedAdd() */          \
  })
#else
#define _lockxadd(MEM, VAL) _lockxadd(MEM, (intptr_t)(VAL), sizeof(*(MEM)))
#endif /* GNUC && !ANSI && x86 */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_LOCKXADD_H_ */
