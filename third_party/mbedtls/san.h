#ifndef COSMOPOLITAN_THIRD_PARTY_MBEDTLS_SAN_H_
#define COSMOPOLITAN_THIRD_PARTY_MBEDTLS_SAN_H_
#include "third_party/mbedtls/x509_crt.h"
COSMOPOLITAN_C_START_

/**
 * Internal tag for an IPv6 subject alternative name.
 *
 * This is not an ASN.1 SAN type; san.c encodes it as
 * MBEDTLS_X509_SAN_IP_ADDRESS. It exists because existing callers set only
 * `tag` and `ip4` on freshly reallocated entries, so an `iplen` field would
 * be left indeterminate. Keeping the length in the tag avoids that.
 */
#define MBEDTLS_X509_SAN_IP6_ADDRESS_INTERNAL 9

struct mbedtls_san {
  int tag;
  union {
    const char *val;
    uint32_t ip4;
    uint8_t ip6[16];
  };
};

int mbedtls_x509write_crt_set_subject_alternative_name(
    mbedtls_x509write_cert *, const struct mbedtls_san *, size_t);

COSMOPOLITAN_C_END_
#endif /* COSMOPOLITAN_THIRD_PARTY_MBEDTLS_SAN_H_ */
