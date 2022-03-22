#ifndef COSMOPOLITAN_THIRD_PARTY_MBEDTLS_IANA_H_
#define COSMOPOLITAN_THIRD_PARTY_MBEDTLS_IANA_H_
#include "third_party/mbedtls/ssl.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

bool IsCipherSuiteGood(uint16_t);
const char *GetCipherSuiteName(uint16_t);
const char *GetAlertDescription(unsigned char);
dontdiscard char *FormatSslClientCiphers(const mbedtls_ssl_context *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_MBEDTLS_IANA_H_ */
