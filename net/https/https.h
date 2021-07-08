#ifndef COSMOPOLITAN_NET_HTTPS_HTTPS_H_
#define COSMOPOLITAN_NET_HTTPS_HTTPS_H_
#include "libc/time/struct/tm.h"
#include "third_party/mbedtls/x509_crt.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

char *DescribeSslVerifyFailure(int);
mbedtls_x509_crt *GetSslRoots(void);
void FormatSslTime(char[restrict hasatleast 16], struct tm *);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_NET_HTTPS_HTTPS_H_ */
