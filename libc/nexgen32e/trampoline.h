#ifndef COSMOPOLITAN_LIBC_NEXGEN32E_TRAMPOLINE_H_
#define COSMOPOLITAN_LIBC_NEXGEN32E_TRAMPOLINE_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define TRAMPOLINE(FUNCTION, THUNK)   \
  ({                                  \
    typeof(FUNCTION) *Tramp;          \
    asm(".section .text.trampoline\n" \
        "183:\n\t"                    \
        "mov\t%1,%%eax\n\t"           \
        "jmp\t" #THUNK "\n\t"         \
        ".previous\n\t"               \
        "mov\t$183b,%k0"              \
        : "=r"(Tramp)                 \
        : "i"(FUNCTION));             \
    Tramp;                            \
  })

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_NEXGEN32E_TRAMPOLINE_H_ */
