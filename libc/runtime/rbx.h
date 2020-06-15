#ifndef COSMOPOLITAN_LIBC_RUNTIME_RBX_H_
#define COSMOPOLITAN_LIBC_RUNTIME_RBX_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#if 0
#ifndef __llvm__
register uint8_t *__rbx asm("rbx");
#else
#define __rbx                         \
  ({                                  \
    register uint8_t *Rbx asm("rbx"); \
    asm("" : "=r"(Rbx));              \
    Rbx;                              \
  })
#endif
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_RUNTIME_RBX_H_ */
