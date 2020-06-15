#ifndef COSMOPOLITAN_LIBC_STR_TPENC_H_
#define COSMOPOLITAN_LIBC_STR_TPENC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

uint64_t tpenc(int32_t) pureconst;
uint64_t tpenc2(int32_t) pureconst;

#define tpenc(CODE)                                                \
  ({                                                               \
    unsigned long Buf;                                             \
    int Di, Code = (CODE);                                         \
    if (0 <= Code && Code <= 127) {                                \
      Buf = Code;                                                  \
    } else {                                                       \
      asm("call\ttpenc" : "=a"(Buf), "=D"(Di) : "1"(CODE) : "cc"); \
    }                                                              \
    Buf;                                                           \
  })

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_TPENC_H_ */
