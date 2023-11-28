#ifndef COSMOPOLITAN_SERIALIZE_H_
#define COSMOPOLITAN_SERIALIZE_H_
#ifdef _COSMO_SOURCE

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define __SWAPBE16(x) (x)
#define __SWAPBE32(x) (x)
#define __SWAPBE64(x) (x)
#else
#define __SWAPBE16(x) __builtin_bswap16(x)
#define __SWAPBE32(x) __builtin_bswap32(x)
#define __SWAPBE64(x) __builtin_bswap64(x)
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define __SWAPLE16(x) (x)
#define __SWAPLE32(x) (x)
#define __SWAPLE64(x) (x)
#else
#define __SWAPLE16(x) __builtin_bswap16(x)
#define __SWAPLE32(x) __builtin_bswap32(x)
#define __SWAPLE64(x) __builtin_bswap64(x)
#endif

#define READ16LE(P)                    \
  (__extension__({                     \
    uint16_t __x;                      \
    __builtin_memcpy(&__x, P, 16 / 8); \
    __SWAPLE16(__x);                   \
  }))

#define READ16BE(P)                    \
  (__extension__({                     \
    uint16_t __x;                      \
    __builtin_memcpy(&__x, P, 16 / 8); \
    __SWAPBE16(__x);                   \
  }))

#define READ32LE(P)                    \
  (__extension__({                     \
    uint32_t __x;                      \
    __builtin_memcpy(&__x, P, 32 / 8); \
    __SWAPLE32(__x);                   \
  }))

#define READ32BE(P)                    \
  (__extension__({                     \
    uint32_t __x;                      \
    __builtin_memcpy(&__x, P, 32 / 8); \
    __SWAPBE32(__x);                   \
  }))

#define READ64LE(P)                    \
  (__extension__({                     \
    uint64_t __x;                      \
    __builtin_memcpy(&__x, P, 64 / 8); \
    __SWAPLE32(__x);                   \
  }))

#define READ64BE(P)                    \
  (__extension__({                     \
    uint64_t __x;                      \
    __builtin_memcpy(&__x, P, 64 / 8); \
    __SWAPBE64(__x);                   \
  }))

#define WRITE16LE(P, X)                  \
  (__extension__({                       \
    __typeof__(&(P)[0]) __p = (P);       \
    uint16_t __x = __SWAPLE16(X);        \
    __builtin_memcpy(__p, &__x, 16 / 8); \
    __p + 16 / 8;                        \
  }))

#define WRITE16BE(P, X)                  \
  (__extension__({                       \
    __typeof__(&(P)[0]) __p = (P);       \
    uint16_t __x = __SWAPBE16(X);        \
    __builtin_memcpy(__p, &__x, 16 / 8); \
    __p + 16 / 8;                        \
  }))

#define WRITE32LE(P, X)                  \
  (__extension__({                       \
    __typeof__(&(P)[0]) __p = (P);       \
    uint32_t __x = __SWAPLE32(X);        \
    __builtin_memcpy(__p, &__x, 32 / 8); \
    __p + 32 / 8;                        \
  }))

#define WRITE32BE(P, X)                  \
  (__extension__({                       \
    __typeof__(&(P)[0]) __p = (P);       \
    uint32_t __x = __SWAPBE32(X);        \
    __builtin_memcpy(__p, &__x, 32 / 8); \
    __p + 32 / 8;                        \
  }))

#define WRITE64LE(P, X)                  \
  (__extension__({                       \
    __typeof__(&(P)[0]) __p = (P);       \
    uint64_t __x = __SWAPLE64(X);        \
    __builtin_memcpy(__p, &__x, 64 / 8); \
    __p + 64 / 8;                        \
  }))

#define WRITE64BE(P, X)                  \
  (__extension__({                       \
    __typeof__(&(P)[0]) __p = (P);       \
    uint64_t __x = __SWAPBE64(X);        \
    __builtin_memcpy(__p, &__x, 64 / 8); \
    __p + 64 / 8;                        \
  }))

#endif /* _COSMO_SOURCE */
#endif /* COSMOPOLITAN_SERIALIZE_H_ */
