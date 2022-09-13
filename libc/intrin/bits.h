#ifndef COSMOPOLITAN_LIBC_BITS_H_
#define COSMOPOLITAN_LIBC_BITS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define CheckUnsigned(x) ((x) / !((typeof(x))(-1) < 0))

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § bits                                                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

extern const uint8_t kReverseBits[256];

uint32_t gray(uint32_t) pureconst;
uint32_t ungray(uint32_t) pureconst;
int _bitreverse8(int) libcesque pureconst;
int _bitreverse16(int) libcesque pureconst;
uint32_t _bitreverse32(uint32_t) libcesque pureconst;
uint64_t _bitreverse64(uint64_t) libcesque pureconst;
unsigned long _roundup2pow(unsigned long) libcesque pureconst;
unsigned long _roundup2log(unsigned long) libcesque pureconst;
unsigned long _rounddown2pow(unsigned long) libcesque pureconst;
unsigned long _hamming(unsigned long, unsigned long) pureconst;
unsigned _bextra(const unsigned *, size_t, char);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § bits » no assembly required                               ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#ifdef __STRICT_ANSI__
#define READ16LE(S) ((255 & (S)[1]) << 8 | (255 & (S)[0]))
#define READ16BE(S) ((255 & (S)[0]) << 8 | (255 & (S)[1]))
#define READ32LE(S)                                                    \
  ((uint32_t)(255 & (S)[3]) << 030 | (uint32_t)(255 & (S)[2]) << 020 | \
   (uint32_t)(255 & (S)[1]) << 010 | (uint32_t)(255 & (S)[0]) << 000)
#define READ32BE(S)                                                    \
  ((uint32_t)(255 & (S)[0]) << 030 | (uint32_t)(255 & (S)[1]) << 020 | \
   (uint32_t)(255 & (S)[2]) << 010 | (uint32_t)(255 & (S)[3]) << 000)
#define READ64LE(S)                                                    \
  ((uint64_t)(255 & (S)[7]) << 070 | (uint64_t)(255 & (S)[6]) << 060 | \
   (uint64_t)(255 & (S)[5]) << 050 | (uint64_t)(255 & (S)[4]) << 040 | \
   (uint64_t)(255 & (S)[3]) << 030 | (uint64_t)(255 & (S)[2]) << 020 | \
   (uint64_t)(255 & (S)[1]) << 010 | (uint64_t)(255 & (S)[0]) << 000)
#define READ64BE(S)                                                    \
  ((uint64_t)(255 & (S)[0]) << 070 | (uint64_t)(255 & (S)[1]) << 060 | \
   (uint64_t)(255 & (S)[2]) << 050 | (uint64_t)(255 & (S)[3]) << 040 | \
   (uint64_t)(255 & (S)[4]) << 030 | (uint64_t)(255 & (S)[5]) << 020 | \
   (uint64_t)(255 & (S)[6]) << 010 | (uint64_t)(255 & (S)[7]) << 000)
#else /* gcc needs help knowing above are mov if s isn't a variable */
#define READ16LE(S)                            \
  ({                                           \
    const uint8_t *Ptr = (const uint8_t *)(S); \
    Ptr[1] << 8 | Ptr[0];                      \
  })
#define READ16BE(S)                            \
  ({                                           \
    const uint8_t *Ptr = (const uint8_t *)(S); \
    Ptr[0] << 8 | Ptr[1];                      \
  })
#define READ32LE(S)                                      \
  ({                                                     \
    const uint8_t *Ptr = (const uint8_t *)(S);           \
    ((uint32_t)Ptr[3] << 030 | (uint32_t)Ptr[2] << 020 | \
     (uint32_t)Ptr[1] << 010 | (uint32_t)Ptr[0] << 000); \
  })
#define READ32BE(S)                                      \
  ({                                                     \
    const uint8_t *Ptr = (const uint8_t *)(S);           \
    ((uint32_t)Ptr[0] << 030 | (uint32_t)Ptr[1] << 020 | \
     (uint32_t)Ptr[2] << 010 | (uint32_t)Ptr[3] << 000); \
  })
#define READ64LE(S)                                      \
  ({                                                     \
    const uint8_t *Ptr = (const uint8_t *)(S);           \
    ((uint64_t)Ptr[7] << 070 | (uint64_t)Ptr[6] << 060 | \
     (uint64_t)Ptr[5] << 050 | (uint64_t)Ptr[4] << 040 | \
     (uint64_t)Ptr[3] << 030 | (uint64_t)Ptr[2] << 020 | \
     (uint64_t)Ptr[1] << 010 | (uint64_t)Ptr[0] << 000); \
  })
