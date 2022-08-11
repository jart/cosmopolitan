#ifndef COSMOPOLITAN_LIBC_BITS_BITOP_H_
#define COSMOPOLITAN_LIBC_BITS_BITOP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define bts(MEM, BIT)     __BitOp("bts", BIT, MEM) /** bit test and set */
#define btr(MEM, BIT)     __BitOp("btr", BIT, MEM) /** bit test and reset */
#define btc(MEM, BIT)     __BitOp("btc", BIT, MEM) /** bit test and complement */
#define lockbts(MEM, BIT) __BitOp("lock bts", BIT, MEM)
#define lockbtr(MEM, BIT) __BitOp("lock btr", BIT, MEM)
#define lockbtc(MEM, BIT) __BitOp("lock btc", BIT, MEM)

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)
#define __BitOp(OP, BIT, MEM)                                  \
  ({                                                           \
    bool OldBit;                                               \
    if (__builtin_constant_p(BIT)) {                           \
      asm(CFLAG_ASM(OP "%z1\t%2,%1")                           \
          : CFLAG_CONSTRAINT(OldBit),                          \
            "+m"((MEM)[(BIT) / (sizeof((MEM)[0]) * CHAR_BIT)]) \
          : "J"((BIT) % (sizeof((MEM)[0]) * CHAR_BIT))         \
          : "cc");                                             \
    } else if (sizeof((MEM)[0]) == 2) {                        \
      asm(CFLAG_ASM(OP "\t%w2,%1")                             \
          : CFLAG_CONSTRAINT(OldBit), "+m"((MEM)[0])           \
          : "r"(BIT)                                           \
          : "cc");                                             \
    } else if (sizeof((MEM)[0]) == 4) {                        \
      asm(CFLAG_ASM(OP "\t%k2,%1")                             \
          : CFLAG_CONSTRAINT(OldBit), "+m"((MEM)[0])           \
          : "r"(BIT)                                           \
          : "cc");                                             \
    } else if (sizeof((MEM)[0]) == 8) {                        \
      asm(CFLAG_ASM(OP "\t%q2,%1")                             \
          : CFLAG_CONSTRAINT(OldBit), "+m"((MEM)[0])           \
          : "r"(BIT)                                           \
          : "cc");                                             \
    }                                                          \
    OldBit;                                                    \
  })
#endif /* __GNUC__ && !__STRICT_ANSI__ */

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_BITOP_H_ */
