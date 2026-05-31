/**
 * \file configs/crypto-config-ccm-aes-sha256.h
 *
 * \brief PSA crypto configuration with only symmetric cryptography: CCM-AES,
 *        SHA-256 and key derivation (uses HMAC).
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef PSA_CRYPTO_CONFIG_H
#define PSA_CRYPTO_CONFIG_H

#define PSA_WANT_ALG_CCM 1
#define PSA_WANT_ALG_SHA_256 1
#define PSA_WANT_ALG_TLS12_PRF 1
#define PSA_WANT_ALG_TLS12_PSK_TO_MS 1
#define PSA_WANT_KEY_TYPE_DERIVE 1
#define PSA_WANT_KEY_TYPE_AES 1
#define PSA_WANT_KEY_TYPE_RAW_DATA 1


#define MBEDTLS_PSA_CRYPTO_C
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_PSA_BUILTIN_GET_ENTROPY

#endif /* PSA_CRYPTO_CONFIG_H */
