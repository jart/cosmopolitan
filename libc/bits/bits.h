#ifndef COSMOPOLITAN_LIBC_BITS_H_
#define COSMOPOLITAN_LIBC_BITS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

#define CheckUnsigned(x) ((x) / !((typeof(x))(-1) < 0))

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § bits                                                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

extern const bool kTrue;
extern const bool kFalse;
extern const uint8_t kReverseBits[256];

uint16_t bswap_16(uint16_t) pureconst;
uint32_t bswap_32(uint32_t) pureconst;
uint32_t bswap_64(uint32_t) pureconst;
unsigned long popcount(unsigned long) pureconst;
uint32_t gray(uint32_t) pureconst;
uint32_t ungray(uint32_t) pureconst;
unsigned bcdadd(unsigned, unsigned) pureconst;
unsigned long bcd2i(unsigned long) pureconst;
unsigned long i2bcd(unsigned long) pureconst;
void bcxcpy(unsigned char (*)[16], unsigned long);
int ffs(int) pureconst;
int ffsl(long int) pureconst;
int ffsll(long long int) pureconst;
int fls(int) pureconst;
int flsl(long int) pureconst;
int flsll(long long int) pureconst;
uint8_t bitreverse8(uint8_t) libcesque pureconst;
uint16_t bitreverse16(uint16_t) libcesque pureconst;
uint32_t bitreverse32(uint32_t) libcesque pureconst;
uint64_t bitreverse64(uint64_t) libcesque pureconst;
unsigned long roundup2pow(unsigned long) libcesque pureconst;
unsigned long roundup2log(unsigned long) libcesque pureconst;
unsigned long rounddown2pow(unsigned long) libcesque pureconst;

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § bits » no assembly required                               ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

/**
 * Undocumented incantations for ROR, ROL, and SAR.
 */
#define ROR(w, k) (CheckUnsigned(w) >> (k) | (w) << (sizeof(w) * 8 - (k)))
#define ROL(w, k) ((w) << (k) | CheckUnsigned(w) >> (sizeof(w) * 8 - (k)))
#define SAR(w, k) (((w) & ~(~0u >> (k))) | ((w) >> ((k) & (sizeof(w) * 8 - 1))))

#define bitreverse8(X) (kReverseBits[(X)&0xff])
#define bitreverse16(X)                      \
  ((uint16_t)kReverseBits[(X)&0xff] << 010 | \
   kReverseBits[((uint16_t)(X) >> 010) & 0xff])

#ifndef __GNUC__
#define READ16LE(P) ((unsigned)(P)[1] << 010 | (unsigned)(P)[0])
#define READ32LE(P)                                              \
  ((unsigned long)(P)[3] << 030 | (unsigned long)(P)[2] << 020 | \
   (unsigned long)(P)[1] << 010 | (unsigned long)(P)[0])
#define READ64LE(P)                                                   \
  ((unsigned long long)(P)[3] << 030 | (unsigned long)(P)[2] << 020 | \
   (unsigned long long)(P)[1] << 010 | (unsigned long)(P)[0])
#else
#define READ16LE(P) read16le(P)
#define READ32LE(P) read32le(P)
#define READ64LE(P) read64le(P)
#define read16le(P)                                       \
  ({                                                      \
    const unsigned char *Pu = (const unsigned char *)(P); \
    (uint16_t) Pu[1] << 010 | (uint16_t)Pu[0];            \
  })
#define read32le(P)                                       \
  ({                                                      \
    const unsigned char *Pu = (const unsigned char *)(P); \
    ((uint32_t)Pu[3] << 030 | (uint32_t)Pu[2] << 020 |    \
     (uint32_t)Pu[1] << 010 | (uint32_t)Pu[0] << 000);    \
  })
#define read64le(P)                                       \
  ({                                                      \
    const unsigned char *Pu = (const unsigned char *)(P); \
    ((uint64_t)Pu[7] << 070 | (uint64_t)Pu[6] << 060 |    \
     (uint64_t)Pu[5] << 050 | (uint64_t)Pu[4] << 040 |    \
     (uint64_t)Pu[3] << 030 | (uint64_t)Pu[2] << 020 |    \
     (uint64_t)Pu[1] << 010 | (uint64_t)Pu[0] << 000);    \
  })
