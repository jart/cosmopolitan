#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_RDTSCP_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_RDTSCP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define TSC_AUX_CORE(MSR) ((MSR)&0xfff)
#define TSC_AUX_NODE(MSR) (((MSR) >> 12) & 0xfff)

/**
 * Reads CPU timestamp counter and IA32_TSC_AUX.
 *
 * This macro inhibits compiler magic.
 * This macro does not inhibit CPU magic.
 *
 * @see X86_HAVE(RDTSCP)
 */
#define rdtscp(OPT_OUT_IA32_TSC_AUX)               \
  ({                                               \
    uint32_t Ecx, *EcxOut;                         \
    uint64_t Rax, Rcx, Rdx;                        \
    asm volatile("rdtscp"                          \
                 : "=a"(Rax), "=c"(Ecx), "=d"(Rdx) \
                 : /* no inputs */                 \
                 : "memory");                      \
    EcxOut = (OPT_OUT_IA32_TSC_AUX);               \
    if (EcxOut) *EcxOut = Ecx;                     \
    Rdx << 32 | Rax;                               \
  })

/**
 * Reads timestamp counter auxiliary model specific register value.
 */
#define rdpid()                                                           \
  ({                                                                      \
    long Msr;                                                             \
    if (X86_HAVE(RDPID)) {                                                \
      asm volatile("rdpid\t%0" : "=r"(Msr) : /* no inputs */ : "memory"); \
    } else if (IsLinux()) {                                               \
      asm volatile("lsl\t%1,%0" : "=r"(Msr) : "r"(0x7b) : "memory");      \
    } else if (X86_HAVE(RDTSCP)) {                                        \
      asm volatile("rdtscp"                                               \
                   : "=c"(Msr)                                            \
                   : /* no inputs */                                      \
                   : "eax", "edx", "memory");                             \
    } else {                                                              \
      Msr = -1;                                                           \
    }                                                                     \
    Msr;                                                                  \
  })

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_RDTSCP_H_ */
