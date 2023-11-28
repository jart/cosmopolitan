#ifndef COSMOPOLITAN_TOOL_BUILD_LIB_EZTLS_H_
#define COSMOPOLITAN_TOOL_BUILD_LIB_EZTLS_H_
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/ssl.h"
#include "third_party/mbedtls/x509_crt.h"
COSMOPOLITAN_C_START_

struct EzTlsBio {
  int fd, c;
  unsigned a, b;
  unsigned char t[4000];
  unsigned char u[1430];
};

extern _Thread_local struct EzTlsBio ezbio;
extern _Thread_local mbedtls_ssl_config ezconf;
extern _Thread_local mbedtls_ssl_context ezssl;
extern _Thread_local mbedtls_ctr_drbg_context ezrng;

void EzFd(int);
void EzSanity(void);
void EzDestroy(void);
void EzHandshake(void);
int EzHandshake2(void);
void EzSetup(char[32]);
void EzInitialize(void);
void EzTlsDie(const char *, int);
int EzTlsFlush(struct EzTlsBio *, const unsigned char *, size_t);

/*
 * openssl s_client -connect 127.0.0.1:31337 \
 *   -psk $(hex <~/.runit.psk)               \
 *   -psk_identity runit
 */
forceinline void SetupPresharedKeySsl(int endpoint, char psk[32]) {
  EzInitialize();
  mbedtls_ssl_config_defaults(&ezconf, endpoint, MBEDTLS_SSL_TRANSPORT_STREAM,
                              MBEDTLS_SSL_PRESET_SUITEC);
  EzSetup(psk);
}

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_TOOL_BUILD_LIB_EZTLS_H_ */
