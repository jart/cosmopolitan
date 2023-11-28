#ifndef COSMOPOLITAN_LIBC_STR_BLAKE2_H_
#define COSMOPOLITAN_LIBC_STR_BLAKE2_H_

#define BLAKE2B256_DIGEST_LENGTH 32
#define BLAKE2B_CBLOCK           128

#define BLAKE2B256_Init    __BLAKE2B256_Init
#define BLAKE2B256_Update  __BLAKE2B256_Update
#define BLAKE2B256_Process __BLAKE2B256_Process
#define BLAKE2B256_Final   __BLAKE2B256_Final
#define BLAKE2B256         __BLAKE2B256

COSMOPOLITAN_C_START_

struct Blake2b {
  uint64_t h[8];
  uint64_t t_low;
  uint64_t t_high;
  union {
    uint8_t bytes[BLAKE2B_CBLOCK];
    uint64_t words[16];
  } block;
  size_t block_used;
};

int BLAKE2B256_Init(struct Blake2b *);
int BLAKE2B256_Update(struct Blake2b *, const void *, size_t);
int BLAKE2B256_Process(struct Blake2b *, const uint64_t[BLAKE2B_CBLOCK / 8]);
int BLAKE2B256_Final(struct Blake2b *, uint8_t[BLAKE2B256_DIGEST_LENGTH]);
int BLAKE2B256(const void *, size_t, uint8_t[BLAKE2B256_DIGEST_LENGTH]);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_LIBC_STR_BLAKE2_H_ */