#endif

#define WRITE16LE(P, V)             \
  do {                              \
    uint8_t *Ple = (P);             \
    uint16_t Vle = (V);             \
    Ple[0] = (uint8_t)(Vle >> 000); \
    Ple[1] = (uint8_t)(Vle >> 010); \
  } while (0)

#define WRITE32LE(P, V)             \
  do {                              \
    uint8_t *Ple = (P);             \
    uint32_t Vle = (V);             \
    Ple[0] = (uint8_t)(Vle >> 000); \
    Ple[1] = (uint8_t)(Vle >> 010); \
    Ple[2] = (uint8_t)(Vle >> 020); \
    Ple[3] = (uint8_t)(Vle >> 030); \
  } while (0)

#define WRITE64LE(P, V)             \
  do {                              \
    uint8_t *Ple = (P);             \
    uint64_t Vle = (V);             \
    Ple[0] = (uint8_t)(Vle >> 000); \
    Ple[1] = (uint8_t)(Vle >> 010); \
    Ple[2] = (uint8_t)(Vle >> 020); \
    Ple[3] = (uint8_t)(Vle >> 030); \
    Ple[4] = (uint8_t)(Vle >> 040); \
    Ple[5] = (uint8_t)(Vle >> 050); \
    Ple[6] = (uint8_t)(Vle >> 060); \
    Ple[7] = (uint8_t)(Vle >> 070); \
  } while (0)

/* TODO(jart): these ones aren't coded correctly */
#define read128le(P) ((uint128_t)read64le((P) + 8) << 0100 | read64le(P))
#define read16be(P)  ((uint16_t)(*(P) << 010) | (uint16_t)(*((P) + 1)))
#define read32be(P)  ((uint32_t)read16be(P) << 020 | (uint32_t)read16be((P) + 2))
#define read64be(P)  ((uint64_t)read32be(P) << 040 | read32be((P) + 4))
#define read128be(P) ((uint128_t)read64be(P) << 0100 | read64be((P) + 8))

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § bits » some assembly required                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

/**
 * Constraints for virtual machine flags.
 * @note we beseech clang devs for flag constraints
 */
#ifdef __GCC_ASM_FLAG_OUTPUTS__ /* GCC6+ CLANG10+ */
#define CF            "=@ccc"
#define CFLAG(OP)     OP
#define ZF            "=@ccz"
#define ZFLAG(OP)     OP
#define OF            "=@cco"
#define OFLAG(OP)     OP
#define SF            "=@ccs"
#define SFLAG(SP)     SP
#define ABOVEF        "=@cca" /* i.e. !ZF && !CF */
#define ABOVEFLAG(OP) OP
#else
#define CF            "=q"
#define CFLAG(OP)     OP "\n\tsetc\t%b0"
#define ZF            "=q"
#define ZFLAG(OP)     OP "\n\tsetz\t%b0"
#define OF            "=q"
#define OFLAG(OP)     OP "\n\tseto\t%b0"
#define SF            "=q"
#define SFLAG(SP)     OP "\n\tsets\t%b0"
#define ABOVEF        "=@cca"
#define ABOVEFLAG(OP) OP "\n\tseta\t%b0"
#endif

/**
 * Reads scalar from memory, offset by segment.
 *
 * @return *(MEM) relative to segment
 * @see arch_prctl()
 * @see pushpop()
 */
#define fs(MEM) __peek("fs", MEM)
#define gs(MEM) __peek("gs", MEM)

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
#define atomic_store(MEM, VAL)                        \
  ({                                                  \
    autotype(VAL) Val = (VAL);                        \
    typeof(&Val) Mem = (MEM);                         \
    asm("mov%z1\t%1,%0" : "=m,m"(*Mem) : "i,r"(Val)); \
    Val;                                              \
  })

