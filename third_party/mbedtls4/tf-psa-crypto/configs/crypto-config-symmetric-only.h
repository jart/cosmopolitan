/**
 * \file crypto-config-symmetric-only.h
 *
 * \brief Crypto configuration without any asymmetric cryptography.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

/**
 * To be used in conjunction with configs/config-symmetric-only.h. */

#ifndef PSA_CRYPTO_CONFIG_H
#define PSA_CRYPTO_CONFIG_H

#define PSA_WANT_ALG_CBC_NO_PADDING             1
#define PSA_WANT_ALG_CBC_PKCS7                  1
#define PSA_WANT_ALG_CCM                        1
#define PSA_WANT_ALG_CCM_STAR_NO_TAG            1
#define PSA_WANT_ALG_CFB                        1
#define PSA_WANT_ALG_CHACHA20_POLY1305          1
#define PSA_WANT_ALG_CMAC                       1
#define PSA_WANT_ALG_CTR                        1
#define PSA_WANT_ALG_ECB_NO_PADDING             1
#define PSA_WANT_ALG_GCM                        1
#define PSA_WANT_ALG_HKDF                       1
#define PSA_WANT_ALG_HKDF_EXTRACT               1
#define PSA_WANT_ALG_HKDF_EXPAND                1
#define PSA_WANT_ALG_HMAC                       1
#define PSA_WANT_ALG_MD5                        1
#define PSA_WANT_ALG_OFB                        1
#define PSA_WANT_ALG_RIPEMD160                  1
#define PSA_WANT_ALG_SHA_1                      1
#define PSA_WANT_ALG_STREAM_CIPHER              1
#define PSA_WANT_ALG_SHA_224                    1
#define PSA_WANT_ALG_SHA_256                    1
#define PSA_WANT_ALG_SHA_384                    1
#define PSA_WANT_ALG_SHA_512                    1
#define PSA_WANT_ALG_SHA3_224                   1
#define PSA_WANT_ALG_SHA3_256                   1
#define PSA_WANT_ALG_SHA3_384                   1
#define PSA_WANT_ALG_SHA3_512                   1
#define PSA_WANT_ALG_TLS12_ECJPAKE_TO_PMS       1
#define PSA_WANT_ALG_TLS12_PRF                  1
#define PSA_WANT_ALG_TLS12_PSK_TO_MS            1

/* XTS is not yet supported via the PSA API in Mbed TLS. */
//#define PSA_WANT_ALG_XTS                        1

#define PSA_WANT_KEY_TYPE_AES                   1
#define PSA_WANT_KEY_TYPE_ARIA                  1
#define PSA_WANT_KEY_TYPE_CAMELLIA              1
#define PSA_WANT_KEY_TYPE_CHACHA20              1
#define PSA_WANT_KEY_TYPE_HMAC                  1

#define MBEDTLS_SELF_TEST

#define MBEDTLS_PSA_CRYPTO_C

/* System support */
//#define MBEDTLS_HAVE_ASM
#define MBEDTLS_HAVE_TIME
#define MBEDTLS_HAVE_TIME_DATE

#define MBEDTLS_FS_IO
#define MBEDTLS_ENTROPY_NV_SEED

/* Mbed TLS modules */
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BASE64_C
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_ERROR_C
#define MBEDTLS_HMAC_DRBG_C
#define MBEDTLS_NIST_KW_C
#define MBEDTLS_PEM_PARSE_C
#define MBEDTLS_PEM_WRITE_C
#define MBEDTLS_PKCS5_C
#define MBEDTLS_PLATFORM_C
#define MBEDTLS_PSA_BUILTIN_GET_ENTROPY
#define MBEDTLS_PSA_CRYPTO_STORAGE_C
#define MBEDTLS_PSA_ITS_FILE_C

//#define MBEDTLS_THREADING_C

#endif /* PSA_CRYPTO_CONFIG_H */
