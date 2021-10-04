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
int bitreverse8(int) libcesque pureconst;
int bitreverse16(int) libcesque pureconst;
uint32_t bitreverse32(uint32_t) libcesque pureconst;
uint64_t bitreverse64(uint64_t) libcesque pureconst;
unsigned long roundup2pow(unsigned long) libcesque pureconst;
unsigned long roundup2log(unsigned long) libcesque pureconst;
unsigned long rounddown2pow(unsigned long) libcesque pureconst;
unsigned long hamming(unsigned long, unsigned long) pureconst;
intptr_t lockxchg(void *, void *, size_t);
bool cmpxchg(void *, intptr_t, intptr_t, size_t);
bool lockcmpxchg(void *, intptr_t, intptr_t, size_t);
intptr_t atomic_load(void *, size_t);
intptr_t atomic_store(void *, intptr_t, size_t);
unsigned bextra(const unsigned *, size_t, char);

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § bits » no assembly required                               ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define BITREVERSE8(X) (kReverseBits[255 & (X)])
#define BITREVERSE16(X) \
  (kReverseBits[0x00FF & (X)] << 8 | kReverseBits[(0xFF00 & (X)) >> 8])

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

/*
 * Constraints for virtual machine flags.
 * @note we beseech clang devs for flag constraints
 */
#ifdef __GCC_ASM_FLAG_OUTPUTS__ /* GCC6+ CLANG10+ */
#define CFLAG_CONSTRAINT  "=@ccc"
#define CFLAG_ASM(OP)     OP
#define ZFLAG_CONSTRAINT  "=@ccz"
#define ZFLAG_ASM(OP)     OP
#define OFLAG_CONSTRAINT  "=@cco"
#define OFLAG_ASM(OP)     OP
#define SFLAG_CONSTRAINT  "=@ccs"
#define SFLAG_ASM(SP)     SP
#define ABOVE_CONSTRAINT  "=@cca" /* i.e. !ZF && !CF */
#define ABOVEFLAG_ASM(OP) OP
#else
#define CFLAG_CONSTRAINT  "=q"
#define CFLAG_ASM(OP)     OP "\n\tsetc\t%b0"
#define ZFLAG_CONSTRAINT  "=q"
#define ZFLAG_ASM(OP)     OP "\n\tsetz\t%b0"
#define OFLAG_CONSTRAINT  "=q"
#define OFLAG_ASM(OP)     OP "\n\tseto\t%b0"
#define SFLAG_CONSTRAINT  "=q"
#define SFLAG_ASM(SP)     OP "\n\tsets\t%b0"
#define ABOVE_CONSTRAINT  "=@cca"
#define ABOVEFLAG_ASM(OP) OP "\n\tseta\t%b0"
#endif

/**
 * Reads scalar from memory w/ one operation.
 *
 * @param MEM is alignas(𝑘) uint𝑘_t[hasatleast 1] where 𝑘 ∈ {8,16,32,64}
 * @return *(MEM)
 * @note defeats compiler load tearing optimizations
 * @note alignas(𝑘) is implied if compiler knows type
 * @note alignas(𝑘) only avoids multi-core / cross-page edge cases
 * @see Intel's Six-Thousand Page Manual V.3A §8.2.3.1
 * @see atomic_store()
 */
#define atomic_load(MEM)                       \
  ({                                           \
    autotype(MEM) Mem = (MEM);                 \
    typeof(*Mem) Reg;                          \
    asm("mov\t%1,%0" : "=r"(Reg) : "m"(*Mem)); \
    Reg;                                       \
  })

/**
 * Saves scalar to memory w/ one operation.
 *
 * This is guaranteed to happen in either one or zero operations,
 * depending on whether or not it's possible for *(MEM) to be read
 * afterwards. This macro only forbids compiler from using >1 ops.
 *
 * @param MEM is alignas(𝑘) uint𝑘_t[hasatleast 1] where 𝑘 ∈ {8,16,32,64}
 * @param VAL is uint𝑘_t w/ better encoding for immediates (constexpr)
 * @return VAL
 * @note alignas(𝑘) on nexgen32e only needed for end of page gotcha
 * @note alignas(𝑘) is implied if compiler knows type
 * @note needed to defeat store tearing optimizations
 * @see Intel Six-Thousand Page Manual Manual V.3A §8.2.3.1
 * @see atomic_load()
 */
#define atomic_store(MEM, VAL)                    \
  ({                                              \
    autotype(VAL) Val = (VAL);                    \
    typeof(&Val) Mem = (MEM);                     \
    asm("mov%z1\t%1,%0" : "=m"(*Mem) : "r"(Val)); \
    Val;                                          \
  })

#define bts(MEM, BIT)     __BitOp("bts", BIT, MEM) /** bit test and set */
#define btr(MEM, BIT)     __BitOp("btr", BIT, MEM) /** bit test and reset */
#define btc(MEM, BIT)     __BitOp("btc", BIT, MEM) /** bit test and complement */
#define lockbts(MEM, BIT) __BitOp("lock bts", BIT, MEM)
#define lockbtr(MEM, BIT) __BitOp("lock btr", BIT, MEM)
#define lockbtc(MEM, BIT) __BitOp("lock btc", BIT, MEM)

