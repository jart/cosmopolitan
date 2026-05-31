/*
 * PSA Crypto configuration base for PSA test driver libraries. It includes:
 * . the minimum set of modules needed by the PSA core.
 * . the Mbed TLS configuration options that may need to be additionally
 *   enabled for the purpose of a specific test.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef PSA_CRYPTO_CONFIG_H
#define PSA_CRYPTO_CONFIG_H

#define MBEDTLS_PSA_CRYPTO_C

/* PSA core mandatory configuration options */
#define MBEDTLS_CIPHER_C
#define MBEDTLS_AES_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_PSA_BUILTIN_ALG_SHA_256 1
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_PSA_CRYPTO_RNG_HASH PSA_ALG_SHA_256
#define MBEDTLS_PSA_BUILTIN_GET_ENTROPY

/*
 * Configuration options that may need to be additionally enabled for the
 * purpose of a specific set of tests.
 */
//#define MBEDTLS_PEM_PARSE_C
//#define MBEDTLS_BASE64_C
//#define MBEDTLS_THREADING_C
//#define MBEDTLS_THREADING_PTHREAD

#endif /* PSA_CRYPTO_CONFIG_H */
