#ifndef COSMOPOLITAN_THIRD_PARTY_MBEDTLS_HKDF_H_
#define COSMOPOLITAN_THIRD_PARTY_MBEDTLS_HKDF_H_
#include "third_party/mbedtls/config.h"
#include "third_party/mbedtls/md.h"
#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

/* clang-format off */
#define MBEDTLS_ERR_HKDF_BAD_INPUT_DATA  -0x5F80  /*< Bad input parameters to function. */
/* clang-format on */

int mbedtls_hkdf(const mbedtls_md_info_t *, const unsigned char *, size_t,
                 const unsigned char *, size_t, const unsigned char *, size_t,
                 unsigned char *, size_t);
int mbedtls_hkdf_extract(const mbedtls_md_info_t *, const unsigned char *,
                         size_t, const unsigned char *, size_t,
                         unsigned char *);
int mbedtls_hkdf_expand(const mbedtls_md_info_t *, const unsigned char *,
                        size_t, const unsigned char *, size_t, unsigned char *,
                        size_t);

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_THIRD_PARTY_MBEDTLS_HKDF_H_ */