#define lockinc(MEM) __ArithmeticOp1("lock inc", MEM)
#define lockdec(MEM) __ArithmeticOp1("lock dec", MEM)
#define locknot(MEM) __ArithmeticOp1("lock not", MEM)
#define lockneg(MEM) __ArithmeticOp1("lock neg", MEM)

#define lockaddeq(MEM, VAL) __ArithmeticOp2("lock add", VAL, MEM)
#define locksubeq(MEM, VAL) __ArithmeticOp2("lock sub", VAL, MEM)
#define lockxoreq(MEM, VAL) __ArithmeticOp2("lock xor", VAL, MEM)
#define lockandeq(MEM, VAL) __ArithmeticOp2("lock and", VAL, MEM)
#define lockoreq(MEM, VAL)  __ArithmeticOp2("lock or", VAL, MEM)

/**
 * Exchanges *MEMORY into *LOCALVAR w/ one operation.
 *
 * @param MEMORY is uint𝑘_t[hasatleast 1] where 𝑘 ∈ {8,16,32,64}
 * @param LOCALVAR is uint𝑘_t[hasatleast 1]
 * @return LOCALVAR[0]
 * @see xchg()
 */
#define lockxchg(MEMORY, LOCALVAR)                            \
  ({                                                          \
    asm("xchg\t%0,%1" : "+%m"(*(MEMORY)), "+r"(*(LOCALVAR))); \
    *(LOCALVAR);                                              \
  })

/**
 * Compares and exchanges.
 *
 * @param IFTHING is uint𝑘_t[hasatleast 1] where 𝑘 ∈ {8,16,32,64}
 * @return true if value was exchanged, otherwise false
 * @see lockcmpxchg()
 */
#define cmpxchg(IFTHING, ISEQUALTOME, REPLACEITWITHME)                        \
  ({                                                                          \
    bool DidIt;                                                               \
    autotype(IFTHING) IfThing = (IFTHING);                                    \
    typeof(*IfThing) IsEqualToMe = (ISEQUALTOME);                             \
    typeof(*IfThing) ReplaceItWithMe = (REPLACEITWITHME);                     \
    asm volatile(ZFLAG_ASM("cmpxchg\t%3,%1")                                  \
                 : ZFLAG_CONSTRAINT(DidIt), "+m"(*IfThing), "+a"(IsEqualToMe) \
                 : "r"(ReplaceItWithMe)                                       \
                 : "cc");                                                     \
    DidIt;                                                                    \
  })

/**
 * Compares and exchanges w/ one operation.
 *
 * @param IFTHING is uint𝑘_t[hasatleast 1] where 𝑘 ∈ {8,16,32,64}
 * @return true if value was exchanged, otherwise false
 * @see lockcmpxchg()
 */
#define lockcmpxchg(IFTHING, ISEQUALTOME, REPLACEITWITHME)                    \
  ({                                                                          \
    bool DidIt;                                                               \
    autotype(IFTHING) IfThing = (IFTHING);                                    \
    typeof(*IfThing) IsEqualToMe = (ISEQUALTOME);                             \
    typeof(*IfThing) ReplaceItWithMe = (REPLACEITWITHME);                     \
    asm volatile(ZFLAG_ASM("lock cmpxchg\t%3,%1")                             \
                 : ZFLAG_CONSTRAINT(DidIt), "+m"(*IfThing), "+a"(IsEqualToMe) \
                 : "r"(ReplaceItWithMe)                                       \
                 : "cc");                                                     \
    DidIt;                                                                    \
  })

#define IsAddressCanonicalForm(P)                             \
  ({                                                          \
    intptr_t p2 = (intptr_t)(P);                              \
    (0xffff800000000000l <= p2 && p2 <= 0x00007fffffffffffl); \
  })

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

#else
#define cmpxchg(MEM, CMP, VAL) \
  cmpxchg(MEM, (intptr_t)(CMP), (intptr_t)(VAL), sizeof(*(MEM)))
#define lockcmpxchg(MEM, CMP, VAL) \
  lockcmpxchg(MEM, (intptr_t)(CMP), (intptr_t)(VAL), sizeof(*(MEM)))
#define lockxchg(MEM, VAR) \
  lockxchg(MEM, VAR, sizeof(*(MEM)) / (sizeof(*(MEM)) == sizeof(*(VAR))))
#define atomic_store(MEM, VAL) \
  atomic_store(MEM, VAL, sizeof(*(MEM)) / (sizeof(*(MEM)) == sizeof(*(VAL))))
#define atomic_load(MEM) atomic_load(MEM, sizeof(*(MEM)))
#endif /* __GNUC__ && !__STRICT_ANSI__ */
COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_H_ */
