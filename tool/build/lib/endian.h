#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_ENDIAN_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_ENDIAN_H_
#include "libc/dce.h"
#include "libc/str/str.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
#if __BYTE_ORDER__ + 0 == 1234

#define Read8(P)        \
  ({                    \
    uint8_t *Ptr = (P); \
    *Ptr;               \
  })

#define Read16(P)         \
  ({                      \
    uint16_t Res;         \
    uint8_t *Ptr = (P);   \
    memcpy(&Res, Ptr, 2); \
    Res;                  \
  })

#define Read32(P)         \
  ({                      \
    uint32_t Res;         \
    uint8_t *Ptr = (P);   \
    memcpy(&Res, Ptr, 4); \
    Res;                  \
  })

#define Read64(P)         \
  ({                      \
    uint64_t Res;         \
    uint8_t *Ptr = (P);   \
    memcpy(&Res, Ptr, 8); \
    Res;                  \
  })

#define Write8(P, B)    \
  do {                  \
    uint8_t *Ptr = (P); \
    *Ptr = (B);         \
  } while (0)

#define Write16(P, V)     \
  do {                    \
    uint16_t Val = (V);   \
    uint8_t *Ptr = (P);   \
    memcpy(Ptr, &Val, 2); \
  } while (0)

#define Write32(P, V)     \
  do {                    \
    uint32_t Val = (V);   \
    uint8_t *Ptr = (P);   \
    memcpy(Ptr, &Val, 4); \
  } while (0)

#define Write64(P, V)     \
  do {                    \
    uint64_t Val = (V);   \
    uint8_t *Ptr = (P);   \
    memcpy(Ptr, &Val, 8); \
  } while (0)

#else

forceinline uint16_t Read8(const uint8_t p[hasatleast 1]) {
  return p[0];
}

forceinline uint16_t Read16(const uint8_t p[hasatleast 2]) {
  return p[0] | p[1] << 010;
}

forceinline uint32_t Read32(const uint8_t bytes[hasatleast 4]) {
  return (uint32_t)bytes[0] << 000 | (uint32_t)bytes[1] << 010 |
         (uint32_t)bytes[2] << 020 | (uint32_t)bytes[3] << 030;
}

forceinline uint64_t Read64(const uint8_t bytes[hasatleast 8]) {
  return (uint64_t)bytes[0] << 000 | (uint64_t)bytes[1] << 010 |
         (uint64_t)bytes[2] << 020 | (uint64_t)bytes[3] << 030 |
         (uint64_t)bytes[4] << 040 | (uint64_t)bytes[5] << 050 |
         (uint64_t)bytes[6] << 060 | (uint64_t)bytes[7] << 070;
}

forceinline void Write8(unsigned char p[hasatleast 1], uint8_t x) {
  p[0] = x >> 000;
}

forceinline void Write16(unsigned char p[hasatleast 2], uint16_t x) {
  p[0] = x >> 000;
  p[1] = x >> 010;
}

forceinline void Write32(unsigned char p[hasatleast 4], uint64_t x) {
  p[0] = x >> 000;
  p[1] = x >> 010;
  p[2] = x >> 020;
  p[3] = x >> 030;
}

forceinline void Write64(unsigned char p[hasatleast 8], uint64_t x) {
  p[0] = x >> 000;
  p[1] = x >> 010;
  p[2] = x >> 020;
  p[3] = x >> 030;
  p[4] = x >> 040;
  p[5] = x >> 050;
  p[6] = x >> 060;
  p[7] = x >> 070;
}

#endif /* ENDIAN */
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_ENDIAN_H_ */
