/**
 * \file config-ccm-psk-dtls1_2.h
 *
 * \brief Small configuration for DTLS 1.2 with PSK and AES-CCM ciphersuites
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */
/*
 * Minimal configuration for DTLS 1.2 with PSK and AES-CCM ciphersuites
 *
 * Distinguishing features:
 * - Optimized for small code size, low bandwidth (on an unreliable transport),
 *   and low RAM usage.
 * - No asymmetric cryptography (no certificates, no Diffie-Hellman key
 *   exchange).
 * - Fully modern and secure (provided the pre-shared keys are generated and
 *   stored securely).
 * - Very low record overhead with CCM-8.
 * - Includes several optional DTLS features typically used in IoT.
 *
 * See README.txt for usage instructions.
 */

/* Mbed TLS modules */
#define MBEDTLS_NET_C
#define MBEDTLS_SSL_CLI_C
#define MBEDTLS_SSL_COOKIE_C
#define MBEDTLS_SSL_SRV_C
#define MBEDTLS_SSL_TLS_C
#define MBEDTLS_TIMING_C //Only used by test programs

/* TLS protocol feature support */
#define MBEDTLS_KEY_EXCHANGE_PSK_ENABLED
#define MBEDTLS_SSL_PROTO_TLS1_2
#define MBEDTLS_SSL_PROTO_DTLS
#define MBEDTLS_SSL_DTLS_ANTI_REPLAY
#define MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE
#define MBEDTLS_SSL_DTLS_CONNECTION_ID
#define MBEDTLS_SSL_DTLS_HELLO_VERIFY
#define MBEDTLS_SSL_MAX_FRAGMENT_LENGTH

/*
 * Use only CCM_8 ciphersuites, and
 * save ROM and a few bytes of RAM by specifying our own ciphersuite list
 */
#define MBEDTLS_SSL_CIPHERSUITES                        \
    MBEDTLS_TLS_PSK_WITH_AES_256_CCM_8,             \
    MBEDTLS_TLS_PSK_WITH_AES_128_CCM_8

/*
 * Save RAM at the expense of interoperability: do this only if you control
 * both ends of the connection!  (See comments in "mbedtls/ssl.h".)
 * The optimal size here depends on the typical size of records.
 */
#define MBEDTLS_SSL_IN_CONTENT_LEN              256
#define MBEDTLS_SSL_OUT_CONTENT_LEN             256

/* Save some RAM by adjusting to your exact needs */
#define MBEDTLS_PSK_MAX_LEN    16 /* 128-bits keys are generally enough */

/* Error messages and TLS debugging traces
 * (huge code size increase, needed for tests/ssl-opt.sh) */
//#define MBEDTLS_DEBUG_C
//#define MBEDTLS_ERROR_C
