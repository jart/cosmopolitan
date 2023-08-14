#ifndef COSMOPOLITAN_LIBC_BITS_H_
#define COSMOPOLITAN_LIBC_BITS_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
#ifdef _COSMO_SOURCE
COSMOPOLITAN_C_START_

int _bitreverse8(int) pureconst;
int _bitreverse16(int) pureconst;
uint32_t _bitreverse32(uint32_t) pureconst;
uint64_t _bitreverse64(uint64_t) pureconst;

#define READ16LE(P)                    \
  (__extension__({                     \
    uint16_t __x;                      \
    __builtin_memcpy(&__x, P, 16 / 8); \
    __x;                               \
  }))

#define READ16BE(P)                    \
  (__extension__({                     \
    uint16_t __x;                      \
    __builtin_memcpy(&__x, P, 16 / 8); \
    __builtin_bswap16(__x);            \
  }))

#define READ32LE(P)                    \
  (__extension__({                     \
    uint32_t __x;                      \
    __builtin_memcpy(&__x, P, 32 / 8); \
    __x;                               \
  }))

#define READ32BE(P)                    \
  (__extension__({                     \
    uint32_t __x;                      \
    __builtin_memcpy(&__x, P, 32 / 8); \
    __builtin_bswap32(__x);            \
  }))

#define READ64LE(P)                    \
  (__extension__({                     \
    uint64_t __x;                      \
    __builtin_memcpy(&__x, P, 64 / 8); \
    __x;                               \
  }))

#define READ64BE(P)                    \
  (__extension__({                     \
    uint64_t __x;                      \
    __builtin_memcpy(&__x, P, 64 / 8); \
    __builtin_bswap64(__x);            \
  }))

#define WRITE16LE(P, X)                  \
  (__extension__({                       \
    __typeof__(&(P)[0]) __p = (P);       \
    uint16_t __x = (X);                  \
    __builtin_memcpy(__p, &__x, 16 / 8); \
    __p + 16 / 8;                        \
  }))

#define WRITE16BE(P, X)                  \
  (__extension__({                       \
    __typeof__(&(P)[0]) __p = (P);       \
    uint16_t __x = __builtin_bswap16(X); \
    __builtin_memcpy(__p, &__x, 16 / 8); \
    __p + 16 / 8;                        \
  }))

#define WRITE32LE(P, X)                  \
  (__extension__({                       \
    __typeof__(&(P)[0]) __p = (P);       \
    uint32_t __x = (X);                  \
    __builtin_memcpy(__p, &__x, 32 / 8); \
    __p + 32 / 8;                        \
  }))

#define WRITE32BE(P, X)                  \
  (__extension__({                       \
    __typeof__(&(P)[0]) __p = (P);       \
    uint32_t __x = __builtin_bswap32(X); \
    __builtin_memcpy(__p, &__x, 32 / 8); \
    __p + 32 / 8;                        \
  }))

#define WRITE64LE(P, X)                  \
  (__extension__({                       \
    __typeof__(&(P)[0]) __p = (P);       \
    uint64_t __x = (X);                  \
    __builtin_memcpy(__p, &__x, 64 / 8); \
    __p + 64 / 8;                        \
  }))

#define WRITE64BE(P, X)                  \
  (__extension__({                       \
    __typeof__(&(P)[0]) __p = (P);       \
    uint64_t __x = __builtin_bswap64(X); \
    __builtin_memcpy(__p, &__x, 64 / 8); \
    __p + 64 / 8;                        \
  }))

COSMOPOLITAN_C_END_
#endif /* _COSMO_SOURCE */
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_BITS_H_ */
