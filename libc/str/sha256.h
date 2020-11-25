#ifndef COSMOPOLITAN_LIBC_STR_SHA256_H_
#define COSMOPOLITAN_LIBC_STR_SHA256_H_

#define SHA256_BLOCK_SIZE 32

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct Sha256Ctx {
  uint8_t data[64];
  uint32_t datalen;
  uint64_t bitlen;
  uint32_t state[8];
};

void sha256_init(struct Sha256Ctx *);
void sha256_update(struct Sha256Ctx *, const uint8_t *, size_t);
void sha256_final(struct Sha256Ctx *, uint8_t *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_STR_SHA256_H_ */
