#ifndef COSMOPOLITAN_LIBC_BITS_LOCKXADD_INTERNAL_H_
#define COSMOPOLITAN_LIBC_BITS_LOCKXADD_INTERNAL_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define _lockxadd(PTR, VAL)                                                \
  ({                                                                       \
    typeof(*(PTR)) Res;                                                    \
    typeof(Res) Val = (VAL);                                               \
    asm volatile("lock xadd\t%0,%1" : "=r"(Res), "+m"(*(PTR)) : "0"(Val)); \
    Res; /* contains *PTR before addition cf. InterlockedAdd() */          \
  })

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_LOCKXADD_INTERNAL_H_ */
