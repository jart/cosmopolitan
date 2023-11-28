#ifndef COSMOPOLITAN_THIRD_PARTY_ARGON2_ENCODING_H_
#define COSMOPOLITAN_THIRD_PARTY_ARGON2_ENCODING_H_
#include "third_party/argon2/argon2.h"
COSMOPOLITAN_C_START_

#define ARGON2_MAX_DECODED_LANES    UINT32_C(255)
#define ARGON2_MIN_DECODED_SALT_LEN UINT32_C(8)
#define ARGON2_MIN_DECODED_OUT_LEN  UINT32_C(12)

int encode_string(char *, size_t, argon2_context *, argon2_type);
int decode_string(argon2_context *, const char *, argon2_type);
size_t b64len(uint32_t);
size_t numlen(uint32_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_ARGON2_ENCODING_H_ */