/**
 * Returns true if bit is set in memory.
 *
 * This is a generically-typed Bitset<T> ∀ RAM. This macro is intended
 * to be container-like with optimal machine instruction encoding, cf.
 * machine-agnostic container abstractions. Memory accesses are words.
 * Register allocation can be avoided if BIT is known. Be careful when
 * casting character arrays since that should cause a page fault.
 *
 * @param MEM is uint𝑘_t[] where 𝑘 ∈ {16,32,64} base address
 * @param BIT ∈ [-(2**(𝑘-1)),2**(𝑘-1)) is zero-based index
 * @return true if bit is set, otherwise false
 * @see Intel's Six Thousand Page Manual V.2A 3-113
 * @see bts(), btr(), btc()
 */
#define bt(MEM, BIT)                                                          \
  ({                                                                          \
    bool OldBit;                                                              \
    if (isconstant(BIT)) {                                                    \
      asm(CFLAG("bt%z1\t%2,%1")                                               \
          : CF(OldBit)                                                        \
          : "m"((MEM)[(BIT) / (sizeof((MEM)[0]) * CHAR_BIT)]),                \
            "J"((BIT) % (sizeof((MEM)[0]) * CHAR_BIT))                        \
          : "cc");                                                            \
    } else if (sizeof((MEM)[0]) == 2) {                                       \
      asm(CFLAG("bt\t%w2,%1") : CF(OldBit) : "m"((MEM)[0]), "r"(BIT) : "cc"); \
    } else if (sizeof((MEM)[0]) == 4) {                                       \
      asm(CFLAG("bt\t%k2,%1") : CF(OldBit) : "m"((MEM)[0]), "r"(BIT) : "cc"); \
    } else if (sizeof((MEM)[0]) == 8) {                                       \
      asm(CFLAG("bt\t%q2,%1") : CF(OldBit) : "m"((MEM)[0]), "r"(BIT) : "cc"); \
    }                                                                         \
    OldBit;                                                                   \
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
#define lockxchg(MEMORY, LOCALVAR)                                          \
  ({                                                                        \
    static_assert(typescompatible(typeof(*(MEMORY)), typeof(*(LOCALVAR)))); \
    asm("xchg\t%0,%1" : "+%m"(*(MEMORY)), "+r"(*(LOCALVAR)));               \
    *(LOCALVAR);                                                            \
  })

/**
 * Compares and exchanges.
 *
 * @param IFTHING is uint𝑘_t[hasatleast 1] where 𝑘 ∈ {8,16,32,64}
 * @return true if value was exchanged, otherwise false
 * @see lockcmpxchg()
 */
#define cmpxchg(IFTHING, ISEQUALTOME, REPLACEITWITHME)      \
  ({                                                        \
    bool DidIt;                                             \
    asm(ZFLAG("cmpxchg\t%3,%1")                             \
        : ZF(DidIt), "+m"(*(IFTHING)), "+a"(*(ISEQUALTOME)) \
        : "r"((typeof(*(IFTHING)))(REPLACEITWITHME))        \
        : "cc");                                            \
    DidIt;                                                  \
  })

#define ezcmpxchg(IFTHING, ISEQUALTOME, REPLACEITWITHME)  \
  ({                                                      \
    bool DidIt;                                           \
    autotype(IFTHING) IfThing = (IFTHING);                \
    typeof(*IfThing) IsEqualToMe = (ISEQUALTOME);         \
    typeof(*IfThing) ReplaceItWithMe = (REPLACEITWITHME); \
    asm(ZFLAG("cmpxchg\t%3,%1")                           \
        : ZF(DidIt), "+m"(*IfThing), "+a"(IsEqualToMe)    \
        : "r"(ReplaceItWithMe)                            \
        : "cc");                                          \
    DidIt;                                                \
  })

/**
 * Compares and exchanges w/ one operation.
 *
 * @param IFTHING is uint𝑘_t[hasatleast 1] where 𝑘 ∈ {8,16,32,64}
 * @return true if value was exchanged, otherwise false
 * @see lockcmpxchg()
 */
#define lockcmpxchg(IFTHING, ISEQUALTOME, REPLACEITWITHME)  \
  ({                                                        \
    bool DidIt;                                             \
    asm(ZFLAG("lock cmpxchg\t%3,%1")                        \
        : ZF(DidIt), "+m"(*(IFTHING)), "+a"(*(ISEQUALTOME)) \
        : "r"((typeof(*(IFTHING)))(REPLACEITWITHME))        \
        : "cc");                                            \
    DidIt;                                                  \
  })

/**
 * Gets value of extended control register.
 */
#define xgetbv(xcr_register_num)                               \
  ({                                                           \
    unsigned hi, lo;                                           \
    asm("xgetbv" : "=d"(hi), "=a"(lo) : "c"(cr_register_num)); \
    (uint64_t) hi << 32 | lo;                                  \
  })

/**
 * Reads model-specific register.
 * @note programs running as guests won't have authorization
 */
#define rdmsr(msr)                                         \
  ({                                                       \
    uint32_t lo, hi;                                       \
    asm volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr)); \
    (uint64_t) hi << 32 | lo;                              \
  })

