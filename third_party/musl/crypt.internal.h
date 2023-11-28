#ifndef COSMOPOLITAN_THIRD_PARTY_MUSL_CRYPT_INTERNAL_H_
#define COSMOPOLITAN_THIRD_PARTY_MUSL_CRYPT_INTERNAL_H_
#include "third_party/musl/crypt.h"
COSMOPOLITAN_C_START_

char *__crypt_des(const char *, const char *, char *);
char *__crypt_md5(const char *, const char *, char *);
char *__crypt_blowfish(const char *, const char *, char *);
char *__crypt_sha256(const char *, const char *, char *);
char *__crypt_sha512(const char *, const char *, char *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_MUSL_CRYPT_INTERNAL_H_ */
