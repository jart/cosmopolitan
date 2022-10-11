#ifndef COSMOPOLITAN_THIRD_PARTY_MUSL_CRYPT_H_
#define COSMOPOLITAN_THIRD_PARTY_MUSL_CRYPT_H_
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct crypt_data {
  int initialized;
  char __buf[256];
};

void encrypt(char *, int);
void setkey(const char *);
char *crypt(const char *, const char *);
char *crypt_r(const char *, const char *, struct crypt_data *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_MUSL_CRYPT_H_ */