/**
 * Writes model-specific register.
 * @note programs running as guests won't have authorization
 */
#define wrmsr(msr, val)                           \
  do {                                            \
    uint64_t val_ = (val);                        \
    asm volatile("wrmsr"                          \
                 : /* no outputs */               \
                 : "c"(msr), "a"((uint32_t)val_), \
                   "d"((uint32_t)(val_ >> 32)));  \
  } while (0)

/**
 * Tells CPU page tables changed for virtual address.
 * @note programs running as guests won't have authorization
 */
#define invlpg(MEM) \
  asm volatile("invlpg\t(%0)" : /* no outputs */ : "r"(MEM) : "memory")

/**
 * Teleports code fragment inside _init().
 */
#define INITIALIZER(PRI, NAME, CODE)                               \
  asm(".pushsection .init." #PRI "." #NAME ",\"ax\",@progbits\n\t" \
      "call\t" #NAME "\n\t"                                        \
      ".popsection");                                              \
  textstartup optimizesize void NAME(char *rdi, const char *rsi) { \
    CODE;                                                          \
    asm volatile("" : /* no outputs */ : "D"(rdi), "S"(rsi));      \
  }

#ifndef __STRICT_ANSI__
#if __PIC__ + __code_model_medium__ + __code_model_large__ + 0 > 1
#define __EZLEA(SYMBOL) "lea\t" SYMBOL "(%%rip),%"
#else
#define __EZLEA(SYMBOL) "mov\t$" SYMBOL ",%k"
#endif
#define weaken(symbol) ((const typeof(&(symbol)))weakaddr(#symbol))
#define strongaddr(symbolstr)                  \
  ({                                           \
    intptr_t waddr;                            \
    asm(__EZLEA(symbolstr) "0" : "=r"(waddr)); \
    waddr;                                     \
  })
#define weakaddr(symbolstr)                                               \
  ({                                                                      \
    intptr_t waddr;                                                       \
    asm(".weak\t" symbolstr "\n\t" __EZLEA(symbolstr) "0" : "=r"(waddr)); \
    waddr;                                                                \
  })
#else
#define weaken(symbol)      symbol
#define weakaddr(symbolstr) &(symbolstr)
#endif

#define slowcall(fn, arg1, arg2, arg3, arg4, arg5, arg6)                \
  ({                                                                    \
    void *ax;                                                           \
    asm volatile("push\t%7\n\t"                                         \
                 "push\t%6\n\t"                                         \
                 "push\t%5\n\t"                                         \
                 "push\t%4\n\t"                                         \
                 "push\t%3\n\t"                                         \
                 "push\t%2\n\t"                                         \
                 "push\t%1\n\t"                                         \
                 "call\tslowcall"                                       \
                 : "=a"(ax)                                             \
                 : "g"(fn), "g"(arg1), "g"(arg2), "g"(arg3), "g"(arg4), \
                   "g"(arg5), "g"(arg6)                                 \
                 : "memory");                                           \
    ax;                                                                 \
  })

#define IsAddressCanonicalForm(P)                             \
  ({                                                          \
    intptr_t p2 = (intptr_t)(P);                              \
    (0xffff800000000000l <= p2 && p2 <= 0x00007fffffffffffl); \
  })

/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § bits » optimizations                                      ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/
#if defined(__GNUC__) && !defined(__STRICT_ANSI__)