#define READ64BE(S)                                      \
  ({                                                     \
    const uint8_t *Ptr = (const uint8_t *)(S);           \
    ((uint64_t)Ptr[0] << 070 | (uint64_t)Ptr[1] << 060 | \
     (uint64_t)Ptr[2] << 050 | (uint64_t)Ptr[3] << 040 | \
     (uint64_t)Ptr[4] << 030 | (uint64_t)Ptr[5] << 020 | \
     (uint64_t)Ptr[6] << 010 | (uint64_t)Ptr[7] << 000); \
  })
#endif

#define WRITE16LE(P, V)                        \
  ((P)[0] = (0x00000000000000FF & (V)) >> 000, \
   (P)[1] = (0x000000000000FF00 & (V)) >> 010, (P) + 2)
#define WRITE16BE(P, V)                        \
  ((P)[0] = (0x000000000000FF00 & (V)) >> 010, \
   (P)[1] = (0x00000000000000FF & (V)) >> 000, (P) + 2)
#define WRITE32LE(P, V)                        \
  ((P)[0] = (0x00000000000000FF & (V)) >> 000, \
   (P)[1] = (0x000000000000FF00 & (V)) >> 010, \
   (P)[2] = (0x0000000000FF0000 & (V)) >> 020, \
   (P)[3] = (0x00000000FF000000 & (V)) >> 030, (P) + 4)
#define WRITE32BE(P, V)                        \
  ((P)[0] = (0x00000000FF000000 & (V)) >> 030, \
   (P)[1] = (0x0000000000FF0000 & (V)) >> 020, \
   (P)[2] = (0x000000000000FF00 & (V)) >> 010, \
   (P)[3] = (0x00000000000000FF & (V)) >> 000, (P) + 4)
#define WRITE64LE(P, V)                        \
  ((P)[0] = (0x00000000000000FF & (V)) >> 000, \
   (P)[1] = (0x000000000000FF00 & (V)) >> 010, \
   (P)[2] = (0x0000000000FF0000 & (V)) >> 020, \
   (P)[3] = (0x00000000FF000000 & (V)) >> 030, \
   (P)[4] = (0x000000FF00000000 & (V)) >> 040, \
   (P)[5] = (0x0000FF0000000000 & (V)) >> 050, \
   (P)[6] = (0x00FF000000000000 & (V)) >> 060, \
   (P)[7] = (0xFF00000000000000 & (V)) >> 070, (P) + 8)
#define WRITE64BE(P, V)                        \
  ((P)[0] = (0xFF00000000000000 & (V)) >> 070, \
   (P)[1] = (0x00FF000000000000 & (V)) >> 060, \
   (P)[2] = (0x0000FF0000000000 & (V)) >> 050, \
   (P)[3] = (0x000000FF00000000 & (V)) >> 040, \
   (P)[4] = (0x00000000FF000000 & (V)) >> 030, \
   (P)[5] = (0x0000000000FF0000 & (V)) >> 020, \
   (P)[6] = (0x000000000000FF00 & (V)) >> 010, \
   (P)[7] = (0x00000000000000FF & (V)) >> 000, (P) + 8)

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § bits » some assembly required                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)

#define lockinc(MEM) __ArithmeticOp1("lock inc", MEM)
#define lockdec(MEM) __ArithmeticOp1("lock dec", MEM)
#define locknot(MEM) __ArithmeticOp1("lock not", MEM)
#define lockneg(MEM) __ArithmeticOp1("lock neg", MEM)

#define lockaddeq(MEM, VAL) __ArithmeticOp2("lock add", VAL, MEM)
#define locksubeq(MEM, VAL) __ArithmeticOp2("lock sub", VAL, MEM)
#define lockxoreq(MEM, VAL) __ArithmeticOp2("lock xor", VAL, MEM)
#define lockandeq(MEM, VAL) __ArithmeticOp2("lock and", VAL, MEM)
#define lockoreq(MEM, VAL)  __ArithmeticOp2("lock or", VAL, MEM)

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § bits » implementation details                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define __ArithmeticOp1(OP, MEM)                               \
  ({                                                           \
    asm(OP "%z0\t%0" : "+m"(*(MEM)) : /* no inputs */ : "cc"); \
    MEM;                                                       \
  })

#define __ArithmeticOp2(OP, VAL, MEM)                          \
  ({                                                           \
    asm(OP "%z0\t%1,%0" : "+m,m"(*(MEM)) : "i,r"(VAL) : "cc"); \
    MEM;                                                       \
  })

#endif /* __GNUC__ && !__STRICT_ANSI__ */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_H_ */
