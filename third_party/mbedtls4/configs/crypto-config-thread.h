/**
 * \file crypto-config-thread.h
 *
 * \brief Minimal crypto configuration for using TLS as part of Thread
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

/**
 * Minimal crypto configuration for using TLS as part of Thread
 * http://threadgroup.org/
 *
 * Distinguishing features:
 * - no RSA or classic DH, fully based on ECC
 * - no X.509
 * - support for experimental EC J-PAKE key exchange
 * - support for PBKDF2-AES-CMAC-PRF-128 password-hashing or key-stretching
 *   algorithm.
 *
 * To be used in conjunction with configs/config-thread.h.
 * See README.txt for usage instructions.
 */

#ifndef PSA_CRYPTO_CONFIG_H
#define PSA_CRYPTO_CONFIG_H

#define PSA_WANT_ALG_CCM                        1
#define PSA_WANT_ALG_ECB_NO_PADDING             1
#define PSA_WANT_ALG_HMAC                       1
#define PSA_WANT_ALG_JPAKE                      1
#define PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128    1
#define PSA_WANT_ALG_SHA_256                    1
#define PSA_WANT_ALG_TLS12_PRF                  1
#define PSA_WANT_ALG_TLS12_ECJPAKE_TO_PMS       1
#define PSA_WANT_ECC_SECP_R1_256                1

#define PSA_WANT_KEY_TYPE_AES                   1
#define PSA_WANT_KEY_TYPE_DERIVE                1
#define PSA_WANT_KEY_TYPE_HMAC                  1
#define PSA_WANT_KEY_TYPE_RAW_DATA              1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC    1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT   1
#define PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE 1

#define MBEDTLS_PSA_CRYPTO_C

/* System support */
#define MBEDTLS_HAVE_ASM
#define MBEDTLS_HAVE_TIME //Only used by test programs

#define MBEDTLS_AES_ROM_TABLES
#define MBEDTLS_ECP_NIST_OPTIM

#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_HMAC_DRBG_C
#define MBEDTLS_MD_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
#define MBEDTLS_PSA_BUILTIN_GET_ENTROPY

/* Save RAM at the expense of ROM */
#define MBEDTLS_AES_ROM_TABLES

/* Save RAM by adjusting to our exact needs */
#define MBEDTLS_MPI_MAX_SIZE              32 // 256-bit EC curve = 32 bytes
#endif /* PSA_CRYPTO_CONFIG_H */
