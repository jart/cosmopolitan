#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_ENDIAN_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_ENDIAN_H_
#include "libc/bits/bits.h"

#define Read8(P) (*(const uint8_t *)(P))

#define Read16(P)                              \
  ({                                           \
    const uint8_t *Ptr = (const uint8_t *)(P); \
    READ16LE(P);                               \
  })

#define Read32(P)                              \
  ({                                           \
    const uint8_t *Ptr = (const uint8_t *)(P); \
    READ32LE(P);                               \
  })

#define Read64(P)                              \
  ({                                           \
    const uint8_t *Ptr = (const uint8_t *)(P); \
    READ64LE(P);                               \
  })

#define Write8(P, V)    \
  do {                  \
    uint8_t Val = (V);  \
    uint8_t *Ptr = (P); \
    *Ptr = Val;         \
  } while (0)

#define Write16(P, V)    \
  do {                   \
    uint16_t Val = (V);  \
    uint8_t *Ptr = (P);  \
    WRITE16LE(Ptr, Val); \
  } while (0)

#define Write32(P, V)    \
  do {                   \
    uint32_t Val = (V);  \
    uint8_t *Ptr = (P);  \
    WRITE32LE(Ptr, Val); \
  } while (0)

#define Write64(P, V)    \
  do {                   \
    uint64_t Val = (V);  \
    uint8_t *Ptr = (P);  \
    WRITE64LE(Ptr, Val); \
  } while (0)

#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_ENDIAN_H_ */
