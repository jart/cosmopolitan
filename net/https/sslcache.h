#ifndef COSMOPOLITAN_NET_HTTPS_SSLCACHE_H_
#define COSMOPOLITAN_NET_HTTPS_SSLCACHE_H_
#include "third_party/mbedtls/ssl.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

struct SslCache {
  size_t size;
  int lifetime;
  uint32_t mask;
  struct SslCacheEntry {
    int64_t time;
    volatile uint64_t tick;
    volatile int pid;
    uint32_t hash;
    unsigned certlen;
    unsigned ticketlen;
    mbedtls_ssl_session session;
    uint8_t cert[1500];
    uint8_t ticket[500];
  } p[];
};

struct SslCache *CreateSslCache(const char *, size_t, int);
void FreeSslCache(struct SslCache *);
int UncacheSslSession(void *, mbedtls_ssl_session *);
int CacheSslSession(void *, const mbedtls_ssl_session *);
char *GetSslCacheFile(void);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_NET_HTTPS_SSLCACHE_H_ */
