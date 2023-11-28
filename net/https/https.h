#ifndef COSMOPOLITAN_NET_HTTPS_HTTPS_H_
#define COSMOPOLITAN_NET_HTTPS_HTTPS_H_
#include "libc/time/struct/tm.h"
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/ecp.h"
#include "third_party/mbedtls/md.h"
#include "third_party/mbedtls/pk.h"
#include "third_party/mbedtls/ssl_ciphersuites.h"
#include "third_party/mbedtls/x509_crt.h"
COSMOPOLITAN_C_START_

struct Cert {
  mbedtls_x509_crt *cert;
  mbedtls_pk_context *key;
};

char *GetTlsError(int);
char *DescribeSslVerifyFailure(int);
mbedtls_x509_crt *GetSslRoots(void);
void InitializeRng(mbedtls_ctr_drbg_context *);
int GetEntropy(void *, unsigned char *, size_t);
void FormatSslTime(char[restrict hasatleast 16], struct tm *);
void ChooseCertificateLifetime(char[16], char[16]);
void LogCertificate(const char *, mbedtls_x509_crt *);
bool IsSelfSigned(mbedtls_x509_crt *);
char *FormatX509Name(const mbedtls_x509_name *);
void TlsDie(const char *, int) wontreturn;
bool ChainCertificate(mbedtls_x509_crt *, mbedtls_x509_crt *);
bool CertHasIp(const mbedtls_x509_crt *, uint32_t);
bool CertHasHost(const mbedtls_x509_crt *, const void *, size_t);
bool IsServerCert(const struct Cert *, mbedtls_pk_type_t);
void TlsDebug(void *, int, const char *, int, const char *);

int GenerateHardRandom(void *, unsigned char *, size_t);
void GenerateCertificateSerial(mbedtls_x509write_cert *);
mbedtls_pk_context *InitializeKey(struct Cert *, mbedtls_x509write_cert *,
                                  mbedtls_md_type_t, int);
struct Cert FinishCertificate(struct Cert *, mbedtls_x509write_cert *,
                              mbedtls_pk_context *);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_NET_HTTPS_HTTPS_H_ */
