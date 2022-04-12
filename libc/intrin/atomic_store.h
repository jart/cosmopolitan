#ifndef COSMOPOLITAN_LIBC_INTRIN_ATOMIC_STORE_H_
#define COSMOPOLITAN_LIBC_INTRIN_ATOMIC_STORE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

intptr_t atomic_store(void *, intptr_t, size_t);

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define atomic_store(MEM, VAL)                    \
  ({                                              \
    autotype(VAL) Val = (VAL);                    \
    typeof(&Val) Mem = (MEM);                     \
    asm("mov%z1\t%1,%0" : "=m"(*Mem) : "r"(Val)); \
    Val;                                          \
  })
#else
#define atomic_store(MEM, VAL) \
  atomic_store(MEM, VAL, sizeof(*(MEM)) / (sizeof(*(MEM)) == sizeof(*(VAL))))
#endif /* __GNUC__ && !__STRICT_ANSI__ */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_ATOMIC_STORE_H_ */
