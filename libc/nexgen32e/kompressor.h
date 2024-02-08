#ifndef COSMOPOLITAN_LIBC_KOMPRESSOR_KOMPRESSOR_H_
#define COSMOPOLITAN_LIBC_KOMPRESSOR_KOMPRESSOR_H_
#ifdef _COSMO_SOURCE

#define rldecode  __rldecode
#define rldecode2 __rldecode2
#define lz4check  __lz4check
#define lz4cpy    __lz4cpy
#define lz4len    __lz4len
#define lz4decode __lz4decode

COSMOPOLITAN_C_START_

struct RlDecode {
  uint8_t repititions;
  uint8_t byte;
};

void rldecode(void *dest, const struct RlDecode *);
void rldecode2(void *dest, const struct RlDecode *);
const uint8_t *lz4check(const void *data);
void *lz4cpy(void *dest, const void *blockdata, size_t blocksize);
size_t lz4len(const void *blockdata, size_t blocksize);
void *lz4decode(void *dest, const void *src);

COSMOPOLITAN_C_END_
#endif /* _COSMO_SOURCE */
#endif /* COSMOPOLITAN_LIBC_KOMPRESSOR_KOMPRESSOR_H_ */
