#ifndef COSMOPOLITAN_THIRD_PARTY_MBEDTLS_ENDIAN_H_
#define COSMOPOLITAN_THIRD_PARTY_MBEDTLS_ENDIAN_H_
#include "libc/serialize.h"

#define GET_UINT32_BE(n, b, i) (n) = READ32BE((b) + (i))
#define PUT_UINT32_BE(n, b, i) WRITE32BE((b) + (i), n)
#define GET_UINT64_BE(n, b, i) (n) = READ64BE((b) + (i))
#define PUT_UINT64_BE(n, b, i) WRITE64BE((b) + (i), n)

#endif /* COSMOPOLITAN_THIRD_PARTY_MBEDTLS_ENDIAN_H_ */
