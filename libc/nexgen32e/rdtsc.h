#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_RDTSC_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_RDTSC_H_
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

#ifdef __x86__
#define __RDTSC(ASM)                                                       \
  ({                                                                       \
    uint64_t Rax, Rdx;                                                     \
    asm volatile(ASM : "=a"(Rax), "=d"(Rdx) : /* no inputs */ : "memory"); \
    Rdx << 32 | Rax;                                                       \
  })
#elif defined(__aarch64__)
#define __RDTSC(ASM)                                \
  ({                                                \
    uint64_t _Ts;                                   \
    asm volatile("mrs\t%0,cntvct_el0" : "=r"(_Ts)); \
    _Ts * 48; /* the fudge factor */                \
  })
#elif defined(__powerpc64__)
#define __RDTSC(ASM)                           \
  ({                                           \
    uint64_t _Ts;                              \
    asm volatile("mfspr\t%0,268" : "=r"(_Ts)); \
    _Ts;                                       \
  })
#elif defined(__riscv)
#define __RDTSC(ASM)                         \
  ({                                         \
    uint64_t _Ts;                            \
    asm volatile("rdcycle\t%0" : "=r"(_Ts)); \
    _Ts;                                     \
  })
#endif

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_RDTSC_H_ */
