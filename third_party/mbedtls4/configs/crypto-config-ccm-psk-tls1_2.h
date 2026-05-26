/**
 * \file crypto-config-ccm-psk-tls1_2.h
 *
 * \brief Minimal crypto configuration for TLS 1.2 with
 * PSK and AES-CCM ciphersuites
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

/**
 * To be used in conjunction with configs/config-ccm-psk-tls1_2.h
 * or configs/config-ccm-psk-dtls1_2.h. */

#ifndef PSA_CRYPTO_CONFIG_H
#define PSA_CRYPTO_CONFIG_H

#define PSA_WANT_ALG_CCM                        1
#define PSA_WANT_ALG_SHA_256                    1
#define PSA_WANT_ALG_TLS12_PRF                  1
#define PSA_WANT_ALG_TLS12_PSK_TO_MS            1

#define PSA_WANT_KEY_TYPE_AES                   1

#define MBEDTLS_PSA_CRYPTO_C

/* System support */
/* Optionally used in Hello messages. Needed for DTLS testing. */
#define MBEDTLS_HAVE_TIME
/* Other MBEDTLS_HAVE_XXX flags irrelevant for this configuration */

#define MBEDTLS_CTR_DRBG_C
#define MBEDTLS_PSA_BUILTIN_GET_ENTROPY

/* Save RAM at the expense of ROM */
#define MBEDTLS_AES_ROM_TABLES

#endif /* PSA_CRYPTO_CONFIG_H */
