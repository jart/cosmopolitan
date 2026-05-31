/* -*-c-*-
 *  Query Mbed TLS compile time configurations from mbedtls_config.h
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "mbedtls/build_info.h"

#include "query_config.h"

#include "mbedtls/platform.h"
#include <string.h>

/* Work around https://github.com/Mbed-TLS/TF-PSA-Crypto/issues/393 */
#if defined(MBEDTLS_HAVE_TIME)
#include <mbedtls/platform_time.h>
#endif

/* *INDENT-OFF* */
#include <mbedtls/build_info.h>
#include <mbedtls/debug.h>
#include <mbedtls/error.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/oid.h>
#include <mbedtls/pkcs7.h>
#include <mbedtls/ssl.h>
#include <mbedtls/ssl_cache.h>
#include <mbedtls/ssl_ciphersuites.h>
#include <mbedtls/ssl_cookie.h>
#include <mbedtls/ssl_ticket.h>
#include <mbedtls/timing.h>
#include <mbedtls/version.h>
#include <mbedtls/x509.h>
#include <mbedtls/x509_crl.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/x509_csr.h>
#include <mbedtls/asn1.h>
#include <mbedtls/asn1write.h>
#include <mbedtls/base64.h>
#include <mbedtls/compat-3-crypto.h>
#include <mbedtls/constant_time.h>
#include <mbedtls/lms.h>
#include <mbedtls/md.h>
#include <mbedtls/memory_buffer_alloc.h>
#include <mbedtls/nist_kw.h>
#include <mbedtls/pem.h>
#include <mbedtls/pk.h>
#include <mbedtls/platform.h>
#include <mbedtls/platform_util.h>
#include <mbedtls/psa_util.h>
#include <mbedtls/threading.h>
#include <psa/crypto.h>
#include <psa/crypto_compat.h>
#include <psa/crypto_driver_common.h>
#include <psa/crypto_driver_contexts_composites.h>
#include <psa/crypto_driver_contexts_key_derivation.h>
#include <psa/crypto_driver_contexts_primitives.h>
#include <psa/crypto_driver_random.h>
#include <psa/crypto_extra.h>
#include <psa/crypto_sizes.h>
#include <psa/crypto_types.h>
#include <psa/crypto_values.h>
#include <tf-psa-crypto/build_info.h>
#include <tf-psa-crypto/version.h>
#include <mbedtls/private/pk_private.h>
#include <mbedtls/private_access.h>
#include <mbedtls/private/aes.h>
#include <mbedtls/private/aria.h>
#include <mbedtls/private/bignum.h>
#include <mbedtls/private/block_cipher.h>
#include <mbedtls/private/camellia.h>
#include <mbedtls/private/ccm.h>
#include <mbedtls/private/chacha20.h>
#include <mbedtls/private/chachapoly.h>
#include <mbedtls/private/cipher.h>
#include <mbedtls/private/cmac.h>
#include <mbedtls/private/crypto_builtin_composites.h>
#include <mbedtls/private/crypto_builtin_key_derivation.h>
#include <mbedtls/private/crypto_builtin_primitives.h>
#include <mbedtls/private/ctr_drbg.h>
#include <mbedtls/private/ecdsa.h>
#include <mbedtls/private/ecjpake.h>
#include <mbedtls/private/ecp.h>
#include <mbedtls/private/entropy.h>
#include <mbedtls/private/error_common.h>
#include <mbedtls/private/gcm.h>
#include <mbedtls/private/hmac_drbg.h>
#include <mbedtls/private/md5.h>
#include <mbedtls/private/pkcs5.h>
#include <mbedtls/private/poly1305.h>
#include <mbedtls/private/ripemd160.h>
#include <mbedtls/private/rsa.h>
#include <mbedtls/private/sha1.h>
#include <mbedtls/private/sha256.h>
#include <mbedtls/private/sha3.h>
#include <mbedtls/private/sha512.h>

/* *INDENT-ON* */

/*
 * Helper macros to convert a macro or its expansion into a string
 * WARNING: This does not work for expanding function-like macros. However,
 * Mbed TLS does not currently have configuration options used in this fashion.
 */
