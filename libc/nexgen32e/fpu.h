#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_FPU_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_FPU_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/**
 * Saves FPU state.
 * @see X86_HAVE(FXSR)
 */
#define FXSAVE(STATE)          \
  asm volatile("fxsave\t%0"    \
               : "=m"(STATE)   \
               : /* x87/sse */ \
               : "memory")

/**
 * Restores FPU state.
 * @see X86_HAVE(FXSR)
 */
#define FXRSTOR(STATE)                                                       \
  asm volatile("fxrstor\t%0"                                                 \
               : /* x87/sse */                                               \
               : "m"(STATE)                                                  \
               : "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6",     \
                 "xmm7", "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", \
                 "xmm14", "xmm15", "st", "st(1)", "st(2)", "st(3)", "st(4)", \
                 "st(5)", "st(6)", "st(7)", "memory", "fpsr", "fpcr")

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_FPU_H_ */
