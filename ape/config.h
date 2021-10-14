#ifndef COSMOPOLITAN_APE_CONFIG_H_
#define COSMOPOLITAN_APE_CONFIG_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)

#define STATIC_SYMBOL(NAME, VALUE)   \
  asm(".equ\t" NAME "," VALUE "\n\t" \
      ".globl\t" NAME)

#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_APE_CONFIG_H_ */
