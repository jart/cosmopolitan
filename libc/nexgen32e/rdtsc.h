#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_RDTSC_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_RDTSC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/**
 * Reads CPU timestamp counter.
 *
 * This macro inhibits compiler magic.
 * This macro does not inhibit CPU magic.
 *
 * @see X86_HAVE(INVTSC)
 */
#define rdtsc() __RDTSC("rdtsc")

/**
 * Reads CPU timestamp counter w/ full serialization.
 *
 * This macro inhibits CPU magic.
 * This macro inhibits compiler magic.
 *
 * The clock isn't read until:
 *
 *   1. previous instructions finish executing; and
 *   2. previous loads are globally visible; and
 *   3. previous stores are globally visible.
 *
 * Later instructions won't dispatch until RDTSC completes.
 *
 * @see X86_HAVE(INVTSC)
 */
#define mfence_lfence_rdtsc_lfence() \
  __RDTSC("mfence\n\tlfence\n\trdtsc\n\tlfence")

#define __RDTSC(ASM)                                                       \
  ({                                                                       \
    uint64_t Rax, Rdx;                                                     \
    asm volatile(ASM : "=a"(Rax), "=d"(Rdx) : /* no inputs */ : "memory"); \
    Rdx << 32 | Rax;                                                       \
  })

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_RDTSC_H_ */