#define MACRO_EXPANSION_TO_STR(macro)   MACRO_NAME_TO_STR(macro)
#define MACRO_NAME_TO_STR(macro)                                        \
    mbedtls_printf("%s", strlen( #macro "") > 0 ? #macro "\n" : "")

#define STRINGIFY(macro)  #macro
#define OUTPUT_MACRO_NAME_VALUE(macro) mbedtls_printf( #macro "%s\n",   \
                                                       (STRINGIFY(macro) "")[0] != 0 ? "=" STRINGIFY( \
                                                           macro) : "")

#if defined(_MSC_VER)
/*
 * Visual Studio throws the warning 4003 because many Mbed TLS feature macros
 * are defined empty. This means that from the preprocessor's point of view
 * the macro MBEDTLS_EXPANSION_TO_STR is being invoked without arguments as
 * some macros expand to nothing. We suppress that specific warning to get a
 * clean build and to ensure that tests treating warnings as errors do not
 * fail.
 */
#pragma warning(push)
#pragma warning(disable:4003)
#endif /* _MSC_VER */

int query_config(const char *config)
{
    #if defined(MBEDTLS_CONFIG_VERSION)
    if( strcmp( "MBEDTLS_CONFIG_VERSION", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_CONFIG_VERSION );
        return( 0 );
    }
#endif /* MBEDTLS_CONFIG_VERSION */

#if defined(MBEDTLS_NET_C)
    if( strcmp( "MBEDTLS_NET_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_NET_C );
        return( 0 );
    }
#endif /* MBEDTLS_NET_C */

#if defined(MBEDTLS_TIMING_ALT)
    if( strcmp( "MBEDTLS_TIMING_ALT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_TIMING_ALT );
        return( 0 );
    }
#endif /* MBEDTLS_TIMING_ALT */

#if defined(MBEDTLS_TIMING_C)
    if( strcmp( "MBEDTLS_TIMING_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_TIMING_C );
        return( 0 );
    }
#endif /* MBEDTLS_TIMING_C */

#if defined(MBEDTLS_ERROR_C)
    if( strcmp( "MBEDTLS_ERROR_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_ERROR_C );
        return( 0 );
    }
#endif /* MBEDTLS_ERROR_C */

#if defined(MBEDTLS_ERROR_STRERROR_DUMMY)
    if( strcmp( "MBEDTLS_ERROR_STRERROR_DUMMY", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_ERROR_STRERROR_DUMMY );
        return( 0 );
    }
#endif /* MBEDTLS_ERROR_STRERROR_DUMMY */

#if defined(MBEDTLS_VERSION_C)
    if( strcmp( "MBEDTLS_VERSION_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_VERSION_C );
        return( 0 );
    }
#endif /* MBEDTLS_VERSION_C */

#if defined(MBEDTLS_VERSION_FEATURES)
    if( strcmp( "MBEDTLS_VERSION_FEATURES", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_VERSION_FEATURES );
        return( 0 );
    }
#endif /* MBEDTLS_VERSION_FEATURES */

#if defined(MBEDTLS_CONFIG_FILE)
    if( strcmp( "MBEDTLS_CONFIG_FILE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_CONFIG_FILE );
        return( 0 );
    }
#endif /* MBEDTLS_CONFIG_FILE */

#if defined(MBEDTLS_USER_CONFIG_FILE)
    if( strcmp( "MBEDTLS_USER_CONFIG_FILE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_USER_CONFIG_FILE );
        return( 0 );
    }
#endif /* MBEDTLS_USER_CONFIG_FILE */

#if defined(MBEDTLS_SSL_NULL_CIPHERSUITES)
    if( strcmp( "MBEDTLS_SSL_NULL_CIPHERSUITES", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_NULL_CIPHERSUITES );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_NULL_CIPHERSUITES */

#if defined(MBEDTLS_DEBUG_C)
    if( strcmp( "MBEDTLS_DEBUG_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_DEBUG_C );
        return( 0 );
    }
#endif /* MBEDTLS_DEBUG_C */

#if defined(MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED)
    if( strcmp( "MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED );
        return( 0 );
    }
#endif /* MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED */

#if defined(MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED)
    if( strcmp( "MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED );
        return( 0 );
    }
#endif /* MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED */

#if defined(MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED)
    if( strcmp( "MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED );
        return( 0 );
    }
#endif /* MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED */

#if defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
    if( strcmp( "MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED );
        return( 0 );
    }
#endif /* MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED */

#if defined(MBEDTLS_KEY_EXCHANGE_PSK_ENABLED)
    if( strcmp( "MBEDTLS_KEY_EXCHANGE_PSK_ENABLED", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_KEY_EXCHANGE_PSK_ENABLED );
        return( 0 );
    }
#endif /* MBEDTLS_KEY_EXCHANGE_PSK_ENABLED */

#if defined(MBEDTLS_SSL_ALL_ALERT_MESSAGES)
    if( strcmp( "MBEDTLS_SSL_ALL_ALERT_MESSAGES", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_ALL_ALERT_MESSAGES );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_ALL_ALERT_MESSAGES */

#if defined(MBEDTLS_SSL_ALPN)
    if( strcmp( "MBEDTLS_SSL_ALPN", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_ALPN );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_ALPN */

#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
    if( strcmp( "MBEDTLS_SSL_ASYNC_PRIVATE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_ASYNC_PRIVATE );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_ASYNC_PRIVATE */

#if defined(MBEDTLS_SSL_CACHE_C)
    if( strcmp( "MBEDTLS_SSL_CACHE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_CACHE_C );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_CACHE_C */

#if defined(MBEDTLS_SSL_CLI_C)
    if( strcmp( "MBEDTLS_SSL_CLI_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_CLI_C );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_CLI_C */

#if defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION)
    if( strcmp( "MBEDTLS_SSL_CONTEXT_SERIALIZATION", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_CONTEXT_SERIALIZATION );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_CONTEXT_SERIALIZATION */

#if defined(MBEDTLS_SSL_COOKIE_C)
    if( strcmp( "MBEDTLS_SSL_COOKIE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_COOKIE_C );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_COOKIE_C */

#if defined(MBEDTLS_SSL_DEBUG_ALL)
    if( strcmp( "MBEDTLS_SSL_DEBUG_ALL", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_DEBUG_ALL );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_DEBUG_ALL */

#if defined(MBEDTLS_SSL_DTLS_ANTI_REPLAY)
    if( strcmp( "MBEDTLS_SSL_DTLS_ANTI_REPLAY", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_DTLS_ANTI_REPLAY );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_DTLS_ANTI_REPLAY */

#if defined(MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE)
    if( strcmp( "MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE */

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    if( strcmp( "MBEDTLS_SSL_DTLS_CONNECTION_ID", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_DTLS_CONNECTION_ID );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

#if defined(MBEDTLS_SSL_DTLS_HELLO_VERIFY)
    if( strcmp( "MBEDTLS_SSL_DTLS_HELLO_VERIFY", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_DTLS_HELLO_VERIFY );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_DTLS_HELLO_VERIFY */

#if defined(MBEDTLS_SSL_DTLS_SRTP)
    if( strcmp( "MBEDTLS_SSL_DTLS_SRTP", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_DTLS_SRTP );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_DTLS_SRTP */

#if defined(MBEDTLS_SSL_EARLY_DATA)
    if( strcmp( "MBEDTLS_SSL_EARLY_DATA", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_EARLY_DATA );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_EARLY_DATA */

#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
    if( strcmp( "MBEDTLS_SSL_ENCRYPT_THEN_MAC", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_ENCRYPT_THEN_MAC );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_ENCRYPT_THEN_MAC */

#if defined(MBEDTLS_SSL_EXTENDED_MASTER_SECRET)
    if( strcmp( "MBEDTLS_SSL_EXTENDED_MASTER_SECRET", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_EXTENDED_MASTER_SECRET );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_EXTENDED_MASTER_SECRET */

#if defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
    if( strcmp( "MBEDTLS_SSL_KEEP_PEER_CERTIFICATE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_KEEP_PEER_CERTIFICATE );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_KEEP_PEER_CERTIFICATE */

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    if( strcmp( "MBEDTLS_SSL_MAX_FRAGMENT_LENGTH", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_MAX_FRAGMENT_LENGTH );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_MAX_FRAGMENT_LENGTH */

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if( strcmp( "MBEDTLS_SSL_PROTO_DTLS", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_PROTO_DTLS );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_PROTO_DTLS */

#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
    if( strcmp( "MBEDTLS_SSL_PROTO_TLS1_2", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_PROTO_TLS1_2 );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    if( strcmp( "MBEDTLS_SSL_PROTO_TLS1_3", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_PROTO_TLS1_3 );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

#if defined(MBEDTLS_SSL_RECORD_SIZE_LIMIT)
    if( strcmp( "MBEDTLS_SSL_RECORD_SIZE_LIMIT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_RECORD_SIZE_LIMIT );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_RECORD_SIZE_LIMIT */

#if defined(MBEDTLS_SSL_KEYING_MATERIAL_EXPORT)
    if( strcmp( "MBEDTLS_SSL_KEYING_MATERIAL_EXPORT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_KEYING_MATERIAL_EXPORT );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_KEYING_MATERIAL_EXPORT */

#if defined(MBEDTLS_SSL_RENEGOTIATION)
    if( strcmp( "MBEDTLS_SSL_RENEGOTIATION", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_RENEGOTIATION );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_RENEGOTIATION */

#if defined(MBEDTLS_SSL_SERVER_NAME_INDICATION)
    if( strcmp( "MBEDTLS_SSL_SERVER_NAME_INDICATION", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_SERVER_NAME_INDICATION );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_SERVER_NAME_INDICATION */

#if defined(MBEDTLS_SSL_SESSION_TICKETS)
    if( strcmp( "MBEDTLS_SSL_SESSION_TICKETS", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_SESSION_TICKETS );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_SESSION_TICKETS */

#if defined(MBEDTLS_SSL_SRV_C)
    if( strcmp( "MBEDTLS_SSL_SRV_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_SRV_C );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_SRV_C */

#if defined(MBEDTLS_SSL_TICKET_C)
    if( strcmp( "MBEDTLS_SSL_TICKET_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_TICKET_C );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_TICKET_C */

#if defined(MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE)
    if( strcmp( "MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE */

#if defined(MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED)
    if( strcmp( "MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED */

#if defined(MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED)
    if( strcmp( "MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED */

#if defined(MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED)
    if( strcmp( "MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED */

#if defined(MBEDTLS_SSL_TLS_C)
    if( strcmp( "MBEDTLS_SSL_TLS_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_TLS_C );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_TLS_C */

#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
    if( strcmp( "MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH */

#if defined(MBEDTLS_PSK_MAX_LEN)
    if( strcmp( "MBEDTLS_PSK_MAX_LEN", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSK_MAX_LEN );
        return( 0 );
    }
#endif /* MBEDTLS_PSK_MAX_LEN */

#if defined(MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES)
    if( strcmp( "MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES */

#if defined(MBEDTLS_SSL_CACHE_DEFAULT_TIMEOUT)
    if( strcmp( "MBEDTLS_SSL_CACHE_DEFAULT_TIMEOUT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_CACHE_DEFAULT_TIMEOUT );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_CACHE_DEFAULT_TIMEOUT */

#if defined(MBEDTLS_SSL_CID_IN_LEN_MAX)
    if( strcmp( "MBEDTLS_SSL_CID_IN_LEN_MAX", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_CID_IN_LEN_MAX );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_CID_IN_LEN_MAX */

#if defined(MBEDTLS_SSL_CID_OUT_LEN_MAX)
    if( strcmp( "MBEDTLS_SSL_CID_OUT_LEN_MAX", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_CID_OUT_LEN_MAX );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_CID_OUT_LEN_MAX */

#if defined(MBEDTLS_SSL_CID_TLS1_3_PADDING_GRANULARITY)
    if( strcmp( "MBEDTLS_SSL_CID_TLS1_3_PADDING_GRANULARITY", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_CID_TLS1_3_PADDING_GRANULARITY );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_CID_TLS1_3_PADDING_GRANULARITY */

#if defined(MBEDTLS_SSL_COOKIE_TIMEOUT)
    if( strcmp( "MBEDTLS_SSL_COOKIE_TIMEOUT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_COOKIE_TIMEOUT );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_COOKIE_TIMEOUT */

#if defined(MBEDTLS_SSL_DTLS_MAX_BUFFERING)
    if( strcmp( "MBEDTLS_SSL_DTLS_MAX_BUFFERING", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_DTLS_MAX_BUFFERING );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_DTLS_MAX_BUFFERING */

#if defined(MBEDTLS_SSL_IN_CONTENT_LEN)
    if( strcmp( "MBEDTLS_SSL_IN_CONTENT_LEN", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_IN_CONTENT_LEN );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_IN_CONTENT_LEN */

#if defined(MBEDTLS_SSL_MAX_EARLY_DATA_SIZE)
    if( strcmp( "MBEDTLS_SSL_MAX_EARLY_DATA_SIZE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_MAX_EARLY_DATA_SIZE );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_MAX_EARLY_DATA_SIZE */

#if defined(MBEDTLS_SSL_OUT_CONTENT_LEN)
    if( strcmp( "MBEDTLS_SSL_OUT_CONTENT_LEN", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_OUT_CONTENT_LEN );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_OUT_CONTENT_LEN */

#if defined(MBEDTLS_SSL_TLS1_3_DEFAULT_NEW_SESSION_TICKETS)
    if( strcmp( "MBEDTLS_SSL_TLS1_3_DEFAULT_NEW_SESSION_TICKETS", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_TLS1_3_DEFAULT_NEW_SESSION_TICKETS );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_TLS1_3_DEFAULT_NEW_SESSION_TICKETS */

#if defined(MBEDTLS_SSL_TLS1_3_TICKET_AGE_TOLERANCE)
    if( strcmp( "MBEDTLS_SSL_TLS1_3_TICKET_AGE_TOLERANCE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_TLS1_3_TICKET_AGE_TOLERANCE );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_TLS1_3_TICKET_AGE_TOLERANCE */

#if defined(MBEDTLS_SSL_TLS1_3_TICKET_NONCE_LENGTH)
    if( strcmp( "MBEDTLS_SSL_TLS1_3_TICKET_NONCE_LENGTH", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SSL_TLS1_3_TICKET_NONCE_LENGTH );
        return( 0 );
    }
#endif /* MBEDTLS_SSL_TLS1_3_TICKET_NONCE_LENGTH */

#if defined(MBEDTLS_PKCS7_C)
    if( strcmp( "MBEDTLS_PKCS7_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PKCS7_C );
        return( 0 );
    }
#endif /* MBEDTLS_PKCS7_C */

#if defined(MBEDTLS_X509_CREATE_C)
    if( strcmp( "MBEDTLS_X509_CREATE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_X509_CREATE_C );
        return( 0 );
    }
#endif /* MBEDTLS_X509_CREATE_C */

#if defined(MBEDTLS_X509_CRL_PARSE_C)
    if( strcmp( "MBEDTLS_X509_CRL_PARSE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_X509_CRL_PARSE_C );
        return( 0 );
    }
#endif /* MBEDTLS_X509_CRL_PARSE_C */

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    if( strcmp( "MBEDTLS_X509_CRT_PARSE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_X509_CRT_PARSE_C );
        return( 0 );
    }
#endif /* MBEDTLS_X509_CRT_PARSE_C */

#if defined(MBEDTLS_X509_CRT_WRITE_C)
    if( strcmp( "MBEDTLS_X509_CRT_WRITE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_X509_CRT_WRITE_C );
        return( 0 );
    }
#endif /* MBEDTLS_X509_CRT_WRITE_C */

#if defined(MBEDTLS_X509_CSR_PARSE_C)
    if( strcmp( "MBEDTLS_X509_CSR_PARSE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_X509_CSR_PARSE_C );
        return( 0 );
    }
#endif /* MBEDTLS_X509_CSR_PARSE_C */

#if defined(MBEDTLS_X509_CSR_WRITE_C)
    if( strcmp( "MBEDTLS_X509_CSR_WRITE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_X509_CSR_WRITE_C );
        return( 0 );
    }
#endif /* MBEDTLS_X509_CSR_WRITE_C */

#if defined(MBEDTLS_X509_REMOVE_INFO)
    if( strcmp( "MBEDTLS_X509_REMOVE_INFO", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_X509_REMOVE_INFO );
        return( 0 );
    }
#endif /* MBEDTLS_X509_REMOVE_INFO */

#if defined(MBEDTLS_X509_RSASSA_PSS_SUPPORT)
    if( strcmp( "MBEDTLS_X509_RSASSA_PSS_SUPPORT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_X509_RSASSA_PSS_SUPPORT );
        return( 0 );
    }
#endif /* MBEDTLS_X509_RSASSA_PSS_SUPPORT */

#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
    if( strcmp( "MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK );
        return( 0 );
    }
#endif /* MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK */

#if defined(MBEDTLS_X509_USE_C)
    if( strcmp( "MBEDTLS_X509_USE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_X509_USE_C );
        return( 0 );
    }
#endif /* MBEDTLS_X509_USE_C */

#if defined(MBEDTLS_X509_MAX_FILE_PATH_LEN)
    if( strcmp( "MBEDTLS_X509_MAX_FILE_PATH_LEN", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_X509_MAX_FILE_PATH_LEN );
        return( 0 );
    }
#endif /* MBEDTLS_X509_MAX_FILE_PATH_LEN */

#if defined(MBEDTLS_X509_MAX_INTERMEDIATE_CA)
    if( strcmp( "MBEDTLS_X509_MAX_INTERMEDIATE_CA", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_X509_MAX_INTERMEDIATE_CA );
        return( 0 );
    }
#endif /* MBEDTLS_X509_MAX_INTERMEDIATE_CA */

#if defined(PSA_WANT_ALG_CBC_NO_PADDING)
    if( strcmp( "PSA_WANT_ALG_CBC_NO_PADDING", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_CBC_NO_PADDING );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_CBC_NO_PADDING */

#if defined(PSA_WANT_ALG_CBC_PKCS7)
    if( strcmp( "PSA_WANT_ALG_CBC_PKCS7", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_CBC_PKCS7 );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_CBC_PKCS7 */

#if defined(PSA_WANT_ALG_CCM)
    if( strcmp( "PSA_WANT_ALG_CCM", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_CCM );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_CCM */

#if defined(PSA_WANT_ALG_CCM_STAR_NO_TAG)
    if( strcmp( "PSA_WANT_ALG_CCM_STAR_NO_TAG", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_CCM_STAR_NO_TAG );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_CCM_STAR_NO_TAG */

#if defined(PSA_WANT_ALG_CMAC)
    if( strcmp( "PSA_WANT_ALG_CMAC", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_CMAC );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_CMAC */

#if defined(PSA_WANT_ALG_CFB)
    if( strcmp( "PSA_WANT_ALG_CFB", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_CFB );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_CFB */

#if defined(PSA_WANT_ALG_CHACHA20_POLY1305)
    if( strcmp( "PSA_WANT_ALG_CHACHA20_POLY1305", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_CHACHA20_POLY1305 );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_CHACHA20_POLY1305 */

#if defined(PSA_WANT_ALG_CTR)
    if( strcmp( "PSA_WANT_ALG_CTR", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_CTR );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_CTR */

#if defined(PSA_WANT_ALG_DETERMINISTIC_ECDSA)
    if( strcmp( "PSA_WANT_ALG_DETERMINISTIC_ECDSA", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_DETERMINISTIC_ECDSA );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_DETERMINISTIC_ECDSA */

#if defined(PSA_WANT_ALG_ECB_NO_PADDING)
    if( strcmp( "PSA_WANT_ALG_ECB_NO_PADDING", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_ECB_NO_PADDING );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_ECB_NO_PADDING */

#if defined(PSA_WANT_ALG_ECDH)
    if( strcmp( "PSA_WANT_ALG_ECDH", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_ECDH );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_ECDH */

#if defined(PSA_WANT_ALG_FFDH)
    if( strcmp( "PSA_WANT_ALG_FFDH", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_FFDH );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_FFDH */

#if defined(PSA_WANT_ALG_ECDSA)
    if( strcmp( "PSA_WANT_ALG_ECDSA", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_ECDSA );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_ECDSA */

#if defined(PSA_WANT_ALG_JPAKE)
    if( strcmp( "PSA_WANT_ALG_JPAKE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_JPAKE );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_JPAKE */

#if defined(PSA_WANT_ALG_GCM)
    if( strcmp( "PSA_WANT_ALG_GCM", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_GCM );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_GCM */

#if defined(PSA_WANT_ALG_HKDF)
    if( strcmp( "PSA_WANT_ALG_HKDF", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_HKDF );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_HKDF */

#if defined(PSA_WANT_ALG_HKDF_EXTRACT)
    if( strcmp( "PSA_WANT_ALG_HKDF_EXTRACT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_HKDF_EXTRACT );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_HKDF_EXTRACT */

#if defined(PSA_WANT_ALG_HKDF_EXPAND)
    if( strcmp( "PSA_WANT_ALG_HKDF_EXPAND", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_HKDF_EXPAND );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_HKDF_EXPAND */

#if defined(PSA_WANT_ALG_HMAC)
    if( strcmp( "PSA_WANT_ALG_HMAC", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_HMAC );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_HMAC */

#if defined(PSA_WANT_ALG_MD5)
    if( strcmp( "PSA_WANT_ALG_MD5", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_MD5 );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_MD5 */

#if defined(PSA_WANT_ALG_OFB)
    if( strcmp( "PSA_WANT_ALG_OFB", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_OFB );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_OFB */

#if defined(PSA_WANT_ALG_PBKDF2_HMAC)
    if( strcmp( "PSA_WANT_ALG_PBKDF2_HMAC", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_PBKDF2_HMAC );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_PBKDF2_HMAC */

#if defined(PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128)
    if( strcmp( "PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128 );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128 */

#if defined(PSA_WANT_ALG_RIPEMD160)
    if( strcmp( "PSA_WANT_ALG_RIPEMD160", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_RIPEMD160 );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_RIPEMD160 */

#if defined(PSA_WANT_ALG_RSA_OAEP)
    if( strcmp( "PSA_WANT_ALG_RSA_OAEP", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_RSA_OAEP );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_RSA_OAEP */

#if defined(PSA_WANT_ALG_RSA_PKCS1V15_CRYPT)
    if( strcmp( "PSA_WANT_ALG_RSA_PKCS1V15_CRYPT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_RSA_PKCS1V15_CRYPT );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_RSA_PKCS1V15_CRYPT */

#if defined(PSA_WANT_ALG_RSA_PKCS1V15_SIGN)
    if( strcmp( "PSA_WANT_ALG_RSA_PKCS1V15_SIGN", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_RSA_PKCS1V15_SIGN );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_RSA_PKCS1V15_SIGN */

#if defined(PSA_WANT_ALG_RSA_PSS)
    if( strcmp( "PSA_WANT_ALG_RSA_PSS", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_RSA_PSS );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_RSA_PSS */

#if defined(PSA_WANT_ALG_SHA_1)
    if( strcmp( "PSA_WANT_ALG_SHA_1", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_SHA_1 );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_SHA_1 */

#if defined(PSA_WANT_ALG_SHA_224)
    if( strcmp( "PSA_WANT_ALG_SHA_224", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_SHA_224 );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_SHA_224 */

#if defined(PSA_WANT_ALG_SHA_256)
    if( strcmp( "PSA_WANT_ALG_SHA_256", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_SHA_256 );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_SHA_256 */

#if defined(PSA_WANT_ALG_SHA_384)
    if( strcmp( "PSA_WANT_ALG_SHA_384", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_SHA_384 );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_SHA_384 */

#if defined(PSA_WANT_ALG_SHA_512)
    if( strcmp( "PSA_WANT_ALG_SHA_512", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_SHA_512 );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_SHA_512 */

#if defined(PSA_WANT_ALG_SHA3_224)
    if( strcmp( "PSA_WANT_ALG_SHA3_224", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_SHA3_224 );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_SHA3_224 */

#if defined(PSA_WANT_ALG_SHA3_256)
    if( strcmp( "PSA_WANT_ALG_SHA3_256", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_SHA3_256 );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_SHA3_256 */

#if defined(PSA_WANT_ALG_SHA3_384)
    if( strcmp( "PSA_WANT_ALG_SHA3_384", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_SHA3_384 );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_SHA3_384 */

#if defined(PSA_WANT_ALG_SHA3_512)
    if( strcmp( "PSA_WANT_ALG_SHA3_512", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_SHA3_512 );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_SHA3_512 */

#if defined(PSA_WANT_ALG_STREAM_CIPHER)
    if( strcmp( "PSA_WANT_ALG_STREAM_CIPHER", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_STREAM_CIPHER );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_STREAM_CIPHER */

#if defined(PSA_WANT_ALG_TLS12_PRF)
    if( strcmp( "PSA_WANT_ALG_TLS12_PRF", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_TLS12_PRF );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_TLS12_PRF */

#if defined(PSA_WANT_ALG_TLS12_PSK_TO_MS)
    if( strcmp( "PSA_WANT_ALG_TLS12_PSK_TO_MS", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_TLS12_PSK_TO_MS );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_TLS12_PSK_TO_MS */

#if defined(PSA_WANT_ALG_TLS12_ECJPAKE_TO_PMS)
    if( strcmp( "PSA_WANT_ALG_TLS12_ECJPAKE_TO_PMS", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_TLS12_ECJPAKE_TO_PMS );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_TLS12_ECJPAKE_TO_PMS */

#if defined(PSA_WANT_ALG_SHAKE128)
    if( strcmp( "PSA_WANT_ALG_SHAKE128", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_SHAKE128 );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_SHAKE128 */

#if defined(PSA_WANT_ALG_SHAKE256)
    if( strcmp( "PSA_WANT_ALG_SHAKE256", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ALG_SHAKE256 );
        return( 0 );
    }
#endif /* PSA_WANT_ALG_SHAKE256 */

#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_256)
    if( strcmp( "PSA_WANT_ECC_BRAINPOOL_P_R1_256", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ECC_BRAINPOOL_P_R1_256 );
        return( 0 );
    }
#endif /* PSA_WANT_ECC_BRAINPOOL_P_R1_256 */

#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_384)
    if( strcmp( "PSA_WANT_ECC_BRAINPOOL_P_R1_384", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ECC_BRAINPOOL_P_R1_384 );
        return( 0 );
    }
#endif /* PSA_WANT_ECC_BRAINPOOL_P_R1_384 */

#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_512)
    if( strcmp( "PSA_WANT_ECC_BRAINPOOL_P_R1_512", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ECC_BRAINPOOL_P_R1_512 );
        return( 0 );
    }
#endif /* PSA_WANT_ECC_BRAINPOOL_P_R1_512 */

#if defined(PSA_WANT_ECC_MONTGOMERY_255)
    if( strcmp( "PSA_WANT_ECC_MONTGOMERY_255", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ECC_MONTGOMERY_255 );
        return( 0 );
    }
#endif /* PSA_WANT_ECC_MONTGOMERY_255 */

#if defined(PSA_WANT_ECC_MONTGOMERY_448)
    if( strcmp( "PSA_WANT_ECC_MONTGOMERY_448", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ECC_MONTGOMERY_448 );
        return( 0 );
    }
#endif /* PSA_WANT_ECC_MONTGOMERY_448 */

#if defined(PSA_WANT_ECC_SECP_K1_256)
    if( strcmp( "PSA_WANT_ECC_SECP_K1_256", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ECC_SECP_K1_256 );
        return( 0 );
    }
#endif /* PSA_WANT_ECC_SECP_K1_256 */

#if defined(PSA_WANT_ECC_SECP_R1_256)
    if( strcmp( "PSA_WANT_ECC_SECP_R1_256", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ECC_SECP_R1_256 );
        return( 0 );
    }
#endif /* PSA_WANT_ECC_SECP_R1_256 */

#if defined(PSA_WANT_ECC_SECP_R1_384)
    if( strcmp( "PSA_WANT_ECC_SECP_R1_384", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ECC_SECP_R1_384 );
        return( 0 );
    }
#endif /* PSA_WANT_ECC_SECP_R1_384 */

#if defined(PSA_WANT_ECC_SECP_R1_521)
    if( strcmp( "PSA_WANT_ECC_SECP_R1_521", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_ECC_SECP_R1_521 );
        return( 0 );
    }
#endif /* PSA_WANT_ECC_SECP_R1_521 */

#if defined(PSA_WANT_DH_RFC7919_2048)
    if( strcmp( "PSA_WANT_DH_RFC7919_2048", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_DH_RFC7919_2048 );
        return( 0 );
    }
#endif /* PSA_WANT_DH_RFC7919_2048 */

#if defined(PSA_WANT_DH_RFC7919_3072)
    if( strcmp( "PSA_WANT_DH_RFC7919_3072", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_DH_RFC7919_3072 );
        return( 0 );
    }
#endif /* PSA_WANT_DH_RFC7919_3072 */

#if defined(PSA_WANT_DH_RFC7919_4096)
    if( strcmp( "PSA_WANT_DH_RFC7919_4096", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_DH_RFC7919_4096 );
        return( 0 );
    }
#endif /* PSA_WANT_DH_RFC7919_4096 */

#if defined(PSA_WANT_DH_RFC7919_6144)
    if( strcmp( "PSA_WANT_DH_RFC7919_6144", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_DH_RFC7919_6144 );
        return( 0 );
    }
#endif /* PSA_WANT_DH_RFC7919_6144 */

#if defined(PSA_WANT_DH_RFC7919_8192)
    if( strcmp( "PSA_WANT_DH_RFC7919_8192", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_DH_RFC7919_8192 );
        return( 0 );
    }
#endif /* PSA_WANT_DH_RFC7919_8192 */

#if defined(PSA_WANT_KEY_TYPE_DERIVE)
    if( strcmp( "PSA_WANT_KEY_TYPE_DERIVE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_DERIVE );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_DERIVE */

#if defined(PSA_WANT_KEY_TYPE_PASSWORD)
    if( strcmp( "PSA_WANT_KEY_TYPE_PASSWORD", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_PASSWORD );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_PASSWORD */

#if defined(PSA_WANT_KEY_TYPE_PASSWORD_HASH)
    if( strcmp( "PSA_WANT_KEY_TYPE_PASSWORD_HASH", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_PASSWORD_HASH );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_PASSWORD_HASH */

#if defined(PSA_WANT_KEY_TYPE_HMAC)
    if( strcmp( "PSA_WANT_KEY_TYPE_HMAC", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_HMAC );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_HMAC */

#if defined(PSA_WANT_KEY_TYPE_AES)
    if( strcmp( "PSA_WANT_KEY_TYPE_AES", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_AES );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_AES */

#if defined(PSA_WANT_KEY_TYPE_ARIA)
    if( strcmp( "PSA_WANT_KEY_TYPE_ARIA", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_ARIA );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_ARIA */

#if defined(PSA_WANT_KEY_TYPE_CAMELLIA)
    if( strcmp( "PSA_WANT_KEY_TYPE_CAMELLIA", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_CAMELLIA );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_CAMELLIA */

#if defined(PSA_WANT_KEY_TYPE_CHACHA20)
    if( strcmp( "PSA_WANT_KEY_TYPE_CHACHA20", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_CHACHA20 );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_CHACHA20 */

#if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY)
    if( strcmp( "PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY */

#if defined(PSA_WANT_KEY_TYPE_DH_PUBLIC_KEY)
    if( strcmp( "PSA_WANT_KEY_TYPE_DH_PUBLIC_KEY", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_DH_PUBLIC_KEY );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_DH_PUBLIC_KEY */

#if defined(PSA_WANT_KEY_TYPE_RAW_DATA)
    if( strcmp( "PSA_WANT_KEY_TYPE_RAW_DATA", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_RAW_DATA );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_RAW_DATA */

#if defined(PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY)
    if( strcmp( "PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY */

#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC)
    if( strcmp( "PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC */

#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT)
    if( strcmp( "PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT */

#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT)
    if( strcmp( "PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT */

#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE)
    if( strcmp( "PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE */

#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE)
    if( strcmp( "PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE */

#if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
    if( strcmp( "PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC */

#if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT)
    if( strcmp( "PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT */

#if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_EXPORT)
    if( strcmp( "PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_EXPORT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_EXPORT );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_EXPORT */

#if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE)
    if( strcmp( "PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE */

#if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_DERIVE)
    if( strcmp( "PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_DERIVE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_DERIVE );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_DERIVE */

#if defined(PSA_WANT_KEY_TYPE_DH_KEY_PAIR_BASIC)
    if( strcmp( "PSA_WANT_KEY_TYPE_DH_KEY_PAIR_BASIC", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_DH_KEY_PAIR_BASIC );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_DH_KEY_PAIR_BASIC */

#if defined(PSA_WANT_KEY_TYPE_DH_KEY_PAIR_IMPORT)
    if( strcmp( "PSA_WANT_KEY_TYPE_DH_KEY_PAIR_IMPORT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_DH_KEY_PAIR_IMPORT );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_DH_KEY_PAIR_IMPORT */

#if defined(PSA_WANT_KEY_TYPE_DH_KEY_PAIR_EXPORT)
    if( strcmp( "PSA_WANT_KEY_TYPE_DH_KEY_PAIR_EXPORT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_DH_KEY_PAIR_EXPORT );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_DH_KEY_PAIR_EXPORT */

#if defined(PSA_WANT_KEY_TYPE_DH_KEY_PAIR_GENERATE)
    if( strcmp( "PSA_WANT_KEY_TYPE_DH_KEY_PAIR_GENERATE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_DH_KEY_PAIR_GENERATE );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_DH_KEY_PAIR_GENERATE */

#if defined(PSA_WANT_KEY_TYPE_DH_KEY_PAIR_DERIVE)
    if( strcmp( "PSA_WANT_KEY_TYPE_DH_KEY_PAIR_DERIVE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( PSA_WANT_KEY_TYPE_DH_KEY_PAIR_DERIVE );
        return( 0 );
    }
#endif /* PSA_WANT_KEY_TYPE_DH_KEY_PAIR_DERIVE */

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
    if( strcmp( "MBEDTLS_MEMORY_BUFFER_ALLOC_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_MEMORY_BUFFER_ALLOC_C );
        return( 0 );
    }
#endif /* MBEDTLS_MEMORY_BUFFER_ALLOC_C */

#if defined(MBEDTLS_FS_IO)
    if( strcmp( "MBEDTLS_FS_IO", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_FS_IO );
        return( 0 );
    }
#endif /* MBEDTLS_FS_IO */

#if defined(MBEDTLS_HAVE_TIME)
    if( strcmp( "MBEDTLS_HAVE_TIME", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_HAVE_TIME );
        return( 0 );
    }
#endif /* MBEDTLS_HAVE_TIME */

#if defined(MBEDTLS_HAVE_TIME_DATE)
    if( strcmp( "MBEDTLS_HAVE_TIME_DATE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_HAVE_TIME_DATE );
        return( 0 );
    }
#endif /* MBEDTLS_HAVE_TIME_DATE */

#if defined(MBEDTLS_MEMORY_DEBUG)
    if( strcmp( "MBEDTLS_MEMORY_DEBUG", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_MEMORY_DEBUG );
        return( 0 );
    }
#endif /* MBEDTLS_MEMORY_DEBUG */

#if defined(MBEDTLS_MEMORY_BACKTRACE)
    if( strcmp( "MBEDTLS_MEMORY_BACKTRACE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_MEMORY_BACKTRACE );
        return( 0 );
    }
#endif /* MBEDTLS_MEMORY_BACKTRACE */

#if defined(MBEDTLS_PLATFORM_C)
    if( strcmp( "MBEDTLS_PLATFORM_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_C );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_C */

#if defined(MBEDTLS_PLATFORM_SETBUF_ALT)
    if( strcmp( "MBEDTLS_PLATFORM_SETBUF_ALT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_SETBUF_ALT );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_SETBUF_ALT */

#if defined(MBEDTLS_PLATFORM_EXIT_ALT)
    if( strcmp( "MBEDTLS_PLATFORM_EXIT_ALT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_EXIT_ALT );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_EXIT_ALT */

#if defined(MBEDTLS_PLATFORM_TIME_ALT)
    if( strcmp( "MBEDTLS_PLATFORM_TIME_ALT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_TIME_ALT );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_TIME_ALT */

#if defined(MBEDTLS_PLATFORM_FPRINTF_ALT)
    if( strcmp( "MBEDTLS_PLATFORM_FPRINTF_ALT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_FPRINTF_ALT );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_FPRINTF_ALT */

#if defined(MBEDTLS_PLATFORM_PRINTF_ALT)
    if( strcmp( "MBEDTLS_PLATFORM_PRINTF_ALT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_PRINTF_ALT );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_PRINTF_ALT */

#if defined(MBEDTLS_PLATFORM_SNPRINTF_ALT)
    if( strcmp( "MBEDTLS_PLATFORM_SNPRINTF_ALT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_SNPRINTF_ALT );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_SNPRINTF_ALT */

#if defined(MBEDTLS_PLATFORM_VSNPRINTF_ALT)
    if( strcmp( "MBEDTLS_PLATFORM_VSNPRINTF_ALT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_VSNPRINTF_ALT );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_VSNPRINTF_ALT */

#if defined(MBEDTLS_PLATFORM_NV_SEED_ALT)
    if( strcmp( "MBEDTLS_PLATFORM_NV_SEED_ALT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_NV_SEED_ALT );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_NV_SEED_ALT */

#if defined(MBEDTLS_PLATFORM_SETUP_TEARDOWN_ALT)
    if( strcmp( "MBEDTLS_PLATFORM_SETUP_TEARDOWN_ALT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_SETUP_TEARDOWN_ALT );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_SETUP_TEARDOWN_ALT */

#if defined(MBEDTLS_PLATFORM_MS_TIME_ALT)
    if( strcmp( "MBEDTLS_PLATFORM_MS_TIME_ALT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_MS_TIME_ALT );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_MS_TIME_ALT */

#if defined(MBEDTLS_PLATFORM_GMTIME_R_ALT)
    if( strcmp( "MBEDTLS_PLATFORM_GMTIME_R_ALT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_GMTIME_R_ALT );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_GMTIME_R_ALT */

#if defined(MBEDTLS_PLATFORM_MEMORY)
    if( strcmp( "MBEDTLS_PLATFORM_MEMORY", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_MEMORY );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_MEMORY */

#if defined(MBEDTLS_PLATFORM_NO_STD_FUNCTIONS)
    if( strcmp( "MBEDTLS_PLATFORM_NO_STD_FUNCTIONS", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_NO_STD_FUNCTIONS );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_NO_STD_FUNCTIONS */

#if defined(MBEDTLS_PLATFORM_ZEROIZE_ALT)
    if( strcmp( "MBEDTLS_PLATFORM_ZEROIZE_ALT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_ZEROIZE_ALT );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_ZEROIZE_ALT */

#if defined(MBEDTLS_THREADING_ALT)
    if( strcmp( "MBEDTLS_THREADING_ALT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_THREADING_ALT );
        return( 0 );
    }
#endif /* MBEDTLS_THREADING_ALT */

#if defined(MBEDTLS_THREADING_PTHREAD)
    if( strcmp( "MBEDTLS_THREADING_PTHREAD", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_THREADING_PTHREAD );
        return( 0 );
    }
#endif /* MBEDTLS_THREADING_PTHREAD */

#if defined(MBEDTLS_THREADING_C)
    if( strcmp( "MBEDTLS_THREADING_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_THREADING_C );
        return( 0 );
    }
#endif /* MBEDTLS_THREADING_C */

#if defined(MBEDTLS_MEMORY_ALIGN_MULTIPLE)
    if( strcmp( "MBEDTLS_MEMORY_ALIGN_MULTIPLE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_MEMORY_ALIGN_MULTIPLE );
        return( 0 );
    }
#endif /* MBEDTLS_MEMORY_ALIGN_MULTIPLE */

#if defined(MBEDTLS_PLATFORM_CALLOC_MACRO)
    if( strcmp( "MBEDTLS_PLATFORM_CALLOC_MACRO", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_CALLOC_MACRO );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_CALLOC_MACRO */

#if defined(MBEDTLS_PLATFORM_EXIT_MACRO)
    if( strcmp( "MBEDTLS_PLATFORM_EXIT_MACRO", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_EXIT_MACRO );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_EXIT_MACRO */

#if defined(MBEDTLS_PLATFORM_FREE_MACRO)
    if( strcmp( "MBEDTLS_PLATFORM_FREE_MACRO", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_FREE_MACRO );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_FREE_MACRO */

#if defined(MBEDTLS_PLATFORM_FPRINTF_MACRO)
    if( strcmp( "MBEDTLS_PLATFORM_FPRINTF_MACRO", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_FPRINTF_MACRO );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_FPRINTF_MACRO */

#if defined(MBEDTLS_PLATFORM_MS_TIME_TYPE_MACRO)
    if( strcmp( "MBEDTLS_PLATFORM_MS_TIME_TYPE_MACRO", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_MS_TIME_TYPE_MACRO );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_MS_TIME_TYPE_MACRO */

#if defined(MBEDTLS_PLATFORM_NV_SEED_READ_MACRO)
    if( strcmp( "MBEDTLS_PLATFORM_NV_SEED_READ_MACRO", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_NV_SEED_READ_MACRO );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_NV_SEED_READ_MACRO */

#if defined(MBEDTLS_PLATFORM_NV_SEED_WRITE_MACRO)
    if( strcmp( "MBEDTLS_PLATFORM_NV_SEED_WRITE_MACRO", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_NV_SEED_WRITE_MACRO );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_NV_SEED_WRITE_MACRO */

#if defined(MBEDTLS_PLATFORM_PRINTF_MACRO)
    if( strcmp( "MBEDTLS_PLATFORM_PRINTF_MACRO", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_PRINTF_MACRO );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_PRINTF_MACRO */

#if defined(MBEDTLS_PLATFORM_SETBUF_MACRO)
    if( strcmp( "MBEDTLS_PLATFORM_SETBUF_MACRO", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_SETBUF_MACRO );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_SETBUF_MACRO */

#if defined(MBEDTLS_PLATFORM_SNPRINTF_MACRO)
    if( strcmp( "MBEDTLS_PLATFORM_SNPRINTF_MACRO", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_SNPRINTF_MACRO );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_SNPRINTF_MACRO */

#if defined(MBEDTLS_PLATFORM_STD_CALLOC)
    if( strcmp( "MBEDTLS_PLATFORM_STD_CALLOC", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_STD_CALLOC );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_STD_CALLOC */

#if defined(MBEDTLS_PLATFORM_STD_EXIT)
    if( strcmp( "MBEDTLS_PLATFORM_STD_EXIT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_STD_EXIT );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_STD_EXIT */

#if defined(MBEDTLS_PLATFORM_STD_EXIT_FAILURE)
    if( strcmp( "MBEDTLS_PLATFORM_STD_EXIT_FAILURE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_STD_EXIT_FAILURE );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_STD_EXIT_FAILURE */

#if defined(MBEDTLS_PLATFORM_STD_EXIT_SUCCESS)
    if( strcmp( "MBEDTLS_PLATFORM_STD_EXIT_SUCCESS", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_STD_EXIT_SUCCESS );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_STD_EXIT_SUCCESS */

#if defined(MBEDTLS_PLATFORM_STD_FPRINTF)
    if( strcmp( "MBEDTLS_PLATFORM_STD_FPRINTF", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_STD_FPRINTF );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_STD_FPRINTF */

#if defined(MBEDTLS_PLATFORM_STD_FREE)
    if( strcmp( "MBEDTLS_PLATFORM_STD_FREE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_STD_FREE );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_STD_FREE */

#if defined(MBEDTLS_PLATFORM_STD_MEM_HDR)
    if( strcmp( "MBEDTLS_PLATFORM_STD_MEM_HDR", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_STD_MEM_HDR );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_STD_MEM_HDR */

#if defined(MBEDTLS_PLATFORM_STD_NV_SEED_FILE)
    if( strcmp( "MBEDTLS_PLATFORM_STD_NV_SEED_FILE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_STD_NV_SEED_FILE );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_STD_NV_SEED_FILE */

#if defined(MBEDTLS_PLATFORM_STD_NV_SEED_READ)
    if( strcmp( "MBEDTLS_PLATFORM_STD_NV_SEED_READ", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_STD_NV_SEED_READ );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_STD_NV_SEED_READ */

#if defined(MBEDTLS_PLATFORM_STD_NV_SEED_WRITE)
    if( strcmp( "MBEDTLS_PLATFORM_STD_NV_SEED_WRITE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_STD_NV_SEED_WRITE );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_STD_NV_SEED_WRITE */

#if defined(MBEDTLS_PLATFORM_STD_PRINTF)
    if( strcmp( "MBEDTLS_PLATFORM_STD_PRINTF", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_STD_PRINTF );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_STD_PRINTF */

#if defined(MBEDTLS_PLATFORM_STD_SETBUF)
    if( strcmp( "MBEDTLS_PLATFORM_STD_SETBUF", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_STD_SETBUF );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_STD_SETBUF */

#if defined(MBEDTLS_PLATFORM_STD_SNPRINTF)
    if( strcmp( "MBEDTLS_PLATFORM_STD_SNPRINTF", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_STD_SNPRINTF );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_STD_SNPRINTF */

#if defined(MBEDTLS_PLATFORM_STD_TIME)
    if( strcmp( "MBEDTLS_PLATFORM_STD_TIME", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_STD_TIME );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_STD_TIME */

#if defined(MBEDTLS_PLATFORM_TIME_MACRO)
    if( strcmp( "MBEDTLS_PLATFORM_TIME_MACRO", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_TIME_MACRO );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_TIME_MACRO */

#if defined(MBEDTLS_PLATFORM_TIME_TYPE_MACRO)
    if( strcmp( "MBEDTLS_PLATFORM_TIME_TYPE_MACRO", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_TIME_TYPE_MACRO );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_TIME_TYPE_MACRO */

#if defined(MBEDTLS_PLATFORM_VSNPRINTF_MACRO)
    if( strcmp( "MBEDTLS_PLATFORM_VSNPRINTF_MACRO", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_VSNPRINTF_MACRO );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_VSNPRINTF_MACRO */

#if defined(MBEDTLS_PRINTF_MS_TIME)
    if( strcmp( "MBEDTLS_PRINTF_MS_TIME", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PRINTF_MS_TIME );
        return( 0 );
    }
#endif /* MBEDTLS_PRINTF_MS_TIME */

#if defined(MBEDTLS_PLATFORM_DEV_RANDOM)
    if( strcmp( "MBEDTLS_PLATFORM_DEV_RANDOM", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PLATFORM_DEV_RANDOM );
        return( 0 );
    }
#endif /* MBEDTLS_PLATFORM_DEV_RANDOM */

#if defined(MBEDTLS_CHECK_RETURN_WARNING)
    if( strcmp( "MBEDTLS_CHECK_RETURN_WARNING", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_CHECK_RETURN_WARNING );
        return( 0 );
    }
#endif /* MBEDTLS_CHECK_RETURN_WARNING */

#if defined(MBEDTLS_DEPRECATED_WARNING)
    if( strcmp( "MBEDTLS_DEPRECATED_WARNING", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_DEPRECATED_WARNING );
        return( 0 );
    }
#endif /* MBEDTLS_DEPRECATED_WARNING */

#if defined(MBEDTLS_DEPRECATED_REMOVED)
    if( strcmp( "MBEDTLS_DEPRECATED_REMOVED", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_DEPRECATED_REMOVED );
        return( 0 );
    }
#endif /* MBEDTLS_DEPRECATED_REMOVED */

#if defined(MBEDTLS_CHECK_RETURN)
    if( strcmp( "MBEDTLS_CHECK_RETURN", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_CHECK_RETURN );
        return( 0 );
    }
#endif /* MBEDTLS_CHECK_RETURN */

#if defined(MBEDTLS_IGNORE_RETURN)
    if( strcmp( "MBEDTLS_IGNORE_RETURN", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_IGNORE_RETURN );
        return( 0 );
    }
#endif /* MBEDTLS_IGNORE_RETURN */

#if defined(MBEDTLS_SELF_TEST)
    if( strcmp( "MBEDTLS_SELF_TEST", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SELF_TEST );
        return( 0 );
    }
#endif /* MBEDTLS_SELF_TEST */

#if defined(MBEDTLS_TEST_CONSTANT_FLOW_MEMSAN)
    if( strcmp( "MBEDTLS_TEST_CONSTANT_FLOW_MEMSAN", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_TEST_CONSTANT_FLOW_MEMSAN );
        return( 0 );
    }
#endif /* MBEDTLS_TEST_CONSTANT_FLOW_MEMSAN */

#if defined(MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND)
    if( strcmp( "MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND );
        return( 0 );
    }
#endif /* MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND */

#if defined(MBEDTLS_TEST_HOOKS)
    if( strcmp( "MBEDTLS_TEST_HOOKS", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_TEST_HOOKS );
        return( 0 );
    }
#endif /* MBEDTLS_TEST_HOOKS */

#if defined(MBEDTLS_LMS_C)
    if( strcmp( "MBEDTLS_LMS_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_LMS_C );
        return( 0 );
    }
#endif /* MBEDTLS_LMS_C */

#if defined(MBEDTLS_LMS_PRIVATE)
    if( strcmp( "MBEDTLS_LMS_PRIVATE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_LMS_PRIVATE );
        return( 0 );
    }
#endif /* MBEDTLS_LMS_PRIVATE */

#if defined(MBEDTLS_MD_C)
    if( strcmp( "MBEDTLS_MD_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_MD_C );
        return( 0 );
    }
#endif /* MBEDTLS_MD_C */

#if defined(MBEDTLS_NIST_KW_C)
    if( strcmp( "MBEDTLS_NIST_KW_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_NIST_KW_C );
        return( 0 );
    }
#endif /* MBEDTLS_NIST_KW_C */

#if defined(MBEDTLS_PK_C)
    if( strcmp( "MBEDTLS_PK_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PK_C );
        return( 0 );
    }
#endif /* MBEDTLS_PK_C */

#if defined(MBEDTLS_PKCS5_C)
    if( strcmp( "MBEDTLS_PKCS5_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PKCS5_C );
        return( 0 );
    }
#endif /* MBEDTLS_PKCS5_C */

#if defined(MBEDTLS_PK_PARSE_C)
    if( strcmp( "MBEDTLS_PK_PARSE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PK_PARSE_C );
        return( 0 );
    }
#endif /* MBEDTLS_PK_PARSE_C */

#if defined(MBEDTLS_PK_PARSE_EC_EXTENDED)
    if( strcmp( "MBEDTLS_PK_PARSE_EC_EXTENDED", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PK_PARSE_EC_EXTENDED );
        return( 0 );
    }
#endif /* MBEDTLS_PK_PARSE_EC_EXTENDED */

#if defined(MBEDTLS_PK_PARSE_EC_COMPRESSED)
    if( strcmp( "MBEDTLS_PK_PARSE_EC_COMPRESSED", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PK_PARSE_EC_COMPRESSED );
        return( 0 );
    }
#endif /* MBEDTLS_PK_PARSE_EC_COMPRESSED */

#if defined(MBEDTLS_PK_WRITE_C)
    if( strcmp( "MBEDTLS_PK_WRITE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PK_WRITE_C );
        return( 0 );
    }
#endif /* MBEDTLS_PK_WRITE_C */

#if defined(MBEDTLS_ASN1_PARSE_C)
    if( strcmp( "MBEDTLS_ASN1_PARSE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_ASN1_PARSE_C );
        return( 0 );
    }
#endif /* MBEDTLS_ASN1_PARSE_C */

#if defined(MBEDTLS_ASN1_WRITE_C)
    if( strcmp( "MBEDTLS_ASN1_WRITE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_ASN1_WRITE_C );
        return( 0 );
    }
#endif /* MBEDTLS_ASN1_WRITE_C */

#if defined(MBEDTLS_BASE64_C)
    if( strcmp( "MBEDTLS_BASE64_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_BASE64_C );
        return( 0 );
    }
#endif /* MBEDTLS_BASE64_C */

#if defined(MBEDTLS_PEM_PARSE_C)
    if( strcmp( "MBEDTLS_PEM_PARSE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PEM_PARSE_C );
        return( 0 );
    }
#endif /* MBEDTLS_PEM_PARSE_C */

#if defined(MBEDTLS_PEM_WRITE_C)
    if( strcmp( "MBEDTLS_PEM_WRITE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PEM_WRITE_C );
        return( 0 );
    }
#endif /* MBEDTLS_PEM_WRITE_C */

#if defined(MBEDTLS_CTR_DRBG_C)
    if( strcmp( "MBEDTLS_CTR_DRBG_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_CTR_DRBG_C );
        return( 0 );
    }
#endif /* MBEDTLS_CTR_DRBG_C */

#if defined(MBEDTLS_ENTROPY_NO_SOURCES_OK)
    if( strcmp( "MBEDTLS_ENTROPY_NO_SOURCES_OK", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_ENTROPY_NO_SOURCES_OK );
        return( 0 );
    }
#endif /* MBEDTLS_ENTROPY_NO_SOURCES_OK */

#if defined(MBEDTLS_ENTROPY_NV_SEED)
    if( strcmp( "MBEDTLS_ENTROPY_NV_SEED", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_ENTROPY_NV_SEED );
        return( 0 );
    }
#endif /* MBEDTLS_ENTROPY_NV_SEED */

#if defined(MBEDTLS_HMAC_DRBG_C)
    if( strcmp( "MBEDTLS_HMAC_DRBG_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_HMAC_DRBG_C );
        return( 0 );
    }
#endif /* MBEDTLS_HMAC_DRBG_C */

#if defined(MBEDTLS_PSA_CRYPTO_C)
    if( strcmp( "MBEDTLS_PSA_CRYPTO_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_CRYPTO_C );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_CRYPTO_C */

#if defined(MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS)
    if( strcmp( "MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS */

#if defined(MBEDTLS_PSA_BUILTIN_GET_ENTROPY)
    if( strcmp( "MBEDTLS_PSA_BUILTIN_GET_ENTROPY", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_BUILTIN_GET_ENTROPY );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_BUILTIN_GET_ENTROPY */

#if defined(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS)
    if( strcmp( "MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
    if( strcmp( "MBEDTLS_PSA_CRYPTO_CLIENT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_CRYPTO_CLIENT );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_CRYPTO_CLIENT */

#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
    if( strcmp( "MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */

#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
    if( strcmp( "MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER */

#if defined(MBEDTLS_PSA_CRYPTO_SPM)
    if( strcmp( "MBEDTLS_PSA_CRYPTO_SPM", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_CRYPTO_SPM );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_CRYPTO_SPM */

#if defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
    if( strcmp( "MBEDTLS_PSA_CRYPTO_STORAGE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_CRYPTO_STORAGE_C );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_CRYPTO_STORAGE_C */

#if defined(MBEDTLS_PSA_DRIVER_GET_ENTROPY)
    if( strcmp( "MBEDTLS_PSA_DRIVER_GET_ENTROPY", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_DRIVER_GET_ENTROPY );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_DRIVER_GET_ENTROPY */

#if defined(MBEDTLS_PSA_ITS_FILE_C)
    if( strcmp( "MBEDTLS_PSA_ITS_FILE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_ITS_FILE_C );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_ITS_FILE_C */

#if defined(MBEDTLS_PSA_KEY_STORE_DYNAMIC)
    if( strcmp( "MBEDTLS_PSA_KEY_STORE_DYNAMIC", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_KEY_STORE_DYNAMIC );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_KEY_STORE_DYNAMIC */

#if defined(MBEDTLS_PSA_STATIC_KEY_SLOTS)
    if( strcmp( "MBEDTLS_PSA_STATIC_KEY_SLOTS", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_STATIC_KEY_SLOTS );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_STATIC_KEY_SLOTS */

#if defined(MBEDTLS_PSA_CRYPTO_PLATFORM_FILE)
    if( strcmp( "MBEDTLS_PSA_CRYPTO_PLATFORM_FILE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_CRYPTO_PLATFORM_FILE );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_CRYPTO_PLATFORM_FILE */

#if defined(MBEDTLS_PSA_CRYPTO_STRUCT_FILE)
    if( strcmp( "MBEDTLS_PSA_CRYPTO_STRUCT_FILE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_CRYPTO_STRUCT_FILE );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_CRYPTO_STRUCT_FILE */

#if defined(MBEDTLS_PSA_KEY_SLOT_COUNT)
    if( strcmp( "MBEDTLS_PSA_KEY_SLOT_COUNT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_KEY_SLOT_COUNT );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_KEY_SLOT_COUNT */

#if defined(MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE)
    if( strcmp( "MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE */

#if defined(MBEDTLS_PSA_CRYPTO_RNG_STRENGTH)
    if( strcmp( "MBEDTLS_PSA_CRYPTO_RNG_STRENGTH", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_CRYPTO_RNG_STRENGTH );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_CRYPTO_RNG_STRENGTH */

#if defined(MBEDTLS_PSA_CRYPTO_RNG_HASH)
    if( strcmp( "MBEDTLS_PSA_CRYPTO_RNG_HASH", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_CRYPTO_RNG_HASH );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_CRYPTO_RNG_HASH */

#if defined(MBEDTLS_PSA_RNG_RESEED_INTERVAL)
    if( strcmp( "MBEDTLS_PSA_RNG_RESEED_INTERVAL", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_RNG_RESEED_INTERVAL );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_RNG_RESEED_INTERVAL */

#if defined(MBEDTLS_AESNI_C)
    if( strcmp( "MBEDTLS_AESNI_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_AESNI_C );
        return( 0 );
    }
#endif /* MBEDTLS_AESNI_C */

#if defined(MBEDTLS_AESCE_C)
    if( strcmp( "MBEDTLS_AESCE_C", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_AESCE_C );
        return( 0 );
    }
#endif /* MBEDTLS_AESCE_C */

#if defined(MBEDTLS_AES_ROM_TABLES)
    if( strcmp( "MBEDTLS_AES_ROM_TABLES", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_AES_ROM_TABLES );
        return( 0 );
    }
#endif /* MBEDTLS_AES_ROM_TABLES */

#if defined(MBEDTLS_AES_FEWER_TABLES)
    if( strcmp( "MBEDTLS_AES_FEWER_TABLES", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_AES_FEWER_TABLES );
        return( 0 );
    }
#endif /* MBEDTLS_AES_FEWER_TABLES */

#if defined(MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH)
    if( strcmp( "MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH );
        return( 0 );
    }
#endif /* MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH */

#if defined(MBEDTLS_AES_USE_HARDWARE_ONLY)
    if( strcmp( "MBEDTLS_AES_USE_HARDWARE_ONLY", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_AES_USE_HARDWARE_ONLY );
        return( 0 );
    }
#endif /* MBEDTLS_AES_USE_HARDWARE_ONLY */

#if defined(MBEDTLS_BLOCK_CIPHER_NO_DECRYPT)
    if( strcmp( "MBEDTLS_BLOCK_CIPHER_NO_DECRYPT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_BLOCK_CIPHER_NO_DECRYPT );
        return( 0 );
    }
#endif /* MBEDTLS_BLOCK_CIPHER_NO_DECRYPT */

#if defined(MBEDTLS_CAMELLIA_SMALL_MEMORY)
    if( strcmp( "MBEDTLS_CAMELLIA_SMALL_MEMORY", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_CAMELLIA_SMALL_MEMORY );
        return( 0 );
    }
#endif /* MBEDTLS_CAMELLIA_SMALL_MEMORY */

#if defined(MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED)
    if( strcmp( "MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED );
        return( 0 );
    }
#endif /* MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED */

#if defined(MBEDTLS_ECP_NIST_OPTIM)
    if( strcmp( "MBEDTLS_ECP_NIST_OPTIM", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_ECP_NIST_OPTIM );
        return( 0 );
    }
#endif /* MBEDTLS_ECP_NIST_OPTIM */

#if defined(MBEDTLS_ECP_RESTARTABLE)
    if( strcmp( "MBEDTLS_ECP_RESTARTABLE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_ECP_RESTARTABLE );
        return( 0 );
    }
#endif /* MBEDTLS_ECP_RESTARTABLE */

#if defined(MBEDTLS_ECP_WITH_MPI_UINT)
    if( strcmp( "MBEDTLS_ECP_WITH_MPI_UINT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_ECP_WITH_MPI_UINT );
        return( 0 );
    }
#endif /* MBEDTLS_ECP_WITH_MPI_UINT */

#if defined(MBEDTLS_GCM_LARGE_TABLE)
    if( strcmp( "MBEDTLS_GCM_LARGE_TABLE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_GCM_LARGE_TABLE );
        return( 0 );
    }
#endif /* MBEDTLS_GCM_LARGE_TABLE */

#if defined(MBEDTLS_HAVE_ASM)
    if( strcmp( "MBEDTLS_HAVE_ASM", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_HAVE_ASM );
        return( 0 );
    }
#endif /* MBEDTLS_HAVE_ASM */

#if defined(MBEDTLS_HAVE_SSE2)
    if( strcmp( "MBEDTLS_HAVE_SSE2", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_HAVE_SSE2 );
        return( 0 );
    }
#endif /* MBEDTLS_HAVE_SSE2 */

#if defined(MBEDTLS_NO_UDBL_DIVISION)
    if( strcmp( "MBEDTLS_NO_UDBL_DIVISION", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_NO_UDBL_DIVISION );
        return( 0 );
    }
#endif /* MBEDTLS_NO_UDBL_DIVISION */

#if defined(MBEDTLS_NO_64BIT_MULTIPLICATION)
    if( strcmp( "MBEDTLS_NO_64BIT_MULTIPLICATION", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_NO_64BIT_MULTIPLICATION );
        return( 0 );
    }
#endif /* MBEDTLS_NO_64BIT_MULTIPLICATION */

#if defined(MBEDTLS_PSA_P256M_DRIVER_ENABLED)
    if( strcmp( "MBEDTLS_PSA_P256M_DRIVER_ENABLED", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_PSA_P256M_DRIVER_ENABLED );
        return( 0 );
    }
#endif /* MBEDTLS_PSA_P256M_DRIVER_ENABLED */

#if defined(MBEDTLS_RSA_NO_CRT)
    if( strcmp( "MBEDTLS_RSA_NO_CRT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_RSA_NO_CRT );
        return( 0 );
    }
#endif /* MBEDTLS_RSA_NO_CRT */

#if defined(MBEDTLS_SHA256_SMALLER)
    if( strcmp( "MBEDTLS_SHA256_SMALLER", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SHA256_SMALLER );
        return( 0 );
    }
#endif /* MBEDTLS_SHA256_SMALLER */

#if defined(MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT)
    if( strcmp( "MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT );
        return( 0 );
    }
#endif /* MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT */

#if defined(MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY)
    if( strcmp( "MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY );
        return( 0 );
    }
#endif /* MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY */

#if defined(MBEDTLS_SHA512_SMALLER)
    if( strcmp( "MBEDTLS_SHA512_SMALLER", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SHA512_SMALLER );
        return( 0 );
    }
#endif /* MBEDTLS_SHA512_SMALLER */

#if defined(MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT)
    if( strcmp( "MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT );
        return( 0 );
    }
#endif /* MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT */

#if defined(MBEDTLS_SHA512_USE_A64_CRYPTO_ONLY)
    if( strcmp( "MBEDTLS_SHA512_USE_A64_CRYPTO_ONLY", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_SHA512_USE_A64_CRYPTO_ONLY );
        return( 0 );
    }
#endif /* MBEDTLS_SHA512_USE_A64_CRYPTO_ONLY */

#if defined(MBEDTLS_ECP_FIXED_POINT_OPTIM)
    if( strcmp( "MBEDTLS_ECP_FIXED_POINT_OPTIM", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_ECP_FIXED_POINT_OPTIM );
        return( 0 );
    }
#endif /* MBEDTLS_ECP_FIXED_POINT_OPTIM */

#if defined(MBEDTLS_ECP_WINDOW_SIZE)
    if( strcmp( "MBEDTLS_ECP_WINDOW_SIZE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_ECP_WINDOW_SIZE );
        return( 0 );
    }
#endif /* MBEDTLS_ECP_WINDOW_SIZE */

#if defined(MBEDTLS_MPI_MAX_SIZE)
    if( strcmp( "MBEDTLS_MPI_MAX_SIZE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_MPI_MAX_SIZE );
        return( 0 );
    }
#endif /* MBEDTLS_MPI_MAX_SIZE */

#if defined(MBEDTLS_MPI_WINDOW_SIZE)
    if( strcmp( "MBEDTLS_MPI_WINDOW_SIZE", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_MPI_WINDOW_SIZE );
        return( 0 );
    }
#endif /* MBEDTLS_MPI_WINDOW_SIZE */

#if defined(MBEDTLS_RSA_GEN_KEY_MIN_BITS)
    if( strcmp( "MBEDTLS_RSA_GEN_KEY_MIN_BITS", config ) == 0 )
    {
        MACRO_EXPANSION_TO_STR( MBEDTLS_RSA_GEN_KEY_MIN_BITS );
        return( 0 );
    }
#endif /* MBEDTLS_RSA_GEN_KEY_MIN_BITS */

 /* If the symbol is not found, return an error */
    return 1;
}

void list_config(void)
{
    #if defined(MBEDTLS_CONFIG_VERSION)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_CONFIG_VERSION);
#endif /* MBEDTLS_CONFIG_VERSION */

#if defined(MBEDTLS_NET_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_NET_C);
#endif /* MBEDTLS_NET_C */

#if defined(MBEDTLS_TIMING_ALT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_TIMING_ALT);
#endif /* MBEDTLS_TIMING_ALT */

#if defined(MBEDTLS_TIMING_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_TIMING_C);
#endif /* MBEDTLS_TIMING_C */

#if defined(MBEDTLS_ERROR_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_ERROR_C);
#endif /* MBEDTLS_ERROR_C */

#if defined(MBEDTLS_ERROR_STRERROR_DUMMY)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_ERROR_STRERROR_DUMMY);
#endif /* MBEDTLS_ERROR_STRERROR_DUMMY */

#if defined(MBEDTLS_VERSION_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_VERSION_C);
#endif /* MBEDTLS_VERSION_C */

#if defined(MBEDTLS_VERSION_FEATURES)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_VERSION_FEATURES);
#endif /* MBEDTLS_VERSION_FEATURES */

#if defined(MBEDTLS_CONFIG_FILE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_CONFIG_FILE);
#endif /* MBEDTLS_CONFIG_FILE */

#if defined(MBEDTLS_USER_CONFIG_FILE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_USER_CONFIG_FILE);
#endif /* MBEDTLS_USER_CONFIG_FILE */

#if defined(MBEDTLS_SSL_NULL_CIPHERSUITES)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_NULL_CIPHERSUITES);
#endif /* MBEDTLS_SSL_NULL_CIPHERSUITES */

#if defined(MBEDTLS_DEBUG_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_DEBUG_C);
#endif /* MBEDTLS_DEBUG_C */

#if defined(MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED);
#endif /* MBEDTLS_KEY_EXCHANGE_ECDHE_ECDSA_ENABLED */

#if defined(MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED);
#endif /* MBEDTLS_KEY_EXCHANGE_ECDHE_PSK_ENABLED */

#if defined(MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED);
#endif /* MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED */

#if defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED);
#endif /* MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED */

#if defined(MBEDTLS_KEY_EXCHANGE_PSK_ENABLED)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_KEY_EXCHANGE_PSK_ENABLED);
#endif /* MBEDTLS_KEY_EXCHANGE_PSK_ENABLED */

#if defined(MBEDTLS_SSL_ALL_ALERT_MESSAGES)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_ALL_ALERT_MESSAGES);
#endif /* MBEDTLS_SSL_ALL_ALERT_MESSAGES */

#if defined(MBEDTLS_SSL_ALPN)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_ALPN);
#endif /* MBEDTLS_SSL_ALPN */

#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_ASYNC_PRIVATE);
#endif /* MBEDTLS_SSL_ASYNC_PRIVATE */

#if defined(MBEDTLS_SSL_CACHE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_CACHE_C);
#endif /* MBEDTLS_SSL_CACHE_C */

#if defined(MBEDTLS_SSL_CLI_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_CLI_C);
#endif /* MBEDTLS_SSL_CLI_C */

#if defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_CONTEXT_SERIALIZATION);
#endif /* MBEDTLS_SSL_CONTEXT_SERIALIZATION */

#if defined(MBEDTLS_SSL_COOKIE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_COOKIE_C);
#endif /* MBEDTLS_SSL_COOKIE_C */

#if defined(MBEDTLS_SSL_DEBUG_ALL)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_DEBUG_ALL);
#endif /* MBEDTLS_SSL_DEBUG_ALL */

#if defined(MBEDTLS_SSL_DTLS_ANTI_REPLAY)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_DTLS_ANTI_REPLAY);
#endif /* MBEDTLS_SSL_DTLS_ANTI_REPLAY */

#if defined(MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE);
#endif /* MBEDTLS_SSL_DTLS_CLIENT_PORT_REUSE */

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_DTLS_CONNECTION_ID);
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

#if defined(MBEDTLS_SSL_DTLS_HELLO_VERIFY)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_DTLS_HELLO_VERIFY);
#endif /* MBEDTLS_SSL_DTLS_HELLO_VERIFY */

#if defined(MBEDTLS_SSL_DTLS_SRTP)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_DTLS_SRTP);
#endif /* MBEDTLS_SSL_DTLS_SRTP */

#if defined(MBEDTLS_SSL_EARLY_DATA)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_EARLY_DATA);
#endif /* MBEDTLS_SSL_EARLY_DATA */

#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_ENCRYPT_THEN_MAC);
#endif /* MBEDTLS_SSL_ENCRYPT_THEN_MAC */

#if defined(MBEDTLS_SSL_EXTENDED_MASTER_SECRET)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_EXTENDED_MASTER_SECRET);
#endif /* MBEDTLS_SSL_EXTENDED_MASTER_SECRET */

#if defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE);
#endif /* MBEDTLS_SSL_KEEP_PEER_CERTIFICATE */

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH);
#endif /* MBEDTLS_SSL_MAX_FRAGMENT_LENGTH */

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_PROTO_DTLS);
#endif /* MBEDTLS_SSL_PROTO_DTLS */

#if defined(MBEDTLS_SSL_PROTO_TLS1_2)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_PROTO_TLS1_2);
#endif /* MBEDTLS_SSL_PROTO_TLS1_2 */

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_PROTO_TLS1_3);
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

#if defined(MBEDTLS_SSL_RECORD_SIZE_LIMIT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_RECORD_SIZE_LIMIT);
#endif /* MBEDTLS_SSL_RECORD_SIZE_LIMIT */

#if defined(MBEDTLS_SSL_KEYING_MATERIAL_EXPORT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_KEYING_MATERIAL_EXPORT);
#endif /* MBEDTLS_SSL_KEYING_MATERIAL_EXPORT */

#if defined(MBEDTLS_SSL_RENEGOTIATION)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_RENEGOTIATION);
#endif /* MBEDTLS_SSL_RENEGOTIATION */

#if defined(MBEDTLS_SSL_SERVER_NAME_INDICATION)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_SERVER_NAME_INDICATION);
#endif /* MBEDTLS_SSL_SERVER_NAME_INDICATION */

#if defined(MBEDTLS_SSL_SESSION_TICKETS)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_SESSION_TICKETS);
#endif /* MBEDTLS_SSL_SESSION_TICKETS */

#if defined(MBEDTLS_SSL_SRV_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_SRV_C);
#endif /* MBEDTLS_SSL_SRV_C */

#if defined(MBEDTLS_SSL_TICKET_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_TICKET_C);
#endif /* MBEDTLS_SSL_TICKET_C */

#if defined(MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE);
#endif /* MBEDTLS_SSL_TLS1_3_COMPATIBILITY_MODE */

#if defined(MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED);
#endif /* MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ENABLED */

#if defined(MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED);
#endif /* MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ENABLED */

#if defined(MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED);
#endif /* MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL_ENABLED */

#if defined(MBEDTLS_SSL_TLS_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_TLS_C);
#endif /* MBEDTLS_SSL_TLS_C */

#if defined(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH);
#endif /* MBEDTLS_SSL_VARIABLE_BUFFER_LENGTH */

#if defined(MBEDTLS_PSK_MAX_LEN)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSK_MAX_LEN);
#endif /* MBEDTLS_PSK_MAX_LEN */

#if defined(MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES);
#endif /* MBEDTLS_SSL_CACHE_DEFAULT_MAX_ENTRIES */

#if defined(MBEDTLS_SSL_CACHE_DEFAULT_TIMEOUT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_CACHE_DEFAULT_TIMEOUT);
#endif /* MBEDTLS_SSL_CACHE_DEFAULT_TIMEOUT */

#if defined(MBEDTLS_SSL_CID_IN_LEN_MAX)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_CID_IN_LEN_MAX);
#endif /* MBEDTLS_SSL_CID_IN_LEN_MAX */

#if defined(MBEDTLS_SSL_CID_OUT_LEN_MAX)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_CID_OUT_LEN_MAX);
#endif /* MBEDTLS_SSL_CID_OUT_LEN_MAX */

#if defined(MBEDTLS_SSL_CID_TLS1_3_PADDING_GRANULARITY)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_CID_TLS1_3_PADDING_GRANULARITY);
#endif /* MBEDTLS_SSL_CID_TLS1_3_PADDING_GRANULARITY */

#if defined(MBEDTLS_SSL_COOKIE_TIMEOUT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_COOKIE_TIMEOUT);
#endif /* MBEDTLS_SSL_COOKIE_TIMEOUT */

#if defined(MBEDTLS_SSL_DTLS_MAX_BUFFERING)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_DTLS_MAX_BUFFERING);
#endif /* MBEDTLS_SSL_DTLS_MAX_BUFFERING */

#if defined(MBEDTLS_SSL_IN_CONTENT_LEN)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_IN_CONTENT_LEN);
#endif /* MBEDTLS_SSL_IN_CONTENT_LEN */

#if defined(MBEDTLS_SSL_MAX_EARLY_DATA_SIZE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_MAX_EARLY_DATA_SIZE);
#endif /* MBEDTLS_SSL_MAX_EARLY_DATA_SIZE */

#if defined(MBEDTLS_SSL_OUT_CONTENT_LEN)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_OUT_CONTENT_LEN);
#endif /* MBEDTLS_SSL_OUT_CONTENT_LEN */

#if defined(MBEDTLS_SSL_TLS1_3_DEFAULT_NEW_SESSION_TICKETS)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_TLS1_3_DEFAULT_NEW_SESSION_TICKETS);
#endif /* MBEDTLS_SSL_TLS1_3_DEFAULT_NEW_SESSION_TICKETS */

#if defined(MBEDTLS_SSL_TLS1_3_TICKET_AGE_TOLERANCE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_TLS1_3_TICKET_AGE_TOLERANCE);
#endif /* MBEDTLS_SSL_TLS1_3_TICKET_AGE_TOLERANCE */

#if defined(MBEDTLS_SSL_TLS1_3_TICKET_NONCE_LENGTH)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SSL_TLS1_3_TICKET_NONCE_LENGTH);
#endif /* MBEDTLS_SSL_TLS1_3_TICKET_NONCE_LENGTH */

#if defined(MBEDTLS_PKCS7_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PKCS7_C);
#endif /* MBEDTLS_PKCS7_C */

#if defined(MBEDTLS_X509_CREATE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_X509_CREATE_C);
#endif /* MBEDTLS_X509_CREATE_C */

#if defined(MBEDTLS_X509_CRL_PARSE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_X509_CRL_PARSE_C);
#endif /* MBEDTLS_X509_CRL_PARSE_C */

#if defined(MBEDTLS_X509_CRT_PARSE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_X509_CRT_PARSE_C);
#endif /* MBEDTLS_X509_CRT_PARSE_C */

#if defined(MBEDTLS_X509_CRT_WRITE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_X509_CRT_WRITE_C);
#endif /* MBEDTLS_X509_CRT_WRITE_C */

#if defined(MBEDTLS_X509_CSR_PARSE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_X509_CSR_PARSE_C);
#endif /* MBEDTLS_X509_CSR_PARSE_C */

#if defined(MBEDTLS_X509_CSR_WRITE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_X509_CSR_WRITE_C);
#endif /* MBEDTLS_X509_CSR_WRITE_C */

#if defined(MBEDTLS_X509_REMOVE_INFO)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_X509_REMOVE_INFO);
#endif /* MBEDTLS_X509_REMOVE_INFO */

#if defined(MBEDTLS_X509_RSASSA_PSS_SUPPORT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_X509_RSASSA_PSS_SUPPORT);
#endif /* MBEDTLS_X509_RSASSA_PSS_SUPPORT */

#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK);
#endif /* MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK */

#if defined(MBEDTLS_X509_USE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_X509_USE_C);
#endif /* MBEDTLS_X509_USE_C */

#if defined(MBEDTLS_X509_MAX_FILE_PATH_LEN)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_X509_MAX_FILE_PATH_LEN);
#endif /* MBEDTLS_X509_MAX_FILE_PATH_LEN */

#if defined(MBEDTLS_X509_MAX_INTERMEDIATE_CA)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_X509_MAX_INTERMEDIATE_CA);
#endif /* MBEDTLS_X509_MAX_INTERMEDIATE_CA */

#if defined(PSA_WANT_ALG_CBC_NO_PADDING)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_CBC_NO_PADDING);
#endif /* PSA_WANT_ALG_CBC_NO_PADDING */

#if defined(PSA_WANT_ALG_CBC_PKCS7)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_CBC_PKCS7);
#endif /* PSA_WANT_ALG_CBC_PKCS7 */

#if defined(PSA_WANT_ALG_CCM)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_CCM);
#endif /* PSA_WANT_ALG_CCM */

#if defined(PSA_WANT_ALG_CCM_STAR_NO_TAG)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_CCM_STAR_NO_TAG);
#endif /* PSA_WANT_ALG_CCM_STAR_NO_TAG */

#if defined(PSA_WANT_ALG_CMAC)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_CMAC);
#endif /* PSA_WANT_ALG_CMAC */

#if defined(PSA_WANT_ALG_CFB)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_CFB);
#endif /* PSA_WANT_ALG_CFB */

#if defined(PSA_WANT_ALG_CHACHA20_POLY1305)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_CHACHA20_POLY1305);
#endif /* PSA_WANT_ALG_CHACHA20_POLY1305 */

#if defined(PSA_WANT_ALG_CTR)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_CTR);
#endif /* PSA_WANT_ALG_CTR */

#if defined(PSA_WANT_ALG_DETERMINISTIC_ECDSA)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_DETERMINISTIC_ECDSA);
#endif /* PSA_WANT_ALG_DETERMINISTIC_ECDSA */

#if defined(PSA_WANT_ALG_ECB_NO_PADDING)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_ECB_NO_PADDING);
#endif /* PSA_WANT_ALG_ECB_NO_PADDING */

#if defined(PSA_WANT_ALG_ECDH)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_ECDH);
#endif /* PSA_WANT_ALG_ECDH */

#if defined(PSA_WANT_ALG_FFDH)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_FFDH);
#endif /* PSA_WANT_ALG_FFDH */

#if defined(PSA_WANT_ALG_ECDSA)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_ECDSA);
#endif /* PSA_WANT_ALG_ECDSA */

#if defined(PSA_WANT_ALG_JPAKE)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_JPAKE);
#endif /* PSA_WANT_ALG_JPAKE */

#if defined(PSA_WANT_ALG_GCM)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_GCM);
#endif /* PSA_WANT_ALG_GCM */

#if defined(PSA_WANT_ALG_HKDF)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_HKDF);
#endif /* PSA_WANT_ALG_HKDF */

#if defined(PSA_WANT_ALG_HKDF_EXTRACT)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_HKDF_EXTRACT);
#endif /* PSA_WANT_ALG_HKDF_EXTRACT */

#if defined(PSA_WANT_ALG_HKDF_EXPAND)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_HKDF_EXPAND);
#endif /* PSA_WANT_ALG_HKDF_EXPAND */

#if defined(PSA_WANT_ALG_HMAC)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_HMAC);
#endif /* PSA_WANT_ALG_HMAC */

#if defined(PSA_WANT_ALG_MD5)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_MD5);
#endif /* PSA_WANT_ALG_MD5 */

#if defined(PSA_WANT_ALG_OFB)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_OFB);
#endif /* PSA_WANT_ALG_OFB */

#if defined(PSA_WANT_ALG_PBKDF2_HMAC)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_PBKDF2_HMAC);
#endif /* PSA_WANT_ALG_PBKDF2_HMAC */

#if defined(PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128);
#endif /* PSA_WANT_ALG_PBKDF2_AES_CMAC_PRF_128 */

#if defined(PSA_WANT_ALG_RIPEMD160)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_RIPEMD160);
#endif /* PSA_WANT_ALG_RIPEMD160 */

#if defined(PSA_WANT_ALG_RSA_OAEP)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_RSA_OAEP);
#endif /* PSA_WANT_ALG_RSA_OAEP */

#if defined(PSA_WANT_ALG_RSA_PKCS1V15_CRYPT)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_RSA_PKCS1V15_CRYPT);
#endif /* PSA_WANT_ALG_RSA_PKCS1V15_CRYPT */

#if defined(PSA_WANT_ALG_RSA_PKCS1V15_SIGN)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_RSA_PKCS1V15_SIGN);
#endif /* PSA_WANT_ALG_RSA_PKCS1V15_SIGN */

#if defined(PSA_WANT_ALG_RSA_PSS)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_RSA_PSS);
#endif /* PSA_WANT_ALG_RSA_PSS */

#if defined(PSA_WANT_ALG_SHA_1)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_SHA_1);
#endif /* PSA_WANT_ALG_SHA_1 */

#if defined(PSA_WANT_ALG_SHA_224)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_SHA_224);
#endif /* PSA_WANT_ALG_SHA_224 */

#if defined(PSA_WANT_ALG_SHA_256)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_SHA_256);
#endif /* PSA_WANT_ALG_SHA_256 */

#if defined(PSA_WANT_ALG_SHA_384)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_SHA_384);
#endif /* PSA_WANT_ALG_SHA_384 */

#if defined(PSA_WANT_ALG_SHA_512)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_SHA_512);
#endif /* PSA_WANT_ALG_SHA_512 */

#if defined(PSA_WANT_ALG_SHA3_224)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_SHA3_224);
#endif /* PSA_WANT_ALG_SHA3_224 */

#if defined(PSA_WANT_ALG_SHA3_256)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_SHA3_256);
#endif /* PSA_WANT_ALG_SHA3_256 */

#if defined(PSA_WANT_ALG_SHA3_384)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_SHA3_384);
#endif /* PSA_WANT_ALG_SHA3_384 */

#if defined(PSA_WANT_ALG_SHA3_512)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_SHA3_512);
#endif /* PSA_WANT_ALG_SHA3_512 */

#if defined(PSA_WANT_ALG_STREAM_CIPHER)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_STREAM_CIPHER);
#endif /* PSA_WANT_ALG_STREAM_CIPHER */

#if defined(PSA_WANT_ALG_TLS12_PRF)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_TLS12_PRF);
#endif /* PSA_WANT_ALG_TLS12_PRF */

#if defined(PSA_WANT_ALG_TLS12_PSK_TO_MS)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_TLS12_PSK_TO_MS);
#endif /* PSA_WANT_ALG_TLS12_PSK_TO_MS */

#if defined(PSA_WANT_ALG_TLS12_ECJPAKE_TO_PMS)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_TLS12_ECJPAKE_TO_PMS);
#endif /* PSA_WANT_ALG_TLS12_ECJPAKE_TO_PMS */

#if defined(PSA_WANT_ALG_SHAKE128)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_SHAKE128);
#endif /* PSA_WANT_ALG_SHAKE128 */

#if defined(PSA_WANT_ALG_SHAKE256)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ALG_SHAKE256);
#endif /* PSA_WANT_ALG_SHAKE256 */

#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_256)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ECC_BRAINPOOL_P_R1_256);
#endif /* PSA_WANT_ECC_BRAINPOOL_P_R1_256 */

#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_384)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ECC_BRAINPOOL_P_R1_384);
#endif /* PSA_WANT_ECC_BRAINPOOL_P_R1_384 */

#if defined(PSA_WANT_ECC_BRAINPOOL_P_R1_512)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ECC_BRAINPOOL_P_R1_512);
#endif /* PSA_WANT_ECC_BRAINPOOL_P_R1_512 */

#if defined(PSA_WANT_ECC_MONTGOMERY_255)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ECC_MONTGOMERY_255);
#endif /* PSA_WANT_ECC_MONTGOMERY_255 */

#if defined(PSA_WANT_ECC_MONTGOMERY_448)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ECC_MONTGOMERY_448);
#endif /* PSA_WANT_ECC_MONTGOMERY_448 */

#if defined(PSA_WANT_ECC_SECP_K1_256)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ECC_SECP_K1_256);
#endif /* PSA_WANT_ECC_SECP_K1_256 */

#if defined(PSA_WANT_ECC_SECP_R1_256)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ECC_SECP_R1_256);
#endif /* PSA_WANT_ECC_SECP_R1_256 */

#if defined(PSA_WANT_ECC_SECP_R1_384)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ECC_SECP_R1_384);
#endif /* PSA_WANT_ECC_SECP_R1_384 */

#if defined(PSA_WANT_ECC_SECP_R1_521)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_ECC_SECP_R1_521);
#endif /* PSA_WANT_ECC_SECP_R1_521 */

#if defined(PSA_WANT_DH_RFC7919_2048)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_DH_RFC7919_2048);
#endif /* PSA_WANT_DH_RFC7919_2048 */

#if defined(PSA_WANT_DH_RFC7919_3072)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_DH_RFC7919_3072);
#endif /* PSA_WANT_DH_RFC7919_3072 */

#if defined(PSA_WANT_DH_RFC7919_4096)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_DH_RFC7919_4096);
#endif /* PSA_WANT_DH_RFC7919_4096 */

#if defined(PSA_WANT_DH_RFC7919_6144)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_DH_RFC7919_6144);
#endif /* PSA_WANT_DH_RFC7919_6144 */

#if defined(PSA_WANT_DH_RFC7919_8192)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_DH_RFC7919_8192);
#endif /* PSA_WANT_DH_RFC7919_8192 */

#if defined(PSA_WANT_KEY_TYPE_DERIVE)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_DERIVE);
#endif /* PSA_WANT_KEY_TYPE_DERIVE */

#if defined(PSA_WANT_KEY_TYPE_PASSWORD)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_PASSWORD);
#endif /* PSA_WANT_KEY_TYPE_PASSWORD */

#if defined(PSA_WANT_KEY_TYPE_PASSWORD_HASH)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_PASSWORD_HASH);
#endif /* PSA_WANT_KEY_TYPE_PASSWORD_HASH */

#if defined(PSA_WANT_KEY_TYPE_HMAC)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_HMAC);
#endif /* PSA_WANT_KEY_TYPE_HMAC */

#if defined(PSA_WANT_KEY_TYPE_AES)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_AES);
#endif /* PSA_WANT_KEY_TYPE_AES */

#if defined(PSA_WANT_KEY_TYPE_ARIA)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_ARIA);
#endif /* PSA_WANT_KEY_TYPE_ARIA */

#if defined(PSA_WANT_KEY_TYPE_CAMELLIA)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_CAMELLIA);
#endif /* PSA_WANT_KEY_TYPE_CAMELLIA */

#if defined(PSA_WANT_KEY_TYPE_CHACHA20)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_CHACHA20);
#endif /* PSA_WANT_KEY_TYPE_CHACHA20 */

#if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY);
#endif /* PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY */

#if defined(PSA_WANT_KEY_TYPE_DH_PUBLIC_KEY)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_DH_PUBLIC_KEY);
#endif /* PSA_WANT_KEY_TYPE_DH_PUBLIC_KEY */

#if defined(PSA_WANT_KEY_TYPE_RAW_DATA)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_RAW_DATA);
#endif /* PSA_WANT_KEY_TYPE_RAW_DATA */

#if defined(PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY);
#endif /* PSA_WANT_KEY_TYPE_RSA_PUBLIC_KEY */

#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC);
#endif /* PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_BASIC */

#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT);
#endif /* PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT */

#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT);
#endif /* PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_EXPORT */

#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE);
#endif /* PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_GENERATE */

#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE);
#endif /* PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_DERIVE */

#if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC);
#endif /* PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_BASIC */

#if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT);
#endif /* PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_IMPORT */

#if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_EXPORT)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_EXPORT);
#endif /* PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_EXPORT */

#if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE);
#endif /* PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_GENERATE */

#if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_DERIVE)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_DERIVE);
#endif /* PSA_WANT_KEY_TYPE_RSA_KEY_PAIR_DERIVE */

#if defined(PSA_WANT_KEY_TYPE_DH_KEY_PAIR_BASIC)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_DH_KEY_PAIR_BASIC);
#endif /* PSA_WANT_KEY_TYPE_DH_KEY_PAIR_BASIC */

#if defined(PSA_WANT_KEY_TYPE_DH_KEY_PAIR_IMPORT)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_DH_KEY_PAIR_IMPORT);
#endif /* PSA_WANT_KEY_TYPE_DH_KEY_PAIR_IMPORT */

#if defined(PSA_WANT_KEY_TYPE_DH_KEY_PAIR_EXPORT)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_DH_KEY_PAIR_EXPORT);
#endif /* PSA_WANT_KEY_TYPE_DH_KEY_PAIR_EXPORT */

#if defined(PSA_WANT_KEY_TYPE_DH_KEY_PAIR_GENERATE)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_DH_KEY_PAIR_GENERATE);
#endif /* PSA_WANT_KEY_TYPE_DH_KEY_PAIR_GENERATE */

#if defined(PSA_WANT_KEY_TYPE_DH_KEY_PAIR_DERIVE)
    OUTPUT_MACRO_NAME_VALUE(PSA_WANT_KEY_TYPE_DH_KEY_PAIR_DERIVE);
#endif /* PSA_WANT_KEY_TYPE_DH_KEY_PAIR_DERIVE */

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_MEMORY_BUFFER_ALLOC_C);
#endif /* MBEDTLS_MEMORY_BUFFER_ALLOC_C */

#if defined(MBEDTLS_FS_IO)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_FS_IO);
#endif /* MBEDTLS_FS_IO */

#if defined(MBEDTLS_HAVE_TIME)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_HAVE_TIME);
#endif /* MBEDTLS_HAVE_TIME */

#if defined(MBEDTLS_HAVE_TIME_DATE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_HAVE_TIME_DATE);
#endif /* MBEDTLS_HAVE_TIME_DATE */

#if defined(MBEDTLS_MEMORY_DEBUG)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_MEMORY_DEBUG);
#endif /* MBEDTLS_MEMORY_DEBUG */

#if defined(MBEDTLS_MEMORY_BACKTRACE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_MEMORY_BACKTRACE);
#endif /* MBEDTLS_MEMORY_BACKTRACE */

#if defined(MBEDTLS_PLATFORM_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_C);
#endif /* MBEDTLS_PLATFORM_C */

#if defined(MBEDTLS_PLATFORM_SETBUF_ALT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_SETBUF_ALT);
#endif /* MBEDTLS_PLATFORM_SETBUF_ALT */

#if defined(MBEDTLS_PLATFORM_EXIT_ALT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_EXIT_ALT);
#endif /* MBEDTLS_PLATFORM_EXIT_ALT */

#if defined(MBEDTLS_PLATFORM_TIME_ALT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_TIME_ALT);
#endif /* MBEDTLS_PLATFORM_TIME_ALT */

#if defined(MBEDTLS_PLATFORM_FPRINTF_ALT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_FPRINTF_ALT);
#endif /* MBEDTLS_PLATFORM_FPRINTF_ALT */

#if defined(MBEDTLS_PLATFORM_PRINTF_ALT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_PRINTF_ALT);
#endif /* MBEDTLS_PLATFORM_PRINTF_ALT */

#if defined(MBEDTLS_PLATFORM_SNPRINTF_ALT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_SNPRINTF_ALT);
#endif /* MBEDTLS_PLATFORM_SNPRINTF_ALT */

#if defined(MBEDTLS_PLATFORM_VSNPRINTF_ALT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_VSNPRINTF_ALT);
#endif /* MBEDTLS_PLATFORM_VSNPRINTF_ALT */

#if defined(MBEDTLS_PLATFORM_NV_SEED_ALT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_NV_SEED_ALT);
#endif /* MBEDTLS_PLATFORM_NV_SEED_ALT */

#if defined(MBEDTLS_PLATFORM_SETUP_TEARDOWN_ALT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_SETUP_TEARDOWN_ALT);
#endif /* MBEDTLS_PLATFORM_SETUP_TEARDOWN_ALT */

#if defined(MBEDTLS_PLATFORM_MS_TIME_ALT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_MS_TIME_ALT);
#endif /* MBEDTLS_PLATFORM_MS_TIME_ALT */

#if defined(MBEDTLS_PLATFORM_GMTIME_R_ALT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_GMTIME_R_ALT);
#endif /* MBEDTLS_PLATFORM_GMTIME_R_ALT */

#if defined(MBEDTLS_PLATFORM_MEMORY)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_MEMORY);
#endif /* MBEDTLS_PLATFORM_MEMORY */

#if defined(MBEDTLS_PLATFORM_NO_STD_FUNCTIONS)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_NO_STD_FUNCTIONS);
#endif /* MBEDTLS_PLATFORM_NO_STD_FUNCTIONS */

#if defined(MBEDTLS_PLATFORM_ZEROIZE_ALT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_ZEROIZE_ALT);
#endif /* MBEDTLS_PLATFORM_ZEROIZE_ALT */

#if defined(MBEDTLS_THREADING_ALT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_THREADING_ALT);
#endif /* MBEDTLS_THREADING_ALT */

#if defined(MBEDTLS_THREADING_PTHREAD)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_THREADING_PTHREAD);
#endif /* MBEDTLS_THREADING_PTHREAD */

#if defined(MBEDTLS_THREADING_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_THREADING_C);
#endif /* MBEDTLS_THREADING_C */

#if defined(MBEDTLS_MEMORY_ALIGN_MULTIPLE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_MEMORY_ALIGN_MULTIPLE);
#endif /* MBEDTLS_MEMORY_ALIGN_MULTIPLE */

#if defined(MBEDTLS_PLATFORM_CALLOC_MACRO)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_CALLOC_MACRO);
#endif /* MBEDTLS_PLATFORM_CALLOC_MACRO */

#if defined(MBEDTLS_PLATFORM_EXIT_MACRO)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_EXIT_MACRO);
#endif /* MBEDTLS_PLATFORM_EXIT_MACRO */

#if defined(MBEDTLS_PLATFORM_FREE_MACRO)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_FREE_MACRO);
#endif /* MBEDTLS_PLATFORM_FREE_MACRO */

#if defined(MBEDTLS_PLATFORM_FPRINTF_MACRO)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_FPRINTF_MACRO);
#endif /* MBEDTLS_PLATFORM_FPRINTF_MACRO */

#if defined(MBEDTLS_PLATFORM_MS_TIME_TYPE_MACRO)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_MS_TIME_TYPE_MACRO);
#endif /* MBEDTLS_PLATFORM_MS_TIME_TYPE_MACRO */

#if defined(MBEDTLS_PLATFORM_NV_SEED_READ_MACRO)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_NV_SEED_READ_MACRO);
#endif /* MBEDTLS_PLATFORM_NV_SEED_READ_MACRO */

#if defined(MBEDTLS_PLATFORM_NV_SEED_WRITE_MACRO)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_NV_SEED_WRITE_MACRO);
#endif /* MBEDTLS_PLATFORM_NV_SEED_WRITE_MACRO */

#if defined(MBEDTLS_PLATFORM_PRINTF_MACRO)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_PRINTF_MACRO);
#endif /* MBEDTLS_PLATFORM_PRINTF_MACRO */

#if defined(MBEDTLS_PLATFORM_SETBUF_MACRO)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_SETBUF_MACRO);
#endif /* MBEDTLS_PLATFORM_SETBUF_MACRO */

#if defined(MBEDTLS_PLATFORM_SNPRINTF_MACRO)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_SNPRINTF_MACRO);
#endif /* MBEDTLS_PLATFORM_SNPRINTF_MACRO */

#if defined(MBEDTLS_PLATFORM_STD_CALLOC)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_STD_CALLOC);
#endif /* MBEDTLS_PLATFORM_STD_CALLOC */

#if defined(MBEDTLS_PLATFORM_STD_EXIT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_STD_EXIT);
#endif /* MBEDTLS_PLATFORM_STD_EXIT */

#if defined(MBEDTLS_PLATFORM_STD_EXIT_FAILURE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_STD_EXIT_FAILURE);
#endif /* MBEDTLS_PLATFORM_STD_EXIT_FAILURE */

#if defined(MBEDTLS_PLATFORM_STD_EXIT_SUCCESS)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_STD_EXIT_SUCCESS);
#endif /* MBEDTLS_PLATFORM_STD_EXIT_SUCCESS */

#if defined(MBEDTLS_PLATFORM_STD_FPRINTF)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_STD_FPRINTF);
#endif /* MBEDTLS_PLATFORM_STD_FPRINTF */

#if defined(MBEDTLS_PLATFORM_STD_FREE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_STD_FREE);
#endif /* MBEDTLS_PLATFORM_STD_FREE */

#if defined(MBEDTLS_PLATFORM_STD_MEM_HDR)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_STD_MEM_HDR);
#endif /* MBEDTLS_PLATFORM_STD_MEM_HDR */

#if defined(MBEDTLS_PLATFORM_STD_NV_SEED_FILE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_STD_NV_SEED_FILE);
#endif /* MBEDTLS_PLATFORM_STD_NV_SEED_FILE */

#if defined(MBEDTLS_PLATFORM_STD_NV_SEED_READ)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_STD_NV_SEED_READ);
#endif /* MBEDTLS_PLATFORM_STD_NV_SEED_READ */

#if defined(MBEDTLS_PLATFORM_STD_NV_SEED_WRITE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_STD_NV_SEED_WRITE);
#endif /* MBEDTLS_PLATFORM_STD_NV_SEED_WRITE */

#if defined(MBEDTLS_PLATFORM_STD_PRINTF)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_STD_PRINTF);
#endif /* MBEDTLS_PLATFORM_STD_PRINTF */

#if defined(MBEDTLS_PLATFORM_STD_SETBUF)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_STD_SETBUF);
#endif /* MBEDTLS_PLATFORM_STD_SETBUF */

#if defined(MBEDTLS_PLATFORM_STD_SNPRINTF)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_STD_SNPRINTF);
#endif /* MBEDTLS_PLATFORM_STD_SNPRINTF */

#if defined(MBEDTLS_PLATFORM_STD_TIME)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_STD_TIME);
#endif /* MBEDTLS_PLATFORM_STD_TIME */

#if defined(MBEDTLS_PLATFORM_TIME_MACRO)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_TIME_MACRO);
#endif /* MBEDTLS_PLATFORM_TIME_MACRO */

#if defined(MBEDTLS_PLATFORM_TIME_TYPE_MACRO)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_TIME_TYPE_MACRO);
#endif /* MBEDTLS_PLATFORM_TIME_TYPE_MACRO */

#if defined(MBEDTLS_PLATFORM_VSNPRINTF_MACRO)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_VSNPRINTF_MACRO);
#endif /* MBEDTLS_PLATFORM_VSNPRINTF_MACRO */

#if defined(MBEDTLS_PRINTF_MS_TIME)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PRINTF_MS_TIME);
#endif /* MBEDTLS_PRINTF_MS_TIME */

#if defined(MBEDTLS_PLATFORM_DEV_RANDOM)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PLATFORM_DEV_RANDOM);
#endif /* MBEDTLS_PLATFORM_DEV_RANDOM */

#if defined(MBEDTLS_CHECK_RETURN_WARNING)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_CHECK_RETURN_WARNING);
#endif /* MBEDTLS_CHECK_RETURN_WARNING */

#if defined(MBEDTLS_DEPRECATED_WARNING)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_DEPRECATED_WARNING);
#endif /* MBEDTLS_DEPRECATED_WARNING */

#if defined(MBEDTLS_DEPRECATED_REMOVED)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_DEPRECATED_REMOVED);
#endif /* MBEDTLS_DEPRECATED_REMOVED */

#if defined(MBEDTLS_CHECK_RETURN)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_CHECK_RETURN);
#endif /* MBEDTLS_CHECK_RETURN */

#if defined(MBEDTLS_IGNORE_RETURN)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_IGNORE_RETURN);
#endif /* MBEDTLS_IGNORE_RETURN */

#if defined(MBEDTLS_SELF_TEST)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SELF_TEST);
#endif /* MBEDTLS_SELF_TEST */

#if defined(MBEDTLS_TEST_CONSTANT_FLOW_MEMSAN)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_TEST_CONSTANT_FLOW_MEMSAN);
#endif /* MBEDTLS_TEST_CONSTANT_FLOW_MEMSAN */

#if defined(MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND);
#endif /* MBEDTLS_TEST_CONSTANT_FLOW_VALGRIND */

#if defined(MBEDTLS_TEST_HOOKS)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_TEST_HOOKS);
#endif /* MBEDTLS_TEST_HOOKS */

#if defined(MBEDTLS_LMS_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_LMS_C);
#endif /* MBEDTLS_LMS_C */

#if defined(MBEDTLS_LMS_PRIVATE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_LMS_PRIVATE);
#endif /* MBEDTLS_LMS_PRIVATE */

#if defined(MBEDTLS_MD_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_MD_C);
#endif /* MBEDTLS_MD_C */

#if defined(MBEDTLS_NIST_KW_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_NIST_KW_C);
#endif /* MBEDTLS_NIST_KW_C */

#if defined(MBEDTLS_PK_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PK_C);
#endif /* MBEDTLS_PK_C */

#if defined(MBEDTLS_PKCS5_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PKCS5_C);
#endif /* MBEDTLS_PKCS5_C */

#if defined(MBEDTLS_PK_PARSE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PK_PARSE_C);
#endif /* MBEDTLS_PK_PARSE_C */

#if defined(MBEDTLS_PK_PARSE_EC_EXTENDED)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PK_PARSE_EC_EXTENDED);
#endif /* MBEDTLS_PK_PARSE_EC_EXTENDED */

#if defined(MBEDTLS_PK_PARSE_EC_COMPRESSED)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PK_PARSE_EC_COMPRESSED);
#endif /* MBEDTLS_PK_PARSE_EC_COMPRESSED */

#if defined(MBEDTLS_PK_WRITE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PK_WRITE_C);
#endif /* MBEDTLS_PK_WRITE_C */

#if defined(MBEDTLS_ASN1_PARSE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_ASN1_PARSE_C);
#endif /* MBEDTLS_ASN1_PARSE_C */

#if defined(MBEDTLS_ASN1_WRITE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_ASN1_WRITE_C);
#endif /* MBEDTLS_ASN1_WRITE_C */

#if defined(MBEDTLS_BASE64_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_BASE64_C);
#endif /* MBEDTLS_BASE64_C */

#if defined(MBEDTLS_PEM_PARSE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PEM_PARSE_C);
#endif /* MBEDTLS_PEM_PARSE_C */

#if defined(MBEDTLS_PEM_WRITE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PEM_WRITE_C);
#endif /* MBEDTLS_PEM_WRITE_C */

#if defined(MBEDTLS_CTR_DRBG_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_CTR_DRBG_C);
#endif /* MBEDTLS_CTR_DRBG_C */

#if defined(MBEDTLS_ENTROPY_NO_SOURCES_OK)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_ENTROPY_NO_SOURCES_OK);
#endif /* MBEDTLS_ENTROPY_NO_SOURCES_OK */

#if defined(MBEDTLS_ENTROPY_NV_SEED)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_ENTROPY_NV_SEED);
#endif /* MBEDTLS_ENTROPY_NV_SEED */

#if defined(MBEDTLS_HMAC_DRBG_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_HMAC_DRBG_C);
#endif /* MBEDTLS_HMAC_DRBG_C */

#if defined(MBEDTLS_PSA_CRYPTO_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_CRYPTO_C);
#endif /* MBEDTLS_PSA_CRYPTO_C */

#if defined(MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS);
#endif /* MBEDTLS_PSA_ASSUME_EXCLUSIVE_BUFFERS */

#if defined(MBEDTLS_PSA_BUILTIN_GET_ENTROPY)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_BUILTIN_GET_ENTROPY);
#endif /* MBEDTLS_PSA_BUILTIN_GET_ENTROPY */

#if defined(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS);
#endif /* MBEDTLS_PSA_CRYPTO_BUILTIN_KEYS */

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_CRYPTO_CLIENT);
#endif /* MBEDTLS_PSA_CRYPTO_CLIENT */

#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG);
#endif /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */

#if defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER);
#endif /* MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER */

#if defined(MBEDTLS_PSA_CRYPTO_SPM)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_CRYPTO_SPM);
#endif /* MBEDTLS_PSA_CRYPTO_SPM */

#if defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_CRYPTO_STORAGE_C);
#endif /* MBEDTLS_PSA_CRYPTO_STORAGE_C */

#if defined(MBEDTLS_PSA_DRIVER_GET_ENTROPY)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_DRIVER_GET_ENTROPY);
#endif /* MBEDTLS_PSA_DRIVER_GET_ENTROPY */

#if defined(MBEDTLS_PSA_ITS_FILE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_ITS_FILE_C);
#endif /* MBEDTLS_PSA_ITS_FILE_C */

#if defined(MBEDTLS_PSA_KEY_STORE_DYNAMIC)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_KEY_STORE_DYNAMIC);
#endif /* MBEDTLS_PSA_KEY_STORE_DYNAMIC */

#if defined(MBEDTLS_PSA_STATIC_KEY_SLOTS)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_STATIC_KEY_SLOTS);
#endif /* MBEDTLS_PSA_STATIC_KEY_SLOTS */

#if defined(MBEDTLS_PSA_CRYPTO_PLATFORM_FILE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_CRYPTO_PLATFORM_FILE);
#endif /* MBEDTLS_PSA_CRYPTO_PLATFORM_FILE */

#if defined(MBEDTLS_PSA_CRYPTO_STRUCT_FILE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_CRYPTO_STRUCT_FILE);
#endif /* MBEDTLS_PSA_CRYPTO_STRUCT_FILE */

#if defined(MBEDTLS_PSA_KEY_SLOT_COUNT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_KEY_SLOT_COUNT);
#endif /* MBEDTLS_PSA_KEY_SLOT_COUNT */

#if defined(MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE);
#endif /* MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE */

#if defined(MBEDTLS_PSA_CRYPTO_RNG_STRENGTH)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_CRYPTO_RNG_STRENGTH);
#endif /* MBEDTLS_PSA_CRYPTO_RNG_STRENGTH */

#if defined(MBEDTLS_PSA_CRYPTO_RNG_HASH)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_CRYPTO_RNG_HASH);
#endif /* MBEDTLS_PSA_CRYPTO_RNG_HASH */

#if defined(MBEDTLS_PSA_RNG_RESEED_INTERVAL)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_RNG_RESEED_INTERVAL);
#endif /* MBEDTLS_PSA_RNG_RESEED_INTERVAL */

#if defined(MBEDTLS_AESNI_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_AESNI_C);
#endif /* MBEDTLS_AESNI_C */

#if defined(MBEDTLS_AESCE_C)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_AESCE_C);
#endif /* MBEDTLS_AESCE_C */

#if defined(MBEDTLS_AES_ROM_TABLES)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_AES_ROM_TABLES);
#endif /* MBEDTLS_AES_ROM_TABLES */

#if defined(MBEDTLS_AES_FEWER_TABLES)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_AES_FEWER_TABLES);
#endif /* MBEDTLS_AES_FEWER_TABLES */

#if defined(MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH);
#endif /* MBEDTLS_AES_ONLY_128_BIT_KEY_LENGTH */

#if defined(MBEDTLS_AES_USE_HARDWARE_ONLY)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_AES_USE_HARDWARE_ONLY);
#endif /* MBEDTLS_AES_USE_HARDWARE_ONLY */

#if defined(MBEDTLS_BLOCK_CIPHER_NO_DECRYPT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_BLOCK_CIPHER_NO_DECRYPT);
#endif /* MBEDTLS_BLOCK_CIPHER_NO_DECRYPT */

#if defined(MBEDTLS_CAMELLIA_SMALL_MEMORY)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_CAMELLIA_SMALL_MEMORY);
#endif /* MBEDTLS_CAMELLIA_SMALL_MEMORY */

#if defined(MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED);
#endif /* MBEDTLS_ECDH_VARIANT_EVEREST_ENABLED */

#if defined(MBEDTLS_ECP_NIST_OPTIM)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_ECP_NIST_OPTIM);
#endif /* MBEDTLS_ECP_NIST_OPTIM */

#if defined(MBEDTLS_ECP_RESTARTABLE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_ECP_RESTARTABLE);
#endif /* MBEDTLS_ECP_RESTARTABLE */

#if defined(MBEDTLS_ECP_WITH_MPI_UINT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_ECP_WITH_MPI_UINT);
#endif /* MBEDTLS_ECP_WITH_MPI_UINT */

#if defined(MBEDTLS_GCM_LARGE_TABLE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_GCM_LARGE_TABLE);
#endif /* MBEDTLS_GCM_LARGE_TABLE */

#if defined(MBEDTLS_HAVE_ASM)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_HAVE_ASM);
#endif /* MBEDTLS_HAVE_ASM */

#if defined(MBEDTLS_HAVE_SSE2)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_HAVE_SSE2);
#endif /* MBEDTLS_HAVE_SSE2 */

#if defined(MBEDTLS_NO_UDBL_DIVISION)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_NO_UDBL_DIVISION);
#endif /* MBEDTLS_NO_UDBL_DIVISION */

#if defined(MBEDTLS_NO_64BIT_MULTIPLICATION)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_NO_64BIT_MULTIPLICATION);
#endif /* MBEDTLS_NO_64BIT_MULTIPLICATION */

#if defined(MBEDTLS_PSA_P256M_DRIVER_ENABLED)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_PSA_P256M_DRIVER_ENABLED);
#endif /* MBEDTLS_PSA_P256M_DRIVER_ENABLED */

#if defined(MBEDTLS_RSA_NO_CRT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_RSA_NO_CRT);
#endif /* MBEDTLS_RSA_NO_CRT */

#if defined(MBEDTLS_SHA256_SMALLER)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SHA256_SMALLER);
#endif /* MBEDTLS_SHA256_SMALLER */

#if defined(MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT);
#endif /* MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_IF_PRESENT */

#if defined(MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY);
#endif /* MBEDTLS_SHA256_USE_ARMV8_A_CRYPTO_ONLY */

#if defined(MBEDTLS_SHA512_SMALLER)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SHA512_SMALLER);
#endif /* MBEDTLS_SHA512_SMALLER */

#if defined(MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT);
#endif /* MBEDTLS_SHA512_USE_A64_CRYPTO_IF_PRESENT */

#if defined(MBEDTLS_SHA512_USE_A64_CRYPTO_ONLY)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_SHA512_USE_A64_CRYPTO_ONLY);
#endif /* MBEDTLS_SHA512_USE_A64_CRYPTO_ONLY */

#if defined(MBEDTLS_ECP_FIXED_POINT_OPTIM)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_ECP_FIXED_POINT_OPTIM);
#endif /* MBEDTLS_ECP_FIXED_POINT_OPTIM */

#if defined(MBEDTLS_ECP_WINDOW_SIZE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_ECP_WINDOW_SIZE);
#endif /* MBEDTLS_ECP_WINDOW_SIZE */

#if defined(MBEDTLS_MPI_MAX_SIZE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_MPI_MAX_SIZE);
#endif /* MBEDTLS_MPI_MAX_SIZE */

#if defined(MBEDTLS_MPI_WINDOW_SIZE)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_MPI_WINDOW_SIZE);
#endif /* MBEDTLS_MPI_WINDOW_SIZE */

#if defined(MBEDTLS_RSA_GEN_KEY_MIN_BITS)
    OUTPUT_MACRO_NAME_VALUE(MBEDTLS_RSA_GEN_KEY_MIN_BITS);
#endif /* MBEDTLS_RSA_GEN_KEY_MIN_BITS */


}
#if defined(_MSC_VER)
#pragma warning(pop)
#endif /* _MSC_VER */
