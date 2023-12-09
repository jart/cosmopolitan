#ifndef COSMOPOLITAN_THIRD_PARTY_MUSL_RESOLV_INTERNAL_H_
#define COSMOPOLITAN_THIRD_PARTY_MUSL_RESOLV_INTERNAL_H_
COSMOPOLITAN_C_START_

int __dn_expand(const unsigned char *, const unsigned char *, const unsigned char *, char *, int);
int __res_mkquery(int, const char *, int, int, const unsigned char *, int, const unsigned char*, unsigned char *, int);
int __res_send(const unsigned char *, int, unsigned char *, int);
int __res_msend(int, const unsigned char *const *, const int *, unsigned char *const *, int *, int);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_MUSL_RESOLV_INTERNAL_H_ */
