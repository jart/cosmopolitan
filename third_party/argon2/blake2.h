#ifndef COSMOPOLITAN_THIRD_PARTY_ARGON2_BLAKE2_H_
#define COSMOPOLITAN_THIRD_PARTY_ARGON2_BLAKE2_H_
#include "third_party/argon2/argon2.h"
COSMOPOLITAN_C_START_

enum blake2b_constant {
  BLAKE2B_BLOCKBYTES = 128,
  BLAKE2B_OUTBYTES = 64,
  BLAKE2B_KEYBYTES = 64,
  BLAKE2B_SALTBYTES = 16,
  BLAKE2B_PERSONALBYTES = 16
};

typedef struct __blake2b_param {
  uint8_t digest_length;                   /* 1 */
  uint8_t key_length;                      /* 2 */
  uint8_t fanout;                          /* 3 */
  uint8_t depth;                           /* 4 */
  uint8_t leaf_length[4];                  /* 8 */
  uint8_t node_offset[8];                  /* 16 */
  uint8_t node_depth;                      /* 17 */
  uint8_t inner_length;                    /* 18 */
  uint8_t reserved[14];                    /* 32 */
  uint8_t salt[BLAKE2B_SALTBYTES];         /* 48 */
  uint8_t personal[BLAKE2B_PERSONALBYTES]; /* 64 */
} blake2b_param;

typedef struct __blake2b_state {
  uint64_t h[8];
  uint64_t t[2];
  uint64_t f[2];
  uint8_t buf[BLAKE2B_BLOCKBYTES];
  unsigned buflen;
  unsigned outlen;
  uint8_t last_node;
} blake2b_state;

/* Streaming API */
int blake2b_init(blake2b_state *, size_t);
int blake2b_init_key(blake2b_state *, size_t, const void *, size_t);
int blake2b_init_param(blake2b_state *, const blake2b_param *);
int blake2b_update(blake2b_state *, const void *, size_t);
int blake2b_final(blake2b_state *, void *, size_t);

/* Simple API */
int blake2b(void *, size_t, const void *, size_t, const void *, size_t);
int blake2b_long(void *, size_t, const void *, size_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_ARGON2_BLAKE2_H_ */
