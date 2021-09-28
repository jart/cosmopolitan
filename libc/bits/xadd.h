#ifndef COSMOPOLITAN_LIBC_BITS_XADD_H_
#define COSMOPOLITAN_LIBC_BITS_XADD_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define _xadd(p, v)                                                 \
  ({                                                                \
    typeof(*(p)) Res;                                               \
    autotype(Res) Val = (v);                                        \
    asm volatile("xadd\t%0,%1" : "=r"(Res), "+m"(*(p)) : "0"(Val)); \
    Res + Val;                                                      \
  })

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_XADD_H_ */
