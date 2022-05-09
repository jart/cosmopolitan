#ifndef COSMOPOLITAN_LIBC_INTRIN_ATOMIC_LOAD_H_
#define COSMOPOLITAN_LIBC_INTRIN_ATOMIC_LOAD_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

intptr_t atomic_load(void *, size_t);

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define atomic_load(MEM)                       \
  ({                                           \
    autotype(MEM) Mem = (MEM);                 \
    typeof(*Mem) Reg;                          \
    asm("mov\t%1,%0" : "=r"(Reg) : "m"(*Mem)); \
    Reg;                                       \
  })
#else
#define atomic_load(MEM) atomic_load(MEM, sizeof(*(MEM)))
#endif /* GNUC && !ANSI && x86 */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_INTRIN_ATOMIC_LOAD_H_ */
