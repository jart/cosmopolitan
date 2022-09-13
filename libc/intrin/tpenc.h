#ifndef COSMOPOLITAN_LIBC_STR_TPENC_H_
#define COSMOPOLITAN_LIBC_STR_TPENC_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

uint64_t _tpenc(int32_t) pureconst;

#if defined(__MNO_RED_ZONE__) && defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define _tpenc(CODE)           \
  ({                           \
    long Edi, Buf;             \
    asm("call\t_tpenc"         \
        : "=a"(Buf), "=D"(Edi) \
        : "1"((int)(CODE))     \
        : "rcx", "rdx", "cc"); \
    Buf;                       \
  })
#endif

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_TPENC_H_ */
