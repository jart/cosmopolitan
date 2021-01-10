#ifndef COSMOPOLITAN_LIBC_BITS_INITIALIZER_H_
#define COSMOPOLITAN_LIBC_BITS_INITIALIZER_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

/* TODO: DELETE */

/**
 * Teleports code fragment inside _init().
 */
#define INITIALIZER(PRI, NAME, CODE)                               \
  asm(".section .init." #PRI "." #NAME ",\"ax\",@progbits\n\t"     \
      "call\t" #NAME "\n\t"                                        \
      ".previous");                                                \
  textstartup optimizesize void NAME(char *rdi, const char *rsi) { \
    CODE;                                                          \
    asm volatile("" : /* no outputs */ : "D"(rdi), "S"(rsi));      \
  }

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_INITIALIZER_H_ */
