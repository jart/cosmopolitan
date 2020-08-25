#ifndef COSMOPOLITAN_LIBC_BITS_BSWAP_H_
#define COSMOPOLITAN_LIBC_BITS_BSWAP_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

uint16_t bswap_16(uint16_t) pureconst;
uint32_t bswap_32(uint32_t) pureconst;
uint32_t bswap_64(uint32_t) pureconst;

#if defined(__GNUC__) && !defined(__STRICT_ANSI__)

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

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_BSWAP_H_ */
