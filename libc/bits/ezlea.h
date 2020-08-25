#ifndef COSMOPOLITAN_LIBC_BITS_EZLEA_H_
#define COSMOPOLITAN_LIBC_BITS_EZLEA_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#if __pic__ + __pie__ + __code_model_medium__ + __code_model_large__ + 0 > 1
#define ezlea(symbol) "lea\t" symbol "(%%rip),%"
#else
#define ezlea(symbol) "mov\t$" symbol ",%k"
#endif

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_EZLEA_H_ */