#define popcount(X) (isconstant(X) ? __builtin_popcount(X) : __popcount(X))
#define popcount$nehalem(X)                                    \
  ({                                                           \
    typeof(X) BitCount;                                        \
    asm("popcnt\t%1,%0" : "=r,r"(BitCount) : "r,m"(X) : "cc"); \
    BitCount;                                                  \
  })
#ifdef __POPCNT__
#define __popcount(X) popcount$nehalem(X)
#else
#define __popcount(X) (popcount)(X)
#endif

#define bswap_16(U16)                                                         \
  (isconstant(U16) ? ((((U16)&0xff00) >> 010) | (((U16)&0x00ff) << 010)) : ({ \
    uint16_t Swapped16, Werd16 = (U16);                                       \
    asm("xchg\t%b0,%h0" : "=Q"(Swapped16) : "0"(Werd16));                     \
    Swapped16;                                                                \
  }))

#define bswap_32(U32)                                                 \
  (isconstant(U32)                                                    \
       ? ((((U32)&0xff000000) >> 030) | (((U32)&0x000000ff) << 030) | \
          (((U32)&0x00ff0000) >> 010) | (((U32)&0x0000ff00) << 010))  \
       : ({                                                           \
           uint32_t Swapped32, Werd32 = (U32);                        \
           asm("bswap\t%0" : "=r"(Swapped32) : "0"(Werd32));          \
           Swapped32;                                                 \
         }))

#define bswap_64(U64)                                                    \
  (isconstant(U64) ? ((((U64)&0xff00000000000000ul) >> 070) |            \
                      (((U64)&0x00000000000000fful) << 070) |            \
                      (((U64)&0x00ff000000000000ul) >> 050) |            \
                      (((U64)&0x000000000000ff00ul) << 050) |            \
                      (((U64)&0x0000ff0000000000ul) >> 030) |            \
                      (((U64)&0x0000000000ff0000ul) << 030) |            \
                      (((U64)&0x000000ff00000000ul) >> 010) |            \
                      (((U64)&0x00000000ff000000ul) << 010))             \
                   : ({                                                  \
                       uint64_t Swapped64, Werd64 = (U64);               \
                       asm("bswap\t%0" : "=r"(Swapped64) : "0"(Werd64)); \
                       Swapped64;                                        \
                     }))

#endif /* defined(__GNUC__) && !defined(__STRICT_ANSI__) */
/*───────────────────────────────────────────────────────────────────────────│─╗
│ cosmopolitan § bits » implementation details                             ─╬─│┼
╚────────────────────────────────────────────────────────────────────────────│*/

#define __peek(SEGMENT, ADDRESS)                                  \
  ({                                                              \
    typeof(*(ADDRESS)) Pk;                                        \
    asm("mov\t%%" SEGMENT ":%1,%0" : "=r"(Pk) : "m"(*(ADDRESS))); \
    Pk;                                                           \
  })

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

#define __BitOp(OP, BIT, MEM)                                              \
  ({                                                                       \
    bool OldBit;                                                           \
    if (isconstant(BIT)) {                                                 \
      asm(CFLAG(OP "%z1\t%2,%1")                                           \
          : CF(OldBit), "+m"((MEM)[(BIT) / (sizeof((MEM)[0]) * CHAR_BIT)]) \
          : "J"((BIT) % (sizeof((MEM)[0]) * CHAR_BIT))                     \
          : "cc");                                                         \
    } else if (sizeof((MEM)[0]) == 2) {                                    \
      asm(CFLAG(OP "\t%w2,%1")                                             \
          : CF(OldBit), "+m"((MEM)[0])                                     \
          : "r"(BIT)                                                       \
          : "cc");                                                         \
    } else if (sizeof((MEM)[0]) == 4) {                                    \
      asm(CFLAG(OP "\t%k2,%1")                                             \
          : CF(OldBit), "+m"((MEM)[0])                                     \
          : "r"(BIT)                                                       \
          : "cc");                                                         \
    } else if (sizeof((MEM)[0]) == 8) {                                    \
      asm(CFLAG(OP "\t%q2,%1")                                             \
          : CF(OldBit), "+m"((MEM)[0])                                     \
          : "r"(BIT)                                                       \
          : "cc");                                                         \
    }                                                                      \
    OldBit;                                                                \
  })

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_H_ */
