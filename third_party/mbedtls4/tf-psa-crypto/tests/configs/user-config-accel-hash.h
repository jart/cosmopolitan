/* Accelerate hashes and XOF (extendable output functions).
 *
 * We accelerate XOF together with hashes because all the XOF functions
 * we support are based on the same construction as a hash, and it would
 * be quite unusual to accelerate one without the other.
 */

#define MBEDTLS_PSA_ACCEL_ALG_MD5
#define MBEDTLS_PSA_ACCEL_ALG_RIPEMD160
#define MBEDTLS_PSA_ACCEL_ALG_SHA_1
#define MBEDTLS_PSA_ACCEL_ALG_SHA_224
#define MBEDTLS_PSA_ACCEL_ALG_SHA_256
#define MBEDTLS_PSA_ACCEL_ALG_SHA_384
#define MBEDTLS_PSA_ACCEL_ALG_SHA_512
#define MBEDTLS_PSA_ACCEL_ALG_SHA3_224
#define MBEDTLS_PSA_ACCEL_ALG_SHA3_256
#define MBEDTLS_PSA_ACCEL_ALG_SHA3_384
#define MBEDTLS_PSA_ACCEL_ALG_SHA3_512

#define MBEDTLS_PSA_ACCEL_ALG_SHAKE128
#define MBEDTLS_PSA_ACCEL_ALG_SHAKE256

#include "user-config-test-driver-extension.h"
