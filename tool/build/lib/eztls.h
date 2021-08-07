#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_EZTLS_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_EZTLS_H_
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/ssl.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct EzTlsBio {
  int fd, c;
  unsigned a, b;
  unsigned char t[4000];
  unsigned char u[1430];
};

extern struct EzTlsBio ezbio;
extern mbedtls_ssl_config ezconf;
extern mbedtls_ssl_context ezssl;
extern mbedtls_ctr_drbg_context ezrng;

void SetupPresharedKeySsl(int);
int EzTlsFlush(struct EzTlsBio *, const unsigned char *, size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_EZTLS_H_ */
