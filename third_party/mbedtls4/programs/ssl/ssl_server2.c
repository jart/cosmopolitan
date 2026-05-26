/*
 *  SSL client with options
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "ssl_test_lib.h"

#if defined(MBEDTLS_SSL_TEST_IMPOSSIBLE)
int main(void)
{
    mbedtls_printf(MBEDTLS_SSL_TEST_IMPOSSIBLE);
    mbedtls_exit(0);
}
#elif !defined(MBEDTLS_SSL_SRV_C)
int main(void)
{
    mbedtls_printf("MBEDTLS_SSL_SRV_C not defined.\n");
    mbedtls_exit(0);
}
#else /* !MBEDTLS_SSL_TEST_IMPOSSIBLE && MBEDTLS_SSL_SRV_C */

#include <stdint.h>

#if !defined(_MSC_VER)
#include <inttypes.h>
#endif

#if !defined(_WIN32)
#include <signal.h>
#endif

#if defined(MBEDTLS_SSL_CACHE_C)
#include "mbedtls/ssl_cache.h"
#endif

#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_TICKET_C)
#include "mbedtls/ssl_ticket.h"
#endif

#if defined(MBEDTLS_SSL_COOKIE_C)
#include "mbedtls/ssl_cookie.h"
#endif

#if defined(MBEDTLS_SSL_SERVER_NAME_INDICATION) && defined(MBEDTLS_FS_IO)
#define SNI_OPTION
#endif

#if defined(_WIN32)
#include <windows.h>
#endif

#include "test/psa_crypto_helpers.h"

#include "mbedtls/pk.h"
#if defined(MBEDTLS_PK_HAVE_PRIVATE_HEADER)
#include <mbedtls/private/pk_private.h>
#endif /* MBEDTLS_PK_HAVE_PRIVATE_HEADER */

/* Size of memory to be allocated for the heap, when using the library's memory
 * management and MBEDTLS_MEMORY_BUFFER_ALLOC_C is enabled. */
#define MEMORY_HEAP_SIZE        180000

#define DFL_SERVER_ADDR         NULL
#define DFL_SERVER_PORT         "4433"
#define DFL_RESPONSE_SIZE       -1
#define DFL_DEBUG_LEVEL         0
#define DFL_NBIO                0
#define DFL_EVENT               0
#define DFL_READ_TIMEOUT        0
#define DFL_EXP_LABEL           NULL
#define DFL_EXP_LEN             20
#define DFL_CA_FILE             ""
#define DFL_CA_PATH             ""
#define DFL_CRT_FILE            ""
#define DFL_KEY_FILE            ""
#define DFL_KEY_OPAQUE          0
#define DFL_KEY_PWD             ""
#define DFL_CRT_FILE2           ""
#define DFL_KEY_FILE2           ""
#define DFL_KEY_PWD2            ""
#define DFL_ASYNC_OPERATIONS    "-"
#define DFL_ASYNC_PRIVATE_DELAY1 (-1)
#define DFL_ASYNC_PRIVATE_DELAY2 (-1)
#define DFL_ASYNC_PRIVATE_ERROR  (0)
#define DFL_PSK                 ""
#define DFL_PSK_OPAQUE          0
#define DFL_PSK_LIST_OPAQUE     0
#define DFL_PSK_IDENTITY        "Client_identity"
#define DFL_ECJPAKE_PW          NULL
#define DFL_ECJPAKE_PW_OPAQUE   0
#define DFL_PSK_LIST            NULL
#define DFL_FORCE_CIPHER        0
#define DFL_TLS1_3_KEX_MODES    MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_ALL
#define DFL_RENEGOTIATION       MBEDTLS_SSL_RENEGOTIATION_DISABLED
#define DFL_ALLOW_LEGACY        -2
#define DFL_RENEGOTIATE         0
#define DFL_RENEGO_DELAY        -2
#define DFL_RENEGO_PERIOD       ((uint64_t) -1)
#define DFL_EXCHANGES           1
#define DFL_MIN_VERSION         -1
#define DFL_MAX_VERSION         -1
#define DFL_SHA1                -1
#define DFL_CID_ENABLED         0
#define DFL_CID_VALUE           ""
#define DFL_CID_ENABLED_RENEGO  -1
#define DFL_CID_VALUE_RENEGO    NULL
#define DFL_AUTH_MODE           -1
#define DFL_CERT_REQ_CA_LIST    MBEDTLS_SSL_CERT_REQ_CA_LIST_ENABLED
#define DFL_CERT_REQ_DN_HINT    0
#define DFL_MFL_CODE            MBEDTLS_SSL_MAX_FRAG_LEN_NONE
#define DFL_TRUNC_HMAC          -1
#define DFL_TICKETS             MBEDTLS_SSL_SESSION_TICKETS_ENABLED
#define DFL_DUMMY_TICKET        0
#define DFL_TICKET_ROTATE       0
#define DFL_TICKET_TIMEOUT      86400
#define DFL_TICKET_ALG          PSA_ALG_GCM
#define DFL_TICKET_KEY_TYPE     PSA_KEY_TYPE_AES
#define DFL_TICKET_KEY_BITS     256
#define DFL_CACHE_MAX           -1
#define DFL_CACHE_TIMEOUT       -1
#define DFL_CACHE_REMOVE        0
#define DFL_SNI                 NULL
#define DFL_ALPN_STRING         NULL
#define DFL_GROUPS              NULL
#define DFL_EARLY_DATA          -1
#define DFL_MAX_EARLY_DATA_SIZE ((uint32_t) -1)
#define DFL_SIG_ALGS            NULL
#define DFL_TRANSPORT           MBEDTLS_SSL_TRANSPORT_STREAM
#define DFL_COOKIES             1
#define DFL_ANTI_REPLAY         -1
#define DFL_HS_TO_MIN           0
#define DFL_HS_TO_MAX           0
#define DFL_DTLS_MTU            -1
#define DFL_BADMAC_LIMIT        -1
#define DFL_DGRAM_PACKING        1
#define DFL_EXTENDED_MS         -1
#define DFL_ETM                 -1
#define DFL_SERIALIZE           0
#define DFL_CONTEXT_FILE        ""
#define DFL_EXTENDED_MS_ENFORCE -1
#define DFL_CA_CALLBACK         0
#define DFL_EAP_TLS             0
#define DFL_REPRODUCIBLE        0
#define DFL_NSS_KEYLOG          0
#define DFL_NSS_KEYLOG_FILE     NULL
#define DFL_QUERY_CONFIG_MODE   0
#define DFL_USE_SRTP            0
#define DFL_SRTP_FORCE_PROFILE  0
#define DFL_SRTP_SUPPORT_MKI    0
#define DFL_KEY_OPAQUE_ALG      "none"

#define LONG_RESPONSE "<p>01-blah-blah-blah-blah-blah-blah-blah-blah-blah\r\n" \
                      "02-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah\r\n"  \
                      "03-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah\r\n"  \
                      "04-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah\r\n"  \
                      "05-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah\r\n"  \
                      "06-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah\r\n"  \
                      "07-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah-blah</p>\r\n"

/* Uncomment LONG_RESPONSE at the end of HTTP_RESPONSE to test sending longer
 * packets (for fragmentation purposes) */
#define HTTP_RESPONSE \
    "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n" \
    "<h2>Mbed TLS Test Server</h2>\r\n" \
    "<p>Successful connection using: %s</p>\r\n" // LONG_RESPONSE

/*
 * Size of the basic I/O buffer. Able to hold our default response.
 */
#define DFL_IO_BUF_LEN      200

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
#if defined(MBEDTLS_FS_IO)
#define USAGE_IO \
    "    ca_file=%%s          The single file containing the top-level CA(s) you fully trust\n" \
    "                        default: \"\" (pre-loaded)\n" \
    "                        use \"none\" to skip loading any top-level CAs.\n" \
    "    ca_path=%%s          The path containing the top-level CA(s) you fully trust\n" \
    "                        default: \"\" (pre-loaded) (overrides ca_file)\n" \
    "                        use \"none\" to skip loading any top-level CAs.\n" \
    "    crt_file=%%s         Your own cert and chain (in bottom to top order, top may be omitted)\n" \
    "                        default: see note after key_file2\n" \
    "    key_file=%%s         default: see note after key_file2\n" \
    "    key_pwd=%%s          Password for key specified by key_file argument\n" \
    "                        default: none\n" \
    "    crt_file2=%%s        Your second cert and chain (in bottom to top order, top may be omitted)\n" \
    "                        default: see note after key_file2\n" \
    "    key_file2=%%s        default: see note below\n" \
    "                        note: if neither crt_file/key_file nor crt_file2/key_file2 are used,\n" \
    "                              preloaded certificate(s) and key(s) are used if available\n" \
    "    key_pwd2=%%s         Password for key specified by key_file2 argument\n" \
    "                        default: none\n"
#else
#define USAGE_IO \
    "\n"                                                    \
    "    No file operations available (MBEDTLS_FS_IO not defined)\n" \
    "\n"
#endif /* MBEDTLS_FS_IO */
#else
#define USAGE_IO ""
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */
#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
#define USAGE_KEY_OPAQUE \
    "    key_opaque=%%d       Handle your private keys as if they were opaque\n" \
    "                        default: 0 (disabled)\n"
#else
#define USAGE_KEY_OPAQUE ""
#endif

#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
#define USAGE_SSL_ASYNC \
    "    async_operations=%%c...   s=sign (default: -=off)\n" \
    "    async_private_delay1=%%d  Asynchronous delay for key_file or preloaded key\n" \
    "    async_private_delay2=%%d  Asynchronous delay for key_file2 and sni\n" \
    "                              default: -1 (not asynchronous)\n" \
    "    async_private_error=%%d   Async callback error injection (default=0=none,\n" \
    "                              1=start, 2=cancel, 3=resume, negative=first time only)"
#else
#define USAGE_SSL_ASYNC ""
#endif /* MBEDTLS_SSL_ASYNC_PRIVATE */

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
#define USAGE_CID \
    "    cid=%%d             Disable (0) or enable (1) the use of the DTLS Connection ID extension.\n" \
    "                       default: 0 (disabled)\n"     \
    "    cid_renego=%%d      Disable (0) or enable (1) the use of the DTLS Connection ID extension during renegotiation.\n" \
    "                       default: same as 'cid' parameter\n"     \
    "    cid_val=%%s          The CID to use for incoming messages (in hex, without 0x).\n"  \
    "                        default: \"\"\n" \
    "    cid_val_renego=%%s   The CID to use for incoming messages (in hex, without 0x) after renegotiation.\n"  \
    "                        default: same as 'cid_val' parameter\n"
#else /* MBEDTLS_SSL_DTLS_CONNECTION_ID */
#define USAGE_CID ""
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED)
#define USAGE_PSK_RAW                                               \
    "    psk=%%s              default: \"\" (disabled)\n"     \
    "                          The PSK values are in hex, without 0x.\n" \
    "    psk_list=%%s         default: \"\"\n"                          \
    "                          A list of (PSK identity, PSK value) pairs.\n" \
    "                          The PSK values are in hex, without 0x.\n" \
    "                          id1,psk1[,id2,psk2[,...]]\n"             \
    "    psk_identity=%%s     default: \"Client_identity\"\n"
#define USAGE_PSK_SLOT                          \
    "    psk_opaque=%%d       default: 0 (don't use opaque static PSK)\n"     \
    "                          Enable this to store the PSK configured through command line\n" \
    "                          parameter `psk` in a PSA-based key slot.\n" \
    "                          Note: Currently only supported in conjunction with\n"                  \
    "                          the use of min_version to force TLS 1.2 and force_ciphersuite \n"      \
    "                          to force a particular PSK-only ciphersuite.\n"                         \
    "                          Note: This is to test integration of PSA-based opaque PSKs with\n"     \
    "                          Mbed TLS only. Production systems are likely to configure Mbed TLS\n"  \
    "                          with prepopulated key slots instead of importing raw key material.\n" \
    "    psk_list_opaque=%%d  default: 0 (don't use opaque dynamic PSKs)\n"     \
    "                          Enable this to store the list of dynamically chosen PSKs configured\n" \
    "                          through the command line parameter `psk_list` in PSA-based key slots.\n" \
    "                          Note: Currently only supported in conjunction with\n" \
    "                          the use of min_version to force TLS 1.2 and force_ciphersuite \n" \
    "                          to force a particular PSK-only ciphersuite.\n" \
    "                          Note: This is to test integration of PSA-based opaque PSKs with\n" \
    "                          Mbed TLS only. Production systems are likely to configure Mbed TLS\n" \
    "                          with prepopulated key slots instead of importing raw key material.\n"
#define USAGE_PSK USAGE_PSK_RAW USAGE_PSK_SLOT
#else
#define USAGE_PSK ""
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED */

#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
#define USAGE_CA_CALLBACK                       \
    "   ca_callback=%%d       default: 0 (disabled)\n"      \
    "                         Enable this to use the trusted certificate callback function\n"
#else
#define USAGE_CA_CALLBACK ""
#endif /* MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK */

#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_TICKET_C)
#define USAGE_TICKETS                                       \
    "    tickets=%%d          default: 1 (enabled)\n"       \
    "    ticket_rotate=%%d    default: 0 (disabled)\n"      \
    "    ticket_timeout=%%d   default: 86400 (one day)\n"   \
    "    ticket_aead=%%s      default: \"AES-256-GCM\"\n"
#else /* MBEDTLS_SSL_SESSION_TICKETS && MBEDTLS_SSL_TICKET_C */
#define USAGE_TICKETS ""
#endif /* MBEDTLS_SSL_SESSION_TICKETS && MBEDTLS_SSL_TICKET_C */

#define USAGE_EAP_TLS                                       \
    "    eap_tls=%%d          default: 0 (disabled)\n"
#define USAGE_NSS_KEYLOG                                    \
    "    nss_keylog=%%d          default: 0 (disabled)\n"   \
    "                             This cannot be used with eap_tls=1\n"
#define USAGE_NSS_KEYLOG_FILE                               \
    "    nss_keylog_file=%%s\n"
#if defined(MBEDTLS_SSL_DTLS_SRTP)
#define USAGE_SRTP \
    "    use_srtp=%%d         default: 0 (disabled)\n" \
    "    srtp_force_profile=%%d  default: 0 (all enabled)\n"   \
    "                        available profiles:\n"       \
    "                        1 - SRTP_AES128_CM_HMAC_SHA1_80\n"  \
    "                        2 - SRTP_AES128_CM_HMAC_SHA1_32\n"  \
    "                        3 - SRTP_NULL_HMAC_SHA1_80\n"       \
    "                        4 - SRTP_NULL_HMAC_SHA1_32\n"       \
    "    support_mki=%%d     default: 0 (not supported)\n"
#else /* MBEDTLS_SSL_DTLS_SRTP */
#define USAGE_SRTP ""
#endif

#if defined(MBEDTLS_SSL_CACHE_C)
#define USAGE_CACHE                                             \
    "    cache_max=%%d        default: cache default (50)\n"    \
    "    cache_remove=%%d     default: 0 (don't remove)\n"
#if defined(MBEDTLS_HAVE_TIME)
#define USAGE_CACHE_TIME \
    "    cache_timeout=%%d    default: cache default (1d)\n"
#else
#define USAGE_CACHE_TIME ""
#endif
#else
#define USAGE_CACHE ""
#define USAGE_CACHE_TIME ""
#endif /* MBEDTLS_SSL_CACHE_C */

#if defined(SNI_OPTION)
#if defined(MBEDTLS_X509_CRL_PARSE_C)
#define SNI_CRL              ",crl"
#else
#define SNI_CRL              ""
#endif

#define USAGE_SNI                                                           \
    "    sni=%%s              name1,cert1,key1,ca1"SNI_CRL ",auth1[,...]\n"  \
                                                           "                        default: disabled\n"
#else
#define USAGE_SNI ""
#endif /* SNI_OPTION */

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
#define USAGE_MAX_FRAG_LEN                                      \
    "    max_frag_len=%%d     default: 16384 (tls default)\n"   \
    "                        options: 512, 1024, 2048, 4096\n"
#else
#define USAGE_MAX_FRAG_LEN ""
#endif /* MBEDTLS_SSL_MAX_FRAGMENT_LENGTH */

#if defined(MBEDTLS_SSL_ALPN)
#define USAGE_ALPN \
    "    alpn=%%s             default: \"\" (disabled)\n"   \
    "                        example: spdy/1,http/1.1\n"
#else
#define USAGE_ALPN ""
#endif /* MBEDTLS_SSL_ALPN */

#if defined(MBEDTLS_SSL_DTLS_HELLO_VERIFY)
#define USAGE_COOKIES \
    "    cookies=0/1/-1      default: 1 (enabled)\n"        \
    "                        0: disabled, -1: library default (broken)\n"
#else
#define USAGE_COOKIES ""
#endif

#if defined(MBEDTLS_SSL_DTLS_ANTI_REPLAY)
#define USAGE_ANTI_REPLAY \
    "    anti_replay=0/1     default: (library default: enabled)\n"
#else
#define USAGE_ANTI_REPLAY ""
#endif

#define USAGE_BADMAC_LIMIT \
    "    badmac_limit=%%d     default: (library default: disabled)\n"

#if defined(MBEDTLS_SSL_PROTO_DTLS)
#define USAGE_DTLS \
    "    dtls=%%d             default: 0 (TLS)\n"                           \
    "    hs_timeout=%%d-%%d    default: (library default: 1000-60000)\n"    \
    "                        range of DTLS handshake timeouts in millisecs\n" \
    "    mtu=%%d              default: (library default: unlimited)\n"  \
    "    dgram_packing=%%d    default: 1 (allowed)\n"                   \
    "                        allow or forbid packing of multiple\n" \
    "                        records within a single datgram.\n"
#else
#define USAGE_DTLS ""
#endif

#if defined(MBEDTLS_SSL_EXTENDED_MASTER_SECRET)
#define USAGE_EMS \
    "    extended_ms=0/1     default: (library default: on)\n"
#else
#define USAGE_EMS ""
#endif

#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
#define USAGE_ETM \
    "    etm=0/1             default: (library default: on)\n"
#else
#define USAGE_ETM ""
#endif

#define USAGE_REPRODUCIBLE \
    "    reproducible=0/1     default: 0 (disabled)\n"

#if defined(MBEDTLS_SSL_RENEGOTIATION)
#define USAGE_RENEGO \
    "    renegotiation=%%d    default: 0 (disabled)\n"      \
    "    renegotiate=%%d      default: 0 (disabled)\n"      \
    "    renego_delay=%%d     default: -2 (library default)\n" \
    "    renego_period=%%d    default: (2^64 - 1 for TLS, 2^48 - 1 for DTLS)\n"
#else
#define USAGE_RENEGO ""
#endif

#if defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
#define USAGE_ECJPAKE \
    "    ecjpake_pw=%%s           default: none (disabled)\n"   \
    "    ecjpake_pw_opaque=%%d    default: 0 (disabled)\n"
#else /* MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED */
#define USAGE_ECJPAKE ""
#endif /* MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED */

#if defined(MBEDTLS_SSL_EARLY_DATA)
#define USAGE_EARLY_DATA \
    "    early_data=%%d      default: library default\n" \
    "                        options: 0 (disabled), 1 (enabled)\n" \
    "    max_early_data_size=%%d default: library default\n" \
    "                            options: max amount of early data\n"
#else
#define USAGE_EARLY_DATA ""
#endif /* MBEDTLS_SSL_EARLY_DATA */

#if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) || \
    (defined(MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_SOME_EPHEMERAL_ENABLED) && \
    defined(PSA_WANT_ALG_FFDH))
#define USAGE_GROUPS \
    "    groups=a,b,c,d      default: \"default\" (library default)\n"  \
    "                        example: \"secp521r1,brainpoolP512r1\"\n"  \
    "                        - use \"none\" for empty list\n"           \
    "                        - see mbedtls_ecp_curve_list()\n"                \
    "                          for acceptable EC group names\n"               \
    "                        - the following ffdh groups are supported:\n"    \
    "                          ffdhe2048, ffdhe3072, ffdhe4096, ffdhe6144,\n" \
    "                          ffdhe8192\n"
#else
#define USAGE_GROUPS ""
#endif

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
#define USAGE_SIG_ALGS \
    "    sig_algs=a,b,c,d      default: \"default\" (library default)\n"  \
    "                          example: \"ecdsa_secp256r1_sha256,ecdsa_secp384r1_sha384\"\n"
#else
#define USAGE_SIG_ALGS ""
#endif

#if defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION)
#define USAGE_SERIALIZATION \
    "    serialize=%%d        default: 0 (do not serialize/deserialize)\n"     \
    "                        options: 1 (serialize)\n"                         \
    "                                 2 (serialize with re-initialization)\n"  \
    "    context_file=%%s     The file path to write a serialized connection\n" \
    "                        in the form of base64 code (serialize option\n"   \
    "                        must be set)\n"                                   \
    "                         default: \"\" (do nothing)\n"                    \
    "                         option: a file path\n"
#else
#define USAGE_SERIALIZATION ""
#endif

#if defined(MBEDTLS_SSL_KEYING_MATERIAL_EXPORT)
#define USAGE_EXPORT \
    "    exp_label=%%s       Label to input into TLS-Exporter\n" \
    "                         default: None (don't try to export a key)\n" \
    "    exp_len=%%d         Length of key to extract from TLS-Exporter \n" \
    "                         default: 20\n"
#else
#define USAGE_EXPORT ""
#endif

#define USAGE_KEY_OPAQUE_ALGS \
    "    key_opaque_algs=%%s  Allowed opaque key 1 algorithms.\n"                      \
    "                        comma-separated pair of values among the following:\n"    \
    "                        rsa-sign-pkcs1, rsa-sign-pss, rsa-sign-pss-sha256,\n"     \
    "                        rsa-sign-pss-sha384, rsa-sign-pss-sha512,\n"              \
    "                        ecdsa-sign, ecdh, none (only acceptable for\n"            \
    "                        the second value).\n"                                     \
    "    key_opaque_algs2=%%s Allowed opaque key 2 algorithms.\n"                      \
    "                        comma-separated pair of values among the following:\n"    \
    "                        rsa-sign-pkcs1, rsa-sign-pss, rsa-sign-pss-sha256,\n"     \
    "                        rsa-sign-pss-sha384, rsa-sign-pss-sha512,\n"              \
    "                        ecdsa-sign, ecdh, none (only acceptable for\n"            \
    "                        the second value).\n"
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
#define USAGE_TLS1_3_KEY_EXCHANGE_MODES \
    "    tls13_kex_modes=%%s   default: all\n"     \
    "                          options: psk, psk_ephemeral, psk_all, ephemeral,\n"  \
    "                                   ephemeral_all, all, psk_or_ephemeral\n"
#else
#define USAGE_TLS1_3_KEY_EXCHANGE_MODES ""
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */


/* USAGE is arbitrarily split to stay under the portable string literal
 * length limit: 4095 bytes in C99. */
#define USAGE1 \
    "\n usage: ssl_server2 param=<>...\n"                   \
    "\n acceptable parameters:\n"                           \
    "    server_addr=%%s      default: (all interfaces)\n"  \
    "    server_port=%%d      default: 4433\n"              \
    "    debug_level=%%d      default: 0 (disabled)\n"      \
    "    build_version=%%d    default: none (disabled)\n"                     \
    "                        option: 1 (print build version only and stop)\n" \
    "    buffer_size=%%d      default: 200 \n" \
    "                         (minimum: 1)\n" \
    "    response_size=%%d    default: about 152 (basic response)\n" \
    "                          (minimum: 0, max: 16384)\n" \
    "                          increases buffer_size if bigger\n" \
    "    nbio=%%d             default: 0 (blocking I/O)\n"  \
    "                        options: 1 (non-blocking), 2 (added delays)\n" \
    "    event=%%d            default: 0 (loop)\n"                            \
    "                        options: 1 (level-triggered, implies nbio=1),\n" \
    "    read_timeout=%%d     default: 0 ms (no timeout)\n"    \
    "\n"                                                    \
    USAGE_DTLS                                              \
    USAGE_SRTP                                              \
    USAGE_COOKIES                                           \
    USAGE_ANTI_REPLAY                                       \
    USAGE_BADMAC_LIMIT                                      \
    "\n"
#define USAGE2 \
    "    auth_mode=%%s        default: (library default: none)\n"      \
    "                        options: none, optional, required\n" \
    "    cert_req_ca_list=%%d default: 1 (send ca list)\n"  \
    "                        options: 1 (send ca list), 0 (don't send)\n" \
    "                                 2 (send conf dn hint), 3 (send hs dn hint)\n" \
    USAGE_IO                                                \
    USAGE_KEY_OPAQUE                                        \
    "\n"                                                    \
    USAGE_PSK                                               \
    USAGE_CA_CALLBACK                                       \
    USAGE_ECJPAKE                                           \
    "\n"
#define USAGE3 \
    "    allow_legacy=%%d     default: (library default: no)\n"      \
    USAGE_RENEGO                                            \
    "    exchanges=%%d        default: 1\n"                 \
    "\n"                                                    \
    USAGE_TICKETS                                           \
    USAGE_EAP_TLS                                           \
    USAGE_REPRODUCIBLE                                      \
    USAGE_NSS_KEYLOG                                        \
    USAGE_NSS_KEYLOG_FILE                                   \
    USAGE_CACHE                                             \
    USAGE_CACHE_TIME                                        \
    USAGE_MAX_FRAG_LEN                                      \
    USAGE_ALPN                                              \
    USAGE_EMS                                               \
    USAGE_ETM                                               \
    USAGE_GROUPS                                            \
    USAGE_SIG_ALGS                                          \
    USAGE_KEY_OPAQUE_ALGS                                   \
    USAGE_EARLY_DATA                                        \
    "\n"

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
#define TLS1_3_VERSION_OPTIONS  ", tls13"
#else /* MBEDTLS_SSL_PROTO_TLS1_3 */
#define TLS1_3_VERSION_OPTIONS  ""
#endif /* !MBEDTLS_SSL_PROTO_TLS1_3 */

#define USAGE4 \
    USAGE_SSL_ASYNC                                         \
    USAGE_SNI                                               \
    "    allow_sha1=%%d       default: 0\n"                                   \
    "    min_version=%%s      default: (library default: tls12)\n"            \
    "    max_version=%%s      default: (library default: tls12)\n"            \
    "    force_version=%%s    default: \"\" (none)\n"                         \
    "                         options: tls12, dtls12" TLS1_3_VERSION_OPTIONS  \
    "\n\n"                                                                    \
    "    force_ciphersuite=<name>    default: all enabled\n"                  \
    USAGE_TLS1_3_KEY_EXCHANGE_MODES                                           \
    "    query_config=<name>         return 0 if the specified\n"             \
    "                                configuration macro is defined and 1\n"  \
    "                                otherwise. The expansion of the macro\n" \
    "                                is printed if it is defined\n"           \
    USAGE_SERIALIZATION                                                       \
    USAGE_EXPORT                                                              \
    "\n"

#define PUT_UINT64_BE(out_be, in_le, i)                                   \
    {                                                                       \
        (out_be)[(i) + 0] = (unsigned char) (((in_le) >> 56) & 0xFF);    \
        (out_be)[(i) + 1] = (unsigned char) (((in_le) >> 48) & 0xFF);    \
        (out_be)[(i) + 2] = (unsigned char) (((in_le) >> 40) & 0xFF);    \
        (out_be)[(i) + 3] = (unsigned char) (((in_le) >> 32) & 0xFF);    \
        (out_be)[(i) + 4] = (unsigned char) (((in_le) >> 24) & 0xFF);    \
        (out_be)[(i) + 5] = (unsigned char) (((in_le) >> 16) & 0xFF);    \
        (out_be)[(i) + 6] = (unsigned char) (((in_le) >> 8) & 0xFF);    \
        (out_be)[(i) + 7] = (unsigned char) (((in_le) >> 0) & 0xFF);    \
    }

/*
 * global options
 */
struct options {
    const char *server_addr;    /* address on which the ssl service runs    */
    const char *server_port;    /* port on which the ssl service runs       */
    int debug_level;            /* level of debugging                       */
    int nbio;                   /* should I/O be blocking?                  */
    int event;                  /* loop or event-driven IO? level or edge triggered? */
    uint32_t read_timeout;      /* timeout on mbedtls_ssl_read() in milliseconds    */
    const char *exp_label;      /* label to input into mbedtls_ssl_export_keying_material() */
    int exp_len;                /* Length of key to export using mbedtls_ssl_export_keying_material() */
    int response_size;          /* pad response with header to requested size */
    uint16_t buffer_size;       /* IO buffer size */
    const char *ca_file;        /* the file with the CA certificate(s)      */
    const char *ca_path;        /* the path with the CA certificate(s) reside */
    const char *crt_file;       /* the file with the server certificate     */
    const char *key_file;       /* the file with the server key             */
    int key_opaque;             /* handle private key as if it were opaque  */
    const char *key_pwd;        /* the password for the server key          */
    const char *crt_file2;      /* the file with the 2nd server certificate */
    const char *key_file2;      /* the file with the 2nd server key         */
    const char *key_pwd2;       /* the password for the 2nd server key      */
    const char *async_operations; /* supported SSL asynchronous operations  */
    int async_private_delay1;   /* number of times f_async_resume needs to be called for key 1, or -1 for no async */
    int async_private_delay2;   /* number of times f_async_resume needs to be called for key 2, or -1 for no async */
    int async_private_error;    /* inject error in async private callback */
    int psk_opaque;
    int psk_list_opaque;
#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
    int ca_callback;            /* Use callback for trusted certificate list */
#endif
    const char *psk;            /* the pre-shared key                       */
    const char *psk_identity;   /* the pre-shared key identity              */
    char *psk_list;             /* list of PSK id/key pairs for callback    */
    const char *ecjpake_pw;     /* the EC J-PAKE password                   */
    int ecjpake_pw_opaque;      /* set to 1 to use the opaque method for setting the password */
    int force_ciphersuite[2];   /* protocol/ciphersuite to use, or all      */
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    int tls13_kex_modes;        /* supported TLS 1.3 key exchange modes     */
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */
    int renegotiation;          /* enable / disable renegotiation           */
    int allow_legacy;           /* allow legacy renegotiation               */
    int renegotiate;            /* attempt renegotiation?                   */
    int renego_delay;           /* delay before enforcing renegotiation     */
    uint64_t renego_period;     /* period for automatic renegotiation       */
    int exchanges;              /* number of data exchanges                 */
    int min_version;            /* minimum protocol version accepted        */
    int max_version;            /* maximum protocol version accepted        */
    int allow_sha1;             /* flag for SHA-1 support                   */
    int auth_mode;              /* verify mode for connection               */
    int cert_req_ca_list;       /* should we send the CA list?              */
    int cert_req_dn_hint;       /* mode to set DN hints for CA list to send */
    unsigned char mfl_code;     /* code for maximum fragment length         */
    int trunc_hmac;             /* accept truncated hmac?                   */
    int tickets;                /* enable / disable session tickets         */
    int dummy_ticket;           /* enable / disable dummy ticket generator  */
    int ticket_rotate;          /* session ticket rotate (code coverage)    */
    int ticket_timeout;         /* session ticket lifetime                  */
    int ticket_alg;             /* session ticket algorithm                 */
    int ticket_key_type;        /* session ticket key type                  */
    int ticket_key_bits;        /* session ticket key size in bits          */
    int cache_max;              /* max number of session cache entries      */
#if defined(MBEDTLS_HAVE_TIME)
    int cache_timeout;          /* expiration delay of session cache entries*/
#endif
    int cache_remove;           /* enable / disable cache entry removal     */
    char *sni;                  /* string describing sni information        */
    const char *groups;         /* list of supported groups                 */
    const char *sig_algs;       /* supported TLS 1.3 signature algorithms   */
    const char *alpn_string;    /* ALPN supported protocols                 */
    int extended_ms;            /* allow negotiation of extended MS?        */
    int etm;                    /* allow negotiation of encrypt-then-MAC?   */
    int transport;              /* TLS or DTLS?                             */
    int cookies;                /* Use cookies for DTLS? -1 to break them   */
    int anti_replay;            /* Use anti-replay for DTLS? -1 for default */
    uint32_t hs_to_min;         /* Initial value of DTLS handshake timer    */
    uint32_t hs_to_max;         /* Max value of DTLS handshake timer        */
    int dtls_mtu;               /* UDP Maximum transport unit for DTLS       */
    int dgram_packing;          /* allow/forbid datagram packing            */
    int badmac_limit;           /* Limit of records with bad MAC            */
    int eap_tls;                /* derive EAP-TLS keying material?          */
    int nss_keylog;             /* export NSS key log material              */
    const char *nss_keylog_file; /* NSS key log file                        */
    int cid_enabled;            /* whether to use the CID extension or not  */
    int cid_enabled_renego;     /* whether to use the CID extension or not
                                 * during renegotiation                     */
    const char *cid_val;        /* the CID to use for incoming messages     */
    int serialize;              /* serialize/deserialize connection         */
    const char *context_file;   /* the file to write a serialized connection
                                 * in the form of base64 code (serialize
                                 * option must be set)                      */
    const char *cid_val_renego; /* the CID to use for incoming messages
                                 * after renegotiation                      */
    int reproducible;           /* make communication reproducible          */
#if defined(MBEDTLS_SSL_EARLY_DATA)
    int early_data;               /* early data enablement flag             */
    uint32_t max_early_data_size; /* max amount of early data               */
#endif
    int query_config_mode;      /* whether to read config                   */
    int use_srtp;               /* Support SRTP                             */
    int force_srtp_profile;     /* SRTP protection profile to use or all    */
    int support_mki;            /* The dtls mki mki support                 */
    const char *key1_opaque_alg1; /* Allowed opaque key 1 alg 1            */
    const char *key1_opaque_alg2; /* Allowed opaque key 1 alg 2            */
    const char *key2_opaque_alg1; /* Allowed opaque key 2 alg 1            */
    const char *key2_opaque_alg2; /* Allowed opaque key 2 alg 2            */
} opt;

#include "ssl_test_common_source.c"

/*
 * Return authmode from string, or -1 on error
 */
static int get_auth_mode(const char *s)
{
    if (strcmp(s, "none") == 0) {
        return MBEDTLS_SSL_VERIFY_NONE;
    }
    if (strcmp(s, "optional") == 0) {
        return MBEDTLS_SSL_VERIFY_OPTIONAL;
    }
    if (strcmp(s, "required") == 0) {
        return MBEDTLS_SSL_VERIFY_REQUIRED;
    }

    return -1;
}

/*
 * Used by sni_parse and psk_parse to handle comma-separated lists
 */
#define GET_ITEM(dst)         \
    do                          \
    {                           \
        (dst) = p;              \
        while (*p != ',')      \
        if (++p > end)     \
        goto error;     \
        *p++ = '\0';            \
    } while (0)

#if defined(SNI_OPTION)
typedef struct _sni_entry sni_entry;

struct _sni_entry {
    const char *name;
    mbedtls_x509_crt *cert;
    mbedtls_pk_context *key;
    mbedtls_x509_crt *ca;
    mbedtls_x509_crl *crl;
    int authmode;
    sni_entry *next;
};

static void sni_free(sni_entry *head)
{
    sni_entry *cur = head, *next;

    while (cur != NULL) {
        mbedtls_x509_crt_free(cur->cert);
        mbedtls_free(cur->cert);

        mbedtls_pk_free(cur->key);
        mbedtls_free(cur->key);

        mbedtls_x509_crt_free(cur->ca);
        mbedtls_free(cur->ca);
#if defined(MBEDTLS_X509_CRL_PARSE_C)
        mbedtls_x509_crl_free(cur->crl);
        mbedtls_free(cur->crl);
#endif
        next = cur->next;
        mbedtls_free(cur);
        cur = next;
    }
}

/*
 * Parse a string of sextuples name1,crt1,key1,ca1,crl1,auth1[,...]
 * into a usable sni_entry list. For ca1, crl1, auth1, the special value
 * '-' means unset. If ca1 is unset, then crl1 is ignored too.
 *
 * Modifies the input string! This is not production quality!
 */
static sni_entry *sni_parse(char *sni_string)
{
    sni_entry *cur = NULL, *new = NULL;
    char *p = sni_string;
    char *end = p;
    char *crt_file, *key_file, *ca_file, *auth_str;
#if defined(MBEDTLS_X509_CRL_PARSE_C)
    char *crl_file;
#endif

    while (*end != '\0') {
        ++end;
    }
    *end = ',';

    while (p <= end) {
        if ((new = mbedtls_calloc(1, sizeof(sni_entry))) == NULL) {
            sni_free(cur);
            return NULL;
        }

        GET_ITEM(new->name);
        GET_ITEM(crt_file);
        GET_ITEM(key_file);
        GET_ITEM(ca_file);
#if defined(MBEDTLS_X509_CRL_PARSE_C)
        GET_ITEM(crl_file);
#endif
        GET_ITEM(auth_str);

        if ((new->cert = mbedtls_calloc(1, sizeof(mbedtls_x509_crt))) == NULL ||
            (new->key = mbedtls_calloc(1, sizeof(mbedtls_pk_context))) == NULL) {
            goto error;
        }

        mbedtls_x509_crt_init(new->cert);
        mbedtls_pk_init(new->key);

        if (mbedtls_x509_crt_parse_file(new->cert, crt_file) != 0 ||
            mbedtls_pk_parse_keyfile(new->key, key_file, "") != 0) {
            goto error;
        }

        if (strcmp(ca_file, "-") != 0) {
            if ((new->ca = mbedtls_calloc(1, sizeof(mbedtls_x509_crt))) == NULL) {
                goto error;
            }

            mbedtls_x509_crt_init(new->ca);

            if (mbedtls_x509_crt_parse_file(new->ca, ca_file) != 0) {
                goto error;
            }
        }

#if defined(MBEDTLS_X509_CRL_PARSE_C)
        if (strcmp(crl_file, "-") != 0) {
            if ((new->crl = mbedtls_calloc(1, sizeof(mbedtls_x509_crl))) == NULL) {
                goto error;
            }

            mbedtls_x509_crl_init(new->crl);

            if (mbedtls_x509_crl_parse_file(new->crl, crl_file) != 0) {
                goto error;
            }
        }
#endif

        if (strcmp(auth_str, "-") != 0) {
            if ((new->authmode = get_auth_mode(auth_str)) < 0) {
                goto error;
            }
        } else {
            new->authmode = DFL_AUTH_MODE;
        }

        new->next = cur;
        cur = new;
    }

    return cur;

error:
    sni_free(new);
    sni_free(cur);
    return NULL;
}

/*
 * SNI callback.
 */
static int sni_callback(void *p_info, mbedtls_ssl_context *ssl,
                        const unsigned char *name, size_t name_len)
{
    const sni_entry *cur = (const sni_entry *) p_info;

    /* preserve behavior which checks for SNI match in sni_callback() for
     * the benefits of tests using sni_callback(), even though the actual
     * certificate assignment has moved to certificate selection callback
     * in this application.  This exercises sni_callback and cert_callback
     * even though real applications might choose to do this differently.
     * Application might choose to save name and name_len in user_data for
     * later use in certificate selection callback.
     */
    while (cur != NULL) {
        if (name_len == strlen(cur->name) &&
            memcmp(name, cur->name, name_len) == 0) {
            void *p;
            *(const void **)&p = cur;
            mbedtls_ssl_set_user_data_p(ssl, p);
            return 0;
        }

        cur = cur->next;
    }

    return -1;
}

/*
 * server certificate selection callback.
 */
static int cert_callback(mbedtls_ssl_context *ssl)
{
    const sni_entry *cur = (sni_entry *) mbedtls_ssl_get_user_data_p(ssl);
    if (cur != NULL) {
        /*(exercise mbedtls_ssl_get_hs_sni(); not otherwise used here)*/
        size_t name_len;
        const unsigned char *name = mbedtls_ssl_get_hs_sni(ssl, &name_len);
        if (strlen(cur->name) != name_len ||
            memcmp(cur->name, name, name_len) != 0) {
            return MBEDTLS_ERR_SSL_DECODE_ERROR;
        }

        if (cur->ca != NULL) {
            mbedtls_ssl_set_hs_ca_chain(ssl, cur->ca, cur->crl);
        }

        if (cur->authmode != DFL_AUTH_MODE) {
            mbedtls_ssl_set_hs_authmode(ssl, cur->authmode);
        }

        return mbedtls_ssl_set_hs_own_cert(ssl, cur->cert, cur->key);
    }

    return 0;
}

#endif /* SNI_OPTION */

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED)

typedef struct _psk_entry psk_entry;

struct _psk_entry {
    const char *name;
    size_t key_len;
    unsigned char key[MBEDTLS_PSK_MAX_LEN];
    mbedtls_svc_key_id_t slot;
    psk_entry *next;
};

/*
 * Free a list of psk_entry's
 */
static int psk_free(psk_entry *head)
{
    psk_entry *next;

    while (head != NULL) {
        psa_status_t status;
        mbedtls_svc_key_id_t const slot = head->slot;

        if (MBEDTLS_SVC_KEY_ID_GET_KEY_ID(slot) != 0) {
            status = psa_destroy_key(slot);
            if (status != PSA_SUCCESS) {
                return status;
            }
        }

        next = head->next;
        mbedtls_free(head);
        head = next;
    }

    return 0;
}

/*
 * Parse a string of pairs name1,key1[,name2,key2[,...]]
 * into a usable psk_entry list.
 *
 * Modifies the input string! This is not production quality!
 */
static psk_entry *psk_parse(char *psk_string)
{
    psk_entry *cur = NULL, *new = NULL;
    char *p = psk_string;
    char *end = p;
    char *key_hex;

    while (*end != '\0') {
        ++end;
    }
    *end = ',';

    while (p <= end) {
        if ((new = mbedtls_calloc(1, sizeof(psk_entry))) == NULL) {
            goto error;
        }

        memset(new, 0, sizeof(psk_entry));

        GET_ITEM(new->name);
        GET_ITEM(key_hex);

        if (mbedtls_test_unhexify(new->key, MBEDTLS_PSK_MAX_LEN,
                                  key_hex, &new->key_len) != 0) {
            goto error;
        }

        new->next = cur;
        cur = new;
    }

    return cur;

error:
    psk_free(new);
    psk_free(cur);
    return 0;
}

/*
 * PSK callback
 */
static int psk_callback(void *p_info, mbedtls_ssl_context *ssl,
                        const unsigned char *name, size_t name_len)
{
    psk_entry *cur = (psk_entry *) p_info;

    while (cur != NULL) {
        if (name_len == strlen(cur->name) &&
            memcmp(name, cur->name, name_len) == 0) {
            if (MBEDTLS_SVC_KEY_ID_GET_KEY_ID(cur->slot) != 0) {
                return mbedtls_ssl_set_hs_psk_opaque(ssl, cur->slot);
            } else {
                return mbedtls_ssl_set_hs_psk(ssl, cur->key, cur->key_len);
            }
        }

        cur = cur->next;
    }

    return -1;
}
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED */

static mbedtls_net_context listen_fd, client_fd;

/* Interruption handler to ensure clean exit (for valgrind testing) */
#if !defined(_WIN32)
static int received_sigterm = 0;
static void term_handler(int sig)
{
    ((void) sig);
    received_sigterm = 1;
    mbedtls_net_free(&listen_fd);   /* causes mbedtls_net_accept() to abort */
    mbedtls_net_free(&client_fd);   /* causes net_read() to abort */
}
#endif

/** Return true if \p ret is a status code indicating that there is an
 * operation in progress on an SSL connection, and false if it indicates
 * success or a fatal error.
 *
 * The possible operations in progress are:
 *
 * - A read, when the SSL input buffer does not contain a full message.
 * - A write, when the SSL output buffer contains some data that has not
 *   been sent over the network yet.
 * - An asynchronous callback that has not completed yet. */
static int mbedtls_status_is_ssl_in_progress(int ret)
{
    return ret == MBEDTLS_ERR_SSL_WANT_READ ||
           ret == MBEDTLS_ERR_SSL_WANT_WRITE ||
           ret == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS;
}

#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
typedef struct {
    mbedtls_x509_crt *cert; /*!< Certificate corresponding to the key */
    mbedtls_pk_context *pk; /*!< Private key */
    unsigned delay; /*!< Number of resume steps to go through */
    unsigned pk_owned : 1; /*!< Whether to free the pk object on exit */
} ssl_async_key_slot_t;

typedef enum {
    SSL_ASYNC_INJECT_ERROR_NONE = 0, /*!< Let the callbacks succeed */
    SSL_ASYNC_INJECT_ERROR_START, /*!< Inject error during start */
    SSL_ASYNC_INJECT_ERROR_CANCEL, /*!< Close the connection after async start */
    SSL_ASYNC_INJECT_ERROR_RESUME, /*!< Inject error during resume */
#define SSL_ASYNC_INJECT_ERROR_MAX SSL_ASYNC_INJECT_ERROR_RESUME
} ssl_async_inject_error_t;

typedef struct {
    ssl_async_key_slot_t slots[4]; /* key, key2, sni1, sni2 */
    size_t slots_used;
    ssl_async_inject_error_t inject_error;
    int (*f_rng)(void *, unsigned char *, size_t);
    void *p_rng;
} ssl_async_key_context_t;

static int ssl_async_set_key(ssl_async_key_context_t *ctx,
                             mbedtls_x509_crt *cert,
                             mbedtls_pk_context *pk,
                             int pk_take_ownership,
                             unsigned delay)
{
    if (ctx->slots_used >= sizeof(ctx->slots) / sizeof(*ctx->slots)) {
        return -1;
    }
    ctx->slots[ctx->slots_used].cert = cert;
    ctx->slots[ctx->slots_used].pk = pk;
    ctx->slots[ctx->slots_used].delay = delay;
    ctx->slots[ctx->slots_used].pk_owned = pk_take_ownership;
    ++ctx->slots_used;
    return 0;
}

#define SSL_ASYNC_INPUT_MAX_SIZE 512

typedef enum {
    ASYNC_OP_SIGN,
} ssl_async_operation_type_t;

typedef struct {
    unsigned slot;
    ssl_async_operation_type_t operation_type;
    mbedtls_md_type_t md_alg;
    unsigned char input[SSL_ASYNC_INPUT_MAX_SIZE];
    size_t input_len;
    unsigned remaining_delay;
} ssl_async_operation_context_t;

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)

/* Note that ssl_async_operation_type_t and the array below need to be kept in sync!
 * `ssl_async_operation_names[op]` is the name of op for each value `op`
 * of type `ssl_async_operation_type_t`. */
static const char *const ssl_async_operation_names[] =
{
    "sign",
};

static int ssl_async_start(mbedtls_ssl_context *ssl,
                           mbedtls_x509_crt *cert,
                           ssl_async_operation_type_t op_type,
                           mbedtls_md_type_t md_alg,
                           const unsigned char *input,
                           size_t input_len)
{
    ssl_async_key_context_t *config_data =
        mbedtls_ssl_conf_get_async_config_data(ssl->conf);
    unsigned slot;
    ssl_async_operation_context_t *ctx = NULL;
    const char *op_name = ssl_async_operation_names[op_type];

    {
        char dn[100];
        if (mbedtls_x509_dn_gets(dn, sizeof(dn), &cert->subject) > 0) {
            mbedtls_printf("Async %s callback: looking for DN=%s\n",
                           op_name, dn);
        }
    }

    /* Look for a private key that matches the public key in cert.
     * Since this test code has the private key inside Mbed TLS,
     * we call mbedtls_pk_check_pair to match a private key with the
     * public key. */
    for (slot = 0; slot < config_data->slots_used; slot++) {
        if (mbedtls_pk_check_pair(&cert->pk,
                                  config_data->slots[slot].pk) == 0) {
            break;
        }
    }
    if (slot == config_data->slots_used) {
        mbedtls_printf("Async %s callback: no key matches this certificate.\n",
                       op_name);
        return MBEDTLS_ERR_SSL_HW_ACCEL_FALLTHROUGH;
    }
    mbedtls_printf("Async %s callback: using key slot %u, delay=%u.\n",
                   op_name, slot, config_data->slots[slot].delay);

    if (config_data->inject_error == SSL_ASYNC_INJECT_ERROR_START) {
        mbedtls_printf("Async %s callback: injected error\n", op_name);
        return MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE;
    }

    if (input_len > SSL_ASYNC_INPUT_MAX_SIZE) {
        return MBEDTLS_ERR_SSL_BAD_INPUT_DATA;
    }

    ctx = mbedtls_calloc(1, sizeof(*ctx));
    if (ctx == NULL) {
        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    }
    ctx->slot = slot;
    ctx->operation_type = op_type;
    ctx->md_alg = md_alg;
    memcpy(ctx->input, input, input_len);
    ctx->input_len = input_len;
    ctx->remaining_delay = config_data->slots[slot].delay;
    mbedtls_ssl_set_async_operation_data(ssl, ctx);

    if (ctx->remaining_delay == 0) {
        return 0;
    } else {
        return MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS;
    }
}

static int ssl_async_sign(mbedtls_ssl_context *ssl,
                          mbedtls_x509_crt *cert,
                          mbedtls_md_type_t md_alg,
                          const unsigned char *hash,
                          size_t hash_len)
{
    return ssl_async_start(ssl, cert,
                           ASYNC_OP_SIGN, md_alg,
                           hash, hash_len);
}

static int ssl_async_resume(mbedtls_ssl_context *ssl,
                            unsigned char *output,
                            size_t *output_len,
                            size_t output_size)
{
    ssl_async_operation_context_t *ctx = mbedtls_ssl_get_async_operation_data(ssl);
    ssl_async_key_context_t *config_data =
        mbedtls_ssl_conf_get_async_config_data(ssl->conf);
    ssl_async_key_slot_t *key_slot = &config_data->slots[ctx->slot];
    int ret;
    const char *op_name;

    if (ctx->remaining_delay > 0) {
        --ctx->remaining_delay;
        mbedtls_printf("Async resume (slot %u): call %u more times.\n",
                       ctx->slot, ctx->remaining_delay);
        return MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS;
    }

    switch (ctx->operation_type) {
        case ASYNC_OP_SIGN:
            ret = mbedtls_pk_sign(key_slot->pk,
                                  ctx->md_alg,
                                  ctx->input, ctx->input_len,
                                  output, output_size, output_len);
            break;
        default:
            mbedtls_printf(
                "Async resume (slot %u): unknown operation type %ld. This shouldn't happen.\n",
                ctx->slot,
                (long) ctx->operation_type);
            mbedtls_free(ctx);
            return MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE;
            break;
    }

    op_name = ssl_async_operation_names[ctx->operation_type];

    if (config_data->inject_error == SSL_ASYNC_INJECT_ERROR_RESUME) {
        mbedtls_printf("Async resume callback: %s done but injected error\n",
                       op_name);
        mbedtls_free(ctx);
        return MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE;
    }

    mbedtls_printf("Async resume (slot %u): %s done, status=%d.\n",
                   ctx->slot, op_name, ret);
    mbedtls_free(ctx);
    return ret;
}

static void ssl_async_cancel(mbedtls_ssl_context *ssl)
{
    ssl_async_operation_context_t *ctx = mbedtls_ssl_get_async_operation_data(ssl);
    mbedtls_printf("Async cancel callback.\n");
    mbedtls_free(ctx);
}
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */
#endif /* MBEDTLS_SSL_ASYNC_PRIVATE */

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED)
static psa_status_t psa_setup_psk_key_slot(mbedtls_svc_key_id_t *slot,
                                           psa_algorithm_t alg,
                                           unsigned char *psk,
                                           size_t psk_len)
{
    psa_status_t status;
    psa_key_attributes_t key_attributes;

    key_attributes = psa_key_attributes_init();
    psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_DERIVE);
    psa_set_key_algorithm(&key_attributes, alg);
    psa_set_key_type(&key_attributes, PSA_KEY_TYPE_DERIVE);

    status = psa_import_key(&key_attributes, psk, psk_len, slot);
    if (status != PSA_SUCCESS) {
        fprintf(stderr, "IMPORT\n");
        return status;
    }

    return PSA_SUCCESS;
}
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED */

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
static int report_cid_usage(mbedtls_ssl_context *ssl,
                            const char *additional_description)
{
    int ret;
    unsigned char peer_cid[MBEDTLS_SSL_CID_OUT_LEN_MAX];
    size_t peer_cid_len;
    int cid_negotiated;

    if (opt.transport != MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
        return 0;
    }

    /* Check if the use of a CID has been negotiated */
    ret = mbedtls_ssl_get_peer_cid(ssl, &cid_negotiated,
                                   peer_cid, &peer_cid_len);
    if (ret != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_get_peer_cid returned -0x%x\n\n",
                       (unsigned int) -ret);
        return ret;
    }

    if (cid_negotiated == MBEDTLS_SSL_CID_DISABLED) {
        if (opt.cid_enabled == MBEDTLS_SSL_CID_ENABLED) {
            mbedtls_printf("(%s) Use of Connection ID was not offered by client.\n",
                           additional_description);
        }
    } else {
        size_t idx = 0;
        mbedtls_printf("(%s) Use of Connection ID has been negotiated.\n",
                       additional_description);
        mbedtls_printf("(%s) Peer CID (length %u Bytes): ",
                       additional_description,
                       (unsigned) peer_cid_len);
        while (idx < peer_cid_len) {
            mbedtls_printf("%02x ", peer_cid[idx]);
            idx++;
        }
        mbedtls_printf("\n");
    }

    return 0;
}
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_TICKET_C) && \
    defined(MBEDTLS_HAVE_TIME)
static inline void put_unaligned_uint32(void *p, uint32_t x)
{
    memcpy(p, &x, sizeof(x));
}

/* Functions for session ticket tests */
static int dummy_ticket_write(void *p_ticket, const mbedtls_ssl_session *session,
                              unsigned char *start, const unsigned char *end,
                              size_t *tlen, uint32_t *ticket_lifetime)
{
    int ret;
    unsigned char *p = start;
    size_t clear_len;
    ((void) p_ticket);

    if (end - p < 4) {
        return MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL;
    }
    put_unaligned_uint32(p, 7 * 24 * 3600);
    *ticket_lifetime = 7 * 24 * 3600;
    p += 4;

    /* Dump session state */
    if ((ret = mbedtls_ssl_session_save(session, p, end - p,
                                        &clear_len)) != 0) {
        return ret;
    }

    *tlen = 4 + clear_len;

    return 0;
}

static int dummy_ticket_parse(void *p_ticket, mbedtls_ssl_session *session,
                              unsigned char *buf, size_t len)
{
    int ret;
    ((void) p_ticket);

    if ((ret = mbedtls_ssl_session_load(session, buf + 4, len - 4)) != 0) {
        return ret;
    }

    switch (opt.dummy_ticket % 11) {
        case 1:
            return MBEDTLS_ERR_SSL_INVALID_MAC;
        case 2:
            return MBEDTLS_ERR_SSL_SESSION_TICKET_EXPIRED;
        case 3:
            /* Creation time in the future. */
            session->ticket_creation_time = mbedtls_ms_time() + 1000;
            break;
        case 4:
            /* Ticket has reached the end of lifetime. */
            session->ticket_creation_time = mbedtls_ms_time() -
                                            (7 * 24 * 3600 * 1000 + 1000);
            break;
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
        case 5:
            /* Ticket is valid, but client age is below the lower bound of the tolerance window. */
            session->ticket_age_add += MBEDTLS_SSL_TLS1_3_TICKET_AGE_TOLERANCE + 4 * 1000;
            /* Make sure the execution time does not affect the result */
            session->ticket_creation_time = mbedtls_ms_time();
            break;

        case 6:
            /* Ticket is valid, but client age is beyond the upper bound of the tolerance window. */
            session->ticket_age_add -= MBEDTLS_SSL_TLS1_3_TICKET_AGE_TOLERANCE + 4 * 1000;
            /* Make sure the execution time does not affect the result */
            session->ticket_creation_time = mbedtls_ms_time();
            break;
        case 7:
            session->ticket_flags = MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_NONE;
            break;
        case 8:
            session->ticket_flags = MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK;
            break;
        case 9:
            session->ticket_flags = MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL;
            break;
        case 10:
            session->ticket_flags = MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ALL;
            break;
#endif
        default:
            break;
    }

    return ret;
}
#endif /* MBEDTLS_SSL_SESSION_TICKETS && MBEDTLS_SSL_TICKET_C && MBEDTLS_HAVE_TIME */

static int parse_cipher(char *buf)
{
    int ret = 0;
    if (strcmp(buf, "AES-128-CCM")) {
        opt.ticket_alg = PSA_ALG_CCM;
        opt.ticket_key_type = PSA_KEY_TYPE_AES;
        opt.ticket_key_bits = 128;
    } else if (strcmp(buf, "AES-128-GCM")) {
        opt.ticket_alg = PSA_ALG_GCM;
        opt.ticket_key_type = PSA_KEY_TYPE_AES;
        opt.ticket_key_bits = 128;
    } else if (strcmp(buf, "AES-192-CCM")) {
        opt.ticket_alg = PSA_ALG_CCM;
        opt.ticket_key_type = PSA_KEY_TYPE_AES;
        opt.ticket_key_bits = 192;
    } else if (strcmp(buf, "AES-192-GCM")) {
        opt.ticket_alg = PSA_ALG_GCM;
        opt.ticket_key_type = PSA_KEY_TYPE_AES;
        opt.ticket_key_bits = 192;
    } else if (strcmp(buf, "AES-256-CCM")) {
        opt.ticket_alg = PSA_ALG_CCM;
        opt.ticket_key_type = PSA_KEY_TYPE_AES;
        opt.ticket_key_bits = 256;
    } else if (strcmp(buf, "ARIA-128-CCM")) {
        opt.ticket_alg = PSA_ALG_CCM;
        opt.ticket_key_type = PSA_KEY_TYPE_ARIA;
        opt.ticket_key_bits = 128;
    } else if (strcmp(buf, "ARIA-128-GCM")) {
        opt.ticket_alg = PSA_ALG_GCM;
        opt.ticket_key_type = PSA_KEY_TYPE_ARIA;
        opt.ticket_key_bits = 128;
    } else if (strcmp(buf, "ARIA-192-CCM")) {
        opt.ticket_alg = PSA_ALG_CCM;
        opt.ticket_key_type = PSA_KEY_TYPE_ARIA;
        opt.ticket_key_bits = 192;
    } else if (strcmp(buf, "ARIA-192-GCM")) {
        opt.ticket_alg = PSA_ALG_GCM;
        opt.ticket_key_type = PSA_KEY_TYPE_ARIA;
        opt.ticket_key_bits = 192;
    } else if (strcmp(buf, "ARIA-256-CCM")) {
        opt.ticket_alg = PSA_ALG_CCM;
        opt.ticket_key_type = PSA_KEY_TYPE_ARIA;
        opt.ticket_key_bits = 256;
    } else if (strcmp(buf, "ARIA-256-GCM")) {
        opt.ticket_alg = PSA_ALG_GCM;
        opt.ticket_key_type = PSA_KEY_TYPE_ARIA;
        opt.ticket_key_bits = 256;
    } else if (strcmp(buf, "CAMELLIA-128-CCM")) {
        opt.ticket_alg = PSA_ALG_CCM;
        opt.ticket_key_type = PSA_KEY_TYPE_CAMELLIA;
        opt.ticket_key_bits = 128;
    } else if (strcmp(buf, "CAMELLIA-192-CCM")) {
        opt.ticket_alg = PSA_ALG_CCM;
        opt.ticket_key_type = PSA_KEY_TYPE_CAMELLIA;
        opt.ticket_key_bits = 192;
    } else if (strcmp(buf, "CAMELLIA-256-CCM")) {
        opt.ticket_alg = PSA_ALG_CCM;
        opt.ticket_key_type = PSA_KEY_TYPE_CAMELLIA;
        opt.ticket_key_bits = 256;
    } else if (strcmp(buf, "CHACHA20-POLY1305")) {
        opt.ticket_alg = PSA_ALG_CHACHA20_POLY1305;
        opt.ticket_key_type = PSA_KEY_TYPE_CHACHA20;
        opt.ticket_key_bits = 256;
    } else {
        ret = -1;
    }
    return ret;
}

int main(int argc, char *argv[])
{
    int ret = 0, len, written, frags, exchanges_left;
    int query_config_ret = 0;
    io_ctx_t io_ctx;
    unsigned char *buf = 0;
#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED)
    psa_algorithm_t alg = 0;
    mbedtls_svc_key_id_t psk_slot = MBEDTLS_SVC_KEY_ID_INIT;
    unsigned char psk[MBEDTLS_PSK_MAX_LEN];
    size_t psk_len = 0;
    psk_entry *psk_info = NULL;
#endif
    const char *pers = "ssl_server2";
    unsigned char client_ip[16] = { 0 };
    size_t cliip_len;
#if defined(MBEDTLS_SSL_COOKIE_C)
    mbedtls_ssl_cookie_ctx cookie_ctx;
#endif

    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
#if defined(MBEDTLS_TIMING_C)
    mbedtls_timing_delay_context timer;
#endif
#if defined(MBEDTLS_SSL_RENEGOTIATION)
    unsigned char renego_period[8] = { 0 };
#endif
#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    uint32_t flags;
    mbedtls_x509_crt cacert;
    mbedtls_x509_crt srvcert;
    mbedtls_pk_context pkey;
    mbedtls_x509_crt srvcert2;
    mbedtls_pk_context pkey2;
    mbedtls_x509_crt_profile crt_profile_for_test = mbedtls_x509_crt_profile_default;
    mbedtls_svc_key_id_t key_slot = MBEDTLS_SVC_KEY_ID_INIT; /* invalid key slot */
    mbedtls_svc_key_id_t key_slot2 = MBEDTLS_SVC_KEY_ID_INIT; /* invalid key slot */
    int key_cert_init = 0, key_cert_init2 = 0;
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */
#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
    ssl_async_key_context_t ssl_async_keys;
#endif /* MBEDTLS_SSL_ASYNC_PRIVATE */
#if defined(MBEDTLS_SSL_CACHE_C)
    mbedtls_ssl_cache_context cache;
#endif
#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_TICKET_C)
    mbedtls_ssl_ticket_context ticket_ctx;
#endif /* MBEDTLS_SSL_SESSION_TICKETS && MBEDTLS_SSL_TICKET_C */
#if defined(SNI_OPTION)
    sni_entry *sni_info = NULL;
#endif
    uint16_t group_list[GROUP_LIST_SIZE];
#if defined(MBEDTLS_SSL_ALPN)
    const char *alpn_list[ALPN_LIST_SIZE];
#endif
#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
    unsigned char alloc_buf[MEMORY_HEAP_SIZE];
#endif
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    unsigned char cid[MBEDTLS_SSL_CID_IN_LEN_MAX];
    unsigned char cid_renego[MBEDTLS_SSL_CID_IN_LEN_MAX];
    size_t cid_len = 0;
    size_t cid_renego_len = 0;
#endif
#if defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION)
    unsigned char *context_buf = NULL;
    size_t context_buf_len = 0;
#endif
#if defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
    mbedtls_svc_key_id_t ecjpake_pw_slot = MBEDTLS_SVC_KEY_ID_INIT; /* ecjpake password key slot */
#endif /* MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED */

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    uint16_t sig_alg_list[SIG_ALG_LIST_SIZE];
#endif

    int i;
    char *p, *q;
    const int *list;
    psa_status_t status;
    unsigned char eap_tls_keymaterial[16];
    unsigned char eap_tls_iv[8];
    const char *eap_tls_label = "client EAP encryption";
    eap_tls_keys eap_tls_keying;
#if defined(MBEDTLS_SSL_DTLS_SRTP)
    /*! master keys and master salt for SRTP generated during handshake */
    unsigned char dtls_srtp_key_material[MBEDTLS_TLS_SRTP_MAX_KEY_MATERIAL_LENGTH];
    const char *dtls_srtp_label = "EXTRACTOR-dtls_srtp";
    dtls_srtp_keys dtls_srtp_keying;
    const mbedtls_ssl_srtp_profile default_profiles[] = {
        MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_80,
        MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_32,
        MBEDTLS_TLS_SRTP_NULL_HMAC_SHA1_80,
        MBEDTLS_TLS_SRTP_NULL_HMAC_SHA1_32,
        MBEDTLS_TLS_SRTP_UNSET
    };
#endif /* MBEDTLS_SSL_DTLS_SRTP */

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
    mbedtls_memory_buffer_alloc_init(alloc_buf, sizeof(alloc_buf));
#if defined(MBEDTLS_MEMORY_DEBUG)
    size_t current_heap_memory, peak_heap_memory, heap_blocks;
#endif  /* MBEDTLS_MEMORY_DEBUG */
#endif  /* MBEDTLS_MEMORY_BUFFER_ALLOC_C */

#if defined(MBEDTLS_TEST_HOOKS)
    test_hooks_init();
#endif /* MBEDTLS_TEST_HOOKS */

    /*
     * Make sure memory references are valid in case we exit early.
     */
    mbedtls_net_init(&client_fd);
    mbedtls_net_init(&listen_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    mbedtls_x509_crt_init(&cacert);
    mbedtls_x509_crt_init(&srvcert);
    mbedtls_pk_init(&pkey);
    mbedtls_x509_crt_init(&srvcert2);
    mbedtls_pk_init(&pkey2);
#endif
#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
    memset(&ssl_async_keys, 0, sizeof(ssl_async_keys));
#endif
#if defined(MBEDTLS_SSL_CACHE_C)
    mbedtls_ssl_cache_init(&cache);
#endif
#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_TICKET_C)
    mbedtls_ssl_ticket_init(&ticket_ctx);
#endif
#if defined(MBEDTLS_SSL_ALPN)
    memset((void *) alpn_list, 0, sizeof(alpn_list));
#endif
#if defined(MBEDTLS_SSL_COOKIE_C)
    mbedtls_ssl_cookie_init(&cookie_ctx);
#endif

    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        mbedtls_fprintf(stderr, "Failed to initialize PSA Crypto implementation: %d\n",
                        (int) status);
        ret = MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
        goto exit;
    }
#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
    mbedtls_test_enable_insecure_external_rng();
#endif  /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */

#if !defined(_WIN32)
    /* Abort cleanly on SIGTERM and SIGINT */
    signal(SIGTERM, term_handler);
    signal(SIGINT, term_handler);
#endif

    opt.buffer_size         = DFL_IO_BUF_LEN;
    opt.server_addr         = DFL_SERVER_ADDR;
    opt.server_port         = DFL_SERVER_PORT;
    opt.debug_level         = DFL_DEBUG_LEVEL;
    opt.event               = DFL_EVENT;
    opt.response_size       = DFL_RESPONSE_SIZE;
    opt.nbio                = DFL_NBIO;
    opt.cid_enabled         = DFL_CID_ENABLED;
    opt.cid_enabled_renego  = DFL_CID_ENABLED_RENEGO;
    opt.cid_val             = DFL_CID_VALUE;
    opt.cid_val_renego      = DFL_CID_VALUE_RENEGO;
    opt.read_timeout        = DFL_READ_TIMEOUT;
    opt.exp_label           = DFL_EXP_LABEL;
    opt.exp_len             = DFL_EXP_LEN;
    opt.ca_file             = DFL_CA_FILE;
    opt.ca_path             = DFL_CA_PATH;
    opt.crt_file            = DFL_CRT_FILE;
    opt.key_file            = DFL_KEY_FILE;
    opt.key_opaque          = DFL_KEY_OPAQUE;
    opt.key_pwd             = DFL_KEY_PWD;
    opt.crt_file2           = DFL_CRT_FILE2;
    opt.key_file2           = DFL_KEY_FILE2;
    opt.key_pwd2            = DFL_KEY_PWD2;
    opt.async_operations    = DFL_ASYNC_OPERATIONS;
    opt.async_private_delay1 = DFL_ASYNC_PRIVATE_DELAY1;
    opt.async_private_delay2 = DFL_ASYNC_PRIVATE_DELAY2;
    opt.async_private_error = DFL_ASYNC_PRIVATE_ERROR;
    opt.psk                 = DFL_PSK;
    opt.psk_opaque          = DFL_PSK_OPAQUE;
    opt.psk_list_opaque     = DFL_PSK_LIST_OPAQUE;
#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
    opt.ca_callback         = DFL_CA_CALLBACK;
#endif
    opt.psk_identity        = DFL_PSK_IDENTITY;
    opt.psk_list            = DFL_PSK_LIST;
    opt.ecjpake_pw          = DFL_ECJPAKE_PW;
    opt.ecjpake_pw_opaque   = DFL_ECJPAKE_PW_OPAQUE;
    opt.force_ciphersuite[0] = DFL_FORCE_CIPHER;
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    opt.tls13_kex_modes     = DFL_TLS1_3_KEX_MODES;
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */
    opt.renegotiation       = DFL_RENEGOTIATION;
    opt.allow_legacy        = DFL_ALLOW_LEGACY;
    opt.renegotiate         = DFL_RENEGOTIATE;
    opt.renego_delay        = DFL_RENEGO_DELAY;
    opt.renego_period       = DFL_RENEGO_PERIOD;
    opt.exchanges           = DFL_EXCHANGES;
    opt.min_version         = DFL_MIN_VERSION;
    opt.max_version         = DFL_MAX_VERSION;
    opt.allow_sha1          = DFL_SHA1;
    opt.auth_mode           = DFL_AUTH_MODE;
    opt.cert_req_ca_list    = DFL_CERT_REQ_CA_LIST;
    opt.cert_req_dn_hint    = DFL_CERT_REQ_DN_HINT;
    opt.mfl_code            = DFL_MFL_CODE;
    opt.trunc_hmac          = DFL_TRUNC_HMAC;
    opt.tickets             = DFL_TICKETS;
    opt.dummy_ticket        = DFL_DUMMY_TICKET;
    opt.ticket_rotate       = DFL_TICKET_ROTATE;
    opt.ticket_timeout      = DFL_TICKET_TIMEOUT;
    opt.ticket_alg          = DFL_TICKET_ALG;
    opt.ticket_key_type     = DFL_TICKET_KEY_TYPE;
    opt.ticket_key_bits     = DFL_TICKET_KEY_BITS;
    opt.cache_max           = DFL_CACHE_MAX;
#if defined(MBEDTLS_HAVE_TIME)
    opt.cache_timeout       = DFL_CACHE_TIMEOUT;
#endif
    opt.cache_remove        = DFL_CACHE_REMOVE;
    opt.sni                 = DFL_SNI;
    opt.alpn_string         = DFL_ALPN_STRING;
    opt.groups              = DFL_GROUPS;
#if defined(MBEDTLS_SSL_EARLY_DATA)
    opt.early_data          = DFL_EARLY_DATA;
    opt.max_early_data_size = DFL_MAX_EARLY_DATA_SIZE;
#endif
    opt.sig_algs            = DFL_SIG_ALGS;
    opt.transport           = DFL_TRANSPORT;
    opt.cookies             = DFL_COOKIES;
    opt.anti_replay         = DFL_ANTI_REPLAY;
    opt.hs_to_min           = DFL_HS_TO_MIN;
    opt.hs_to_max           = DFL_HS_TO_MAX;
    opt.dtls_mtu            = DFL_DTLS_MTU;
    opt.dgram_packing       = DFL_DGRAM_PACKING;
    opt.badmac_limit        = DFL_BADMAC_LIMIT;
    opt.extended_ms         = DFL_EXTENDED_MS;
    opt.etm                 = DFL_ETM;
    opt.serialize           = DFL_SERIALIZE;
    opt.context_file        = DFL_CONTEXT_FILE;
    opt.eap_tls             = DFL_EAP_TLS;
    opt.reproducible        = DFL_REPRODUCIBLE;
    opt.nss_keylog          = DFL_NSS_KEYLOG;
    opt.nss_keylog_file     = DFL_NSS_KEYLOG_FILE;
    opt.query_config_mode   = DFL_QUERY_CONFIG_MODE;
    opt.use_srtp            = DFL_USE_SRTP;
    opt.force_srtp_profile  = DFL_SRTP_FORCE_PROFILE;
    opt.support_mki         = DFL_SRTP_SUPPORT_MKI;
    opt.key1_opaque_alg1   = DFL_KEY_OPAQUE_ALG;
    opt.key1_opaque_alg2   = DFL_KEY_OPAQUE_ALG;
    opt.key2_opaque_alg1   = DFL_KEY_OPAQUE_ALG;
    opt.key2_opaque_alg2   = DFL_KEY_OPAQUE_ALG;

    p = q = NULL;
    if (argc < 1) {
usage:
        if (p != NULL && q != NULL) {
            printf("unrecognized value for '%s': '%s'\n", p, q);
        } else if (p != NULL && q == NULL) {
            printf("unrecognized param: '%s'\n", p);
        }

        mbedtls_printf("usage: ssl_client2 [param=value] [...]\n");
        mbedtls_printf("       ssl_client2 help[_theme]\n");
        mbedtls_printf("'help' lists acceptable 'param' and 'value'\n");
        mbedtls_printf("'help_ciphersuites' lists available ciphersuites\n");
        mbedtls_printf("\n");

        if (ret == 0) {
            ret = 1;
        }
        goto exit;
    }

    for (i = 1; i < argc; i++) {
        p = argv[i];

        if (strcmp(p, "help") == 0) {
            mbedtls_printf(USAGE1);
            mbedtls_printf(USAGE2);
            mbedtls_printf(USAGE3);
            mbedtls_printf(USAGE4);

            ret = 0;
            goto exit;
        }
        if (strcmp(p, "help_ciphersuites") == 0) {
            mbedtls_printf(" acceptable ciphersuite names:\n");
            for (list = mbedtls_ssl_list_ciphersuites();
                 *list != 0;
                 list++) {
                mbedtls_printf(" %s\n", mbedtls_ssl_get_ciphersuite_name(*list));
            }

            ret = 0;
            goto exit;
        }

        if ((q = strchr(p, '=')) == NULL) {
            mbedtls_printf("param requires a value: '%s'\n", p);
            p = NULL; // avoid "unrecnognized param" message
            goto usage;
        }
        *q++ = '\0';

        if (strcmp(p, "server_port") == 0) {
            opt.server_port = q;
        } else if (strcmp(p, "server_addr") == 0) {
            opt.server_addr = q;
        } else if (strcmp(p, "dtls") == 0) {
            int t = atoi(q);
            if (t == 0) {
                opt.transport = MBEDTLS_SSL_TRANSPORT_STREAM;
            } else if (t == 1) {
                opt.transport = MBEDTLS_SSL_TRANSPORT_DATAGRAM;
            } else {
                goto usage;
            }
        } else if (strcmp(p, "debug_level") == 0) {
            opt.debug_level = atoi(q);
            if (opt.debug_level < 0 || opt.debug_level > 65535) {
                goto usage;
            }
        } else if (strcmp(p, "build_version") == 0) {
            if (strcmp(q, "1") == 0) {
                mbedtls_printf("build version: %s (build %d)\n",
                               MBEDTLS_VERSION_STRING_FULL,
                               MBEDTLS_VERSION_NUMBER);
                goto exit;
            }
        } else if (strcmp(p, "nbio") == 0) {
            opt.nbio = atoi(q);
            if (opt.nbio < 0 || opt.nbio > 2) {
                goto usage;
            }
        } else if (strcmp(p, "event") == 0) {
            opt.event = atoi(q);
            if (opt.event < 0 || opt.event > 2) {
                goto usage;
            }
        } else if (strcmp(p, "read_timeout") == 0) {
            opt.read_timeout = atoi(q);
        } else if (strcmp(p, "exp_label") == 0) {
            opt.exp_label = q;
        } else if (strcmp(p, "exp_len") == 0) {
            opt.exp_len = atoi(q);
        } else if (strcmp(p, "buffer_size") == 0) {
            opt.buffer_size = atoi(q);
            if (opt.buffer_size < 1) {
                goto usage;
            }
        } else if (strcmp(p, "response_size") == 0) {
            opt.response_size = atoi(q);
            if (opt.response_size < 0 || opt.response_size > MBEDTLS_SSL_OUT_CONTENT_LEN) {
                goto usage;
            }
            if (opt.buffer_size < opt.response_size) {
                opt.buffer_size = opt.response_size;
            }
        } else if (strcmp(p, "ca_file") == 0) {
            opt.ca_file = q;
        } else if (strcmp(p, "ca_path") == 0) {
            opt.ca_path = q;
        } else if (strcmp(p, "crt_file") == 0) {
            opt.crt_file = q;
        } else if (strcmp(p, "key_file") == 0) {
            opt.key_file = q;
        } else if (strcmp(p, "key_pwd") == 0) {
            opt.key_pwd = q;
        }
#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
        else if (strcmp(p, "key_opaque") == 0) {
            opt.key_opaque = atoi(q);
        }
#endif
        else if (strcmp(p, "crt_file2") == 0) {
            opt.crt_file2 = q;
        } else if (strcmp(p, "key_file2") == 0) {
            opt.key_file2 = q;
        } else if (strcmp(p, "key_pwd2") == 0) {
            opt.key_pwd2 = q;
        }
#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
        else if (strcmp(p, "async_operations") == 0) {
            opt.async_operations = q;
        } else if (strcmp(p, "async_private_delay1") == 0) {
            opt.async_private_delay1 = atoi(q);
        } else if (strcmp(p, "async_private_delay2") == 0) {
            opt.async_private_delay2 = atoi(q);
        } else if (strcmp(p, "async_private_error") == 0) {
            int n = atoi(q);
            if (n < -SSL_ASYNC_INJECT_ERROR_MAX ||
                n > SSL_ASYNC_INJECT_ERROR_MAX) {
                ret = 2;
                goto usage;
            }
            opt.async_private_error = n;
        }
#endif /* MBEDTLS_SSL_ASYNC_PRIVATE */
#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
        else if (strcmp(p, "cid") == 0) {
            opt.cid_enabled = atoi(q);
            if (opt.cid_enabled != 0 && opt.cid_enabled != 1) {
                goto usage;
            }
        } else if (strcmp(p, "cid_renego") == 0) {
            opt.cid_enabled_renego = atoi(q);
            if (opt.cid_enabled_renego != 0 && opt.cid_enabled_renego != 1) {
                goto usage;
            }
        } else if (strcmp(p, "cid_val") == 0) {
            opt.cid_val = q;
        } else if (strcmp(p, "cid_val_renego") == 0) {
            opt.cid_val_renego = q;
        }
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */
        else if (strcmp(p, "psk") == 0) {
            opt.psk = q;
        } else if (strcmp(p, "psk_opaque") == 0) {
            opt.psk_opaque = atoi(q);
        } else if (strcmp(p, "psk_list_opaque") == 0) {
            opt.psk_list_opaque = atoi(q);
        }
#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
        else if (strcmp(p, "ca_callback") == 0) {
            opt.ca_callback = atoi(q);
        }
#endif
        else if (strcmp(p, "psk_identity") == 0) {
            opt.psk_identity = q;
        } else if (strcmp(p, "psk_list") == 0) {
            opt.psk_list = q;
        } else if (strcmp(p, "ecjpake_pw") == 0) {
            opt.ecjpake_pw = q;
        } else if (strcmp(p, "ecjpake_pw_opaque") == 0) {
            opt.ecjpake_pw_opaque = atoi(q);
        } else if (strcmp(p, "force_ciphersuite") == 0) {
            opt.force_ciphersuite[0] = mbedtls_ssl_get_ciphersuite_id(q);

            if (opt.force_ciphersuite[0] == 0) {
                ret = 2;
                goto usage;
            }
            opt.force_ciphersuite[1] = 0;
        } else if (strcmp(p, "groups") == 0) {
            opt.groups = q;
        }
#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
        else if (strcmp(p, "sig_algs") == 0) {
            opt.sig_algs = q;
        }
#endif
#if defined(MBEDTLS_SSL_EARLY_DATA)
        else if (strcmp(p, "early_data") == 0) {
            switch (atoi(q)) {
                case 0:
                    opt.early_data = MBEDTLS_SSL_EARLY_DATA_DISABLED;
                    break;
                case 1:
                    opt.early_data = MBEDTLS_SSL_EARLY_DATA_ENABLED;
                    break;
                default: goto usage;
            }
        } else if (strcmp(p, "max_early_data_size") == 0) {
            opt.max_early_data_size = (uint32_t) atoll(q);
        }
#endif /* MBEDTLS_SSL_EARLY_DATA */
        else if (strcmp(p, "renegotiation") == 0) {
            opt.renegotiation = (atoi(q)) ?
                                MBEDTLS_SSL_RENEGOTIATION_ENABLED :
                                MBEDTLS_SSL_RENEGOTIATION_DISABLED;
        } else if (strcmp(p, "allow_legacy") == 0) {
            switch (atoi(q)) {
                case -1:
                    opt.allow_legacy = MBEDTLS_SSL_LEGACY_BREAK_HANDSHAKE;
                    break;
                case 0:
                    opt.allow_legacy = MBEDTLS_SSL_LEGACY_NO_RENEGOTIATION;
                    break;
                case 1:
                    opt.allow_legacy = MBEDTLS_SSL_LEGACY_ALLOW_RENEGOTIATION;
                    break;
                default: goto usage;
            }
        } else if (strcmp(p, "renegotiate") == 0) {
            opt.renegotiate = atoi(q);
            if (opt.renegotiate < 0 || opt.renegotiate > 1) {
                goto usage;
            }
        } else if (strcmp(p, "renego_delay") == 0) {
            opt.renego_delay = atoi(q);
        } else if (strcmp(p, "renego_period") == 0) {
#if defined(_MSC_VER)
            opt.renego_period = _strtoui64(q, NULL, 10);
#else
            if (sscanf(q, "%" SCNu64, &opt.renego_period) != 1) {
                goto usage;
            }
#endif /* _MSC_VER */
            if (opt.renego_period < 2) {
                goto usage;
            }
        } else if (strcmp(p, "exchanges") == 0) {
            opt.exchanges = atoi(q);
            if (opt.exchanges < 0) {
                goto usage;
            }
        }
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
        else if (strcmp(p, "tls13_kex_modes") == 0) {
            if (strcmp(q, "psk") == 0) {
                opt.tls13_kex_modes = MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK;
            } else if (strcmp(q, "psk_ephemeral") == 0) {
                opt.tls13_kex_modes = MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_EPHEMERAL;
            } else if (strcmp(q, "ephemeral") == 0) {
                opt.tls13_kex_modes = MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL;
            } else if (strcmp(q, "ephemeral_all") == 0) {
                opt.tls13_kex_modes = MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL_ALL;
            } else if (strcmp(q, "psk_all") == 0) {
                opt.tls13_kex_modes = MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK_ALL;
            } else if (strcmp(q, "all") == 0) {
                opt.tls13_kex_modes = MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_ALL;
            }
            /* The purpose of `psk_or_ephemeral` is to improve test coverage. That
             * is not recommended in practice.
             * `psk_or_ephemeral` exists in theory, we need this mode to test if
             * this setting work correctly. With this key exchange setting, server
             * should always perform `ephemeral` handshake. `psk` or `psk_ephemeral`
             * is not expected.
             */
            else if (strcmp(q, "psk_or_ephemeral") == 0) {
                opt.tls13_kex_modes = MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_PSK |
                                      MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_EPHEMERAL;
            } else {
                goto usage;
            }
        }
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

        else if (strcmp(p, "min_version") == 0) {
            if (strcmp(q, "tls12") == 0 ||
                strcmp(q, "dtls12") == 0) {
                opt.min_version = MBEDTLS_SSL_VERSION_TLS1_2;
            }
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
            else if (strcmp(q, "tls13") == 0) {
                opt.min_version = MBEDTLS_SSL_VERSION_TLS1_3;
            }
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */
            else {
                goto usage;
            }
        } else if (strcmp(p, "max_version") == 0) {
            if (strcmp(q, "tls12") == 0 ||
                strcmp(q, "dtls12") == 0) {
                opt.max_version = MBEDTLS_SSL_VERSION_TLS1_2;
            }
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
            else if (strcmp(q, "tls13") == 0) {
                opt.max_version = MBEDTLS_SSL_VERSION_TLS1_3;
            }
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */
            else {
                goto usage;
            }
        } else if (strcmp(p, "allow_sha1") == 0) {
            switch (atoi(q)) {
                case 0:     opt.allow_sha1 = 0;   break;
                case 1:     opt.allow_sha1 = 1;    break;
                default:    goto usage;
            }
        } else if (strcmp(p, "force_version") == 0) {
            if (strcmp(q, "tls12") == 0) {
                opt.min_version = MBEDTLS_SSL_VERSION_TLS1_2;
                opt.max_version = MBEDTLS_SSL_VERSION_TLS1_2;
            } else if (strcmp(q, "dtls12") == 0) {
                opt.min_version = MBEDTLS_SSL_VERSION_TLS1_2;
                opt.max_version = MBEDTLS_SSL_VERSION_TLS1_2;
                opt.transport = MBEDTLS_SSL_TRANSPORT_DATAGRAM;
            }
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
            else if (strcmp(q, "tls13") == 0) {
                opt.min_version = MBEDTLS_SSL_VERSION_TLS1_3;
                opt.max_version = MBEDTLS_SSL_VERSION_TLS1_3;
            }
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */
            else {
                goto usage;
            }
        } else if (strcmp(p, "auth_mode") == 0) {
            if ((opt.auth_mode = get_auth_mode(q)) < 0) {
                goto usage;
            }
        } else if (strcmp(p, "cert_req_ca_list") == 0) {
            opt.cert_req_ca_list = atoi(q);
            if (opt.cert_req_ca_list < 0 || opt.cert_req_ca_list > 3) {
                goto usage;
            }
            if (opt.cert_req_ca_list > 1) {
                opt.cert_req_dn_hint = opt.cert_req_ca_list;
                opt.cert_req_ca_list = MBEDTLS_SSL_CERT_REQ_CA_LIST_ENABLED;
            }
        } else if (strcmp(p, "max_frag_len") == 0) {
            if (strcmp(q, "512") == 0) {
                opt.mfl_code = MBEDTLS_SSL_MAX_FRAG_LEN_512;
            } else if (strcmp(q, "1024") == 0) {
                opt.mfl_code = MBEDTLS_SSL_MAX_FRAG_LEN_1024;
            } else if (strcmp(q, "2048") == 0) {
                opt.mfl_code = MBEDTLS_SSL_MAX_FRAG_LEN_2048;
            } else if (strcmp(q, "4096") == 0) {
                opt.mfl_code = MBEDTLS_SSL_MAX_FRAG_LEN_4096;
            } else {
                goto usage;
            }
        } else if (strcmp(p, "alpn") == 0) {
            opt.alpn_string = q;
        } else if (strcmp(p, "trunc_hmac") == 0) {
            switch (atoi(q)) {
                case 0: opt.trunc_hmac = MBEDTLS_SSL_TRUNC_HMAC_DISABLED; break;
                case 1: opt.trunc_hmac = MBEDTLS_SSL_TRUNC_HMAC_ENABLED; break;
                default: goto usage;
            }
        } else if (strcmp(p, "extended_ms") == 0) {
            switch (atoi(q)) {
                case 0:
                    opt.extended_ms = MBEDTLS_SSL_EXTENDED_MS_DISABLED;
                    break;
                case 1:
                    opt.extended_ms = MBEDTLS_SSL_EXTENDED_MS_ENABLED;
                    break;
                default: goto usage;
            }
        } else if (strcmp(p, "etm") == 0) {
            switch (atoi(q)) {
                case 0: opt.etm = MBEDTLS_SSL_ETM_DISABLED; break;
                case 1: opt.etm = MBEDTLS_SSL_ETM_ENABLED; break;
                default: goto usage;
            }
        } else if (strcmp(p, "tickets") == 0) {
            opt.tickets = atoi(q);
            if (opt.tickets < 0) {
                goto usage;
            }
        } else if (strcmp(p, "dummy_ticket") == 0) {
            opt.dummy_ticket = atoi(q);
            if (opt.dummy_ticket < 0) {
                goto usage;
            }
        } else if (strcmp(p, "ticket_rotate") == 0) {
            opt.ticket_rotate = atoi(q);
            if (opt.ticket_rotate < 0 || opt.ticket_rotate > 1) {
                goto usage;
            }
        } else if (strcmp(p, "ticket_timeout") == 0) {
            opt.ticket_timeout = atoi(q);
            if (opt.ticket_timeout < 0) {
                goto usage;
            }
        } else if (strcmp(p, "ticket_aead") == 0) {
            if (parse_cipher(q) != 0) {
                goto usage;
            }
        } else if (strcmp(p, "cache_max") == 0) {
            opt.cache_max = atoi(q);
            if (opt.cache_max < 0) {
                goto usage;
            }
        }
#if defined(MBEDTLS_HAVE_TIME)
        else if (strcmp(p, "cache_timeout") == 0) {
            opt.cache_timeout = atoi(q);
            if (opt.cache_timeout < 0) {
                goto usage;
            }
        }
#endif
        else if (strcmp(p, "cache_remove") == 0) {
            opt.cache_remove = atoi(q);
            if (opt.cache_remove < 0 || opt.cache_remove > 1) {
                goto usage;
            }
        } else if (strcmp(p, "cookies") == 0) {
            opt.cookies = atoi(q);
            if (opt.cookies < -1 || opt.cookies > 1) {
                goto usage;
            }
        } else if (strcmp(p, "anti_replay") == 0) {
            opt.anti_replay = atoi(q);
            if (opt.anti_replay < 0 || opt.anti_replay > 1) {
                goto usage;
            }
        } else if (strcmp(p, "badmac_limit") == 0) {
            opt.badmac_limit = atoi(q);
            if (opt.badmac_limit < 0) {
                goto usage;
            }
        } else if (strcmp(p, "hs_timeout") == 0) {
            if ((p = strchr(q, '-')) == NULL) {
                goto usage;
            }
            *p++ = '\0';
            opt.hs_to_min = atoi(q);
            opt.hs_to_max = atoi(p);
            if (opt.hs_to_min == 0 || opt.hs_to_max < opt.hs_to_min) {
                goto usage;
            }
        } else if (strcmp(p, "mtu") == 0) {
            opt.dtls_mtu = atoi(q);
            if (opt.dtls_mtu < 0) {
                goto usage;
            }
        } else if (strcmp(p, "dgram_packing") == 0) {
            opt.dgram_packing = atoi(q);
            if (opt.dgram_packing != 0 &&
                opt.dgram_packing != 1) {
                goto usage;
            }
        } else if (strcmp(p, "sni") == 0) {
            opt.sni = q;
        } else if (strcmp(p, "query_config") == 0) {
            opt.query_config_mode = 1;
            query_config_ret = query_config(q);
            goto exit;
        } else if (strcmp(p, "serialize") == 0) {
            opt.serialize = atoi(q);
            if (opt.serialize < 0 || opt.serialize > 2) {
                goto usage;
            }
        } else if (strcmp(p, "context_file") == 0) {
            opt.context_file = q;
        } else if (strcmp(p, "eap_tls") == 0) {
            opt.eap_tls = atoi(q);
            if (opt.eap_tls < 0 || opt.eap_tls > 1) {
                goto usage;
            }
        } else if (strcmp(p, "reproducible") == 0) {
            opt.reproducible = 1;
        } else if (strcmp(p, "nss_keylog") == 0) {
            opt.nss_keylog = atoi(q);
            if (opt.nss_keylog < 0 || opt.nss_keylog > 1) {
                goto usage;
            }
        } else if (strcmp(p, "nss_keylog_file") == 0) {
            opt.nss_keylog_file = q;
        } else if (strcmp(p, "use_srtp") == 0) {
            opt.use_srtp = atoi(q);
        } else if (strcmp(p, "srtp_force_profile") == 0) {
            opt.force_srtp_profile = atoi(q);
        } else if (strcmp(p, "support_mki") == 0) {
            opt.support_mki = atoi(q);
        } else if (strcmp(p, "key_opaque_algs") == 0) {
            if (key_opaque_alg_parse(q, &opt.key1_opaque_alg1,
                                     &opt.key1_opaque_alg2) != 0) {
                goto usage;
            }
        } else if (strcmp(p, "key_opaque_algs2") == 0) {
            if (key_opaque_alg_parse(q, &opt.key2_opaque_alg1,
                                     &opt.key2_opaque_alg2) != 0) {
                goto usage;
            }
        } else {
            /* This signals that the problem is with p not q */
            q = NULL;
            goto usage;
        }
    }
    /* This signals that any further erorrs are not with a single option */
    p = q = NULL;

    if (opt.nss_keylog != 0 && opt.eap_tls != 0) {
        mbedtls_printf("Error: eap_tls and nss_keylog options cannot be used together.\n");
        goto usage;
    }

    /* Event-driven IO is incompatible with the above custom
     * receive and send functions, as the polling builds on
     * refers to the underlying net_context. */
    if (opt.event == 1 && opt.nbio != 1) {
        mbedtls_printf("Warning: event-driven IO mandates nbio=1 - overwrite\n");
        opt.nbio = 1;
    }

#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(opt.debug_level);
#endif

    /* buf will alternatively contain the input read from the client and the
     * response that's about to be sent, plus a null byte in each case. */
    size_t buf_content_size = opt.buffer_size;
    /* The default response contains the ciphersuite name. Leave enough
     * room for that plus some margin. */
    if (buf_content_size < strlen(HTTP_RESPONSE) + 80) {
        buf_content_size = strlen(HTTP_RESPONSE) + 80;
    }
    if (opt.response_size != DFL_RESPONSE_SIZE &&
        buf_content_size < (size_t) opt.response_size) {
        buf_content_size = opt.response_size;
    }
    buf = mbedtls_calloc(1, buf_content_size + 1);
    if (buf == NULL) {
        mbedtls_printf("Could not allocate %lu bytes\n",
                       (unsigned long) buf_content_size + 1);
        ret = 3;
        goto exit;
    }

    if (opt.psk_opaque != 0) {
        if (strlen(opt.psk) == 0) {
            mbedtls_printf("psk_opaque set but no psk to be imported specified.\n");
            ret = 2;
            goto usage;
        }

        if (opt.force_ciphersuite[0] <= 0) {
            mbedtls_printf(
                "opaque PSKs are only supported in conjunction with forcing TLS 1.2 and a PSK-only ciphersuite through the 'force_ciphersuite' option.\n");
            ret = 2;
            goto usage;
        }
    }

    if (opt.psk_list_opaque != 0) {
        if (opt.psk_list == NULL) {
            mbedtls_printf("psk_slot set but no psk to be imported specified.\n");
            ret = 2;
            goto usage;
        }

        if (opt.force_ciphersuite[0] <= 0) {
            mbedtls_printf(
                "opaque PSKs are only supported in conjunction with forcing TLS 1.2 and a PSK-only ciphersuite through the 'force_ciphersuite' option.\n");
            ret = 2;
            goto usage;
        }
    }

    if (opt.force_ciphersuite[0] > 0) {
        const mbedtls_ssl_ciphersuite_t *ciphersuite_info;
        ciphersuite_info =
            mbedtls_ssl_ciphersuite_from_id(opt.force_ciphersuite[0]);

        if (opt.max_version != -1 &&
            ciphersuite_info->min_tls_version > opt.max_version) {
            mbedtls_printf("forced ciphersuite not allowed with this protocol version\n");
            ret = 2;
            goto usage;
        }
        if (opt.min_version != -1 &&
            ciphersuite_info->max_tls_version < opt.min_version) {
            mbedtls_printf("forced ciphersuite not allowed with this protocol version\n");
            ret = 2;
            goto usage;
        }

        /* If we select a version that's not supported by
         * this suite, then there will be no common ciphersuite... */
        if (opt.max_version == -1 ||
            opt.max_version > ciphersuite_info->max_tls_version) {
            opt.max_version = ciphersuite_info->max_tls_version;
        }
        if (opt.min_version < ciphersuite_info->min_tls_version) {
            opt.min_version = ciphersuite_info->min_tls_version;
        }

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED)
        if (opt.psk_opaque != 0 || opt.psk_list_opaque != 0) {
            /* Determine KDF algorithm the opaque PSK will be used in. */
#if defined(PSA_WANT_ALG_SHA_384)
            if (ciphersuite_info->mac == MBEDTLS_MD_SHA384) {
                alg = PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_384);
            } else
#endif /* PSA_WANT_ALG_SHA_384 */
            alg = PSA_ALG_TLS12_PSK_TO_MS(PSA_ALG_SHA_256);
        }
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED */
    }

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    if (mbedtls_test_unhexify(cid, sizeof(cid),
                              opt.cid_val, &cid_len) != 0) {
        mbedtls_printf("CID not valid hex\n");
        goto exit;
    }

    /* Keep CID settings for renegotiation unless
     * specified otherwise. */
    if (opt.cid_enabled_renego == DFL_CID_ENABLED_RENEGO) {
        opt.cid_enabled_renego = opt.cid_enabled;
    }
    if (opt.cid_val_renego == DFL_CID_VALUE_RENEGO) {
        opt.cid_val_renego = opt.cid_val;
    }

    if (mbedtls_test_unhexify(cid_renego, sizeof(cid_renego),
                              opt.cid_val_renego, &cid_renego_len) != 0) {
        mbedtls_printf("CID not valid hex\n");
        goto exit;
    }
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED)
    /*
     * Unhexify the pre-shared key and parse the list if any given
     */
    if (mbedtls_test_unhexify(psk, sizeof(psk),
                              opt.psk, &psk_len) != 0) {
        mbedtls_printf("pre-shared key not valid hex\n");
        goto exit;
    }

    if (opt.psk_list != NULL) {
        if ((psk_info = psk_parse(opt.psk_list)) == NULL) {
            mbedtls_printf("psk_list invalid");
            goto exit;
        }
    }
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED */

    if (opt.groups != NULL) {
        if (parse_groups(opt.groups, group_list, GROUP_LIST_SIZE) != 0) {
            goto exit;
        }
    }

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    if (opt.sig_algs != NULL) {
        p = (char *) opt.sig_algs;
        i = 0;

        /* Leave room for a final MBEDTLS_TLS1_3_SIG_NONE in signature algorithm list (sig_alg_list). */
        while (i < SIG_ALG_LIST_SIZE - 1 && *p != '\0') {
            q = p;

            /* Terminate the current string */
            while (*p != ',' && *p != '\0') {
                p++;
            }
            if (*p == ',') {
                *p++ = '\0';
            }

            if (strcmp(q, "rsa_pkcs1_sha256") == 0) {
                sig_alg_list[i++] = MBEDTLS_TLS1_3_SIG_RSA_PKCS1_SHA256;
            } else if (strcmp(q, "rsa_pkcs1_sha384") == 0) {
                sig_alg_list[i++] = MBEDTLS_TLS1_3_SIG_RSA_PKCS1_SHA384;
            } else if (strcmp(q, "rsa_pkcs1_sha512") == 0) {
                sig_alg_list[i++] = MBEDTLS_TLS1_3_SIG_RSA_PKCS1_SHA512;
            } else if (strcmp(q, "ecdsa_secp256r1_sha256") == 0) {
                sig_alg_list[i++] = MBEDTLS_TLS1_3_SIG_ECDSA_SECP256R1_SHA256;
            } else if (strcmp(q, "ecdsa_secp384r1_sha384") == 0) {
                sig_alg_list[i++] = MBEDTLS_TLS1_3_SIG_ECDSA_SECP384R1_SHA384;
            } else if (strcmp(q, "ecdsa_secp521r1_sha512") == 0) {
                sig_alg_list[i++] = MBEDTLS_TLS1_3_SIG_ECDSA_SECP521R1_SHA512;
            } else if (strcmp(q, "rsa_pss_rsae_sha256") == 0) {
                sig_alg_list[i++] = MBEDTLS_TLS1_3_SIG_RSA_PSS_RSAE_SHA256;
            } else if (strcmp(q, "rsa_pss_rsae_sha384") == 0) {
                sig_alg_list[i++] = MBEDTLS_TLS1_3_SIG_RSA_PSS_RSAE_SHA384;
            } else if (strcmp(q, "rsa_pss_rsae_sha512") == 0) {
                sig_alg_list[i++] = MBEDTLS_TLS1_3_SIG_RSA_PSS_RSAE_SHA512;
            } else if (strcmp(q, "ed25519") == 0) {
                sig_alg_list[i++] = MBEDTLS_TLS1_3_SIG_ED25519;
            } else if (strcmp(q, "ed448") == 0) {
                sig_alg_list[i++] = MBEDTLS_TLS1_3_SIG_ED448;
            } else if (strcmp(q, "rsa_pss_pss_sha256") == 0) {
                sig_alg_list[i++] = MBEDTLS_TLS1_3_SIG_RSA_PSS_PSS_SHA256;
            } else if (strcmp(q, "rsa_pss_pss_sha384") == 0) {
                sig_alg_list[i++] = MBEDTLS_TLS1_3_SIG_RSA_PSS_PSS_SHA384;
            } else if (strcmp(q, "rsa_pss_pss_sha512") == 0) {
                sig_alg_list[i++] = MBEDTLS_TLS1_3_SIG_RSA_PSS_PSS_SHA512;
            } else if (strcmp(q, "rsa_pkcs1_sha1") == 0) {
                sig_alg_list[i++] = MBEDTLS_TLS1_3_SIG_RSA_PKCS1_SHA1;
            } else if (strcmp(q, "ecdsa_sha1") == 0) {
                sig_alg_list[i++] = MBEDTLS_TLS1_3_SIG_ECDSA_SHA1;
            } else {
                ret = -1;
                mbedtls_printf("unknown signature algorithm \"%s\"\n", q);
                mbedtls_print_supported_sig_algs();
                goto exit;
            }
        }

        if (i == (SIG_ALG_LIST_SIZE - 1) && *p != '\0') {
            mbedtls_printf("signature algorithm list too long, maximum %d",
                           SIG_ALG_LIST_SIZE - 1);
            goto exit;
        }

        sig_alg_list[i] = MBEDTLS_TLS1_3_SIG_NONE;
    }
#endif

#if defined(MBEDTLS_SSL_ALPN)
    if (opt.alpn_string != NULL) {
        p = (char *) opt.alpn_string;
        i = 0;

        /* Leave room for a final NULL in alpn_list */
        while (i < ALPN_LIST_SIZE - 1 && *p != '\0') {
            alpn_list[i++] = p;

            /* Terminate the current string and move on to next one */
            while (*p != ',' && *p != '\0') {
                p++;
            }
            if (*p == ',') {
                *p++ = '\0';
            }
        }
    }
#endif /* MBEDTLS_SSL_ALPN */

    mbedtls_printf("build version: %s (build %d)\n",
                   MBEDTLS_VERSION_STRING_FULL, MBEDTLS_VERSION_NUMBER);

    /*
     * 0. Initialize the RNG and the session data
     */
    mbedtls_printf("\n  . Seeding the random number generator...");
    fflush(stdout);

    ret = rng_seed(opt.reproducible, pers);
    if (ret != 0) {
        goto exit;
    }
    mbedtls_printf(" ok\n");

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    /*
     * 1.1. Load the trusted CA
     */
    mbedtls_printf("  . Loading the CA root certificate ...");
    fflush(stdout);

    if (strcmp(opt.ca_path, "none") == 0 ||
        strcmp(opt.ca_file, "none") == 0) {
        ret = 0;
    } else
#if defined(MBEDTLS_FS_IO)
    if (strlen(opt.ca_path)) {
        ret = mbedtls_x509_crt_parse_path(&cacert, opt.ca_path);
    } else if (strlen(opt.ca_file)) {
        ret = mbedtls_x509_crt_parse_file(&cacert, opt.ca_file);
    } else
#endif
    {
#if defined(MBEDTLS_PEM_PARSE_C)
        for (i = 0; mbedtls_test_cas[i] != NULL; i++) {
            ret = mbedtls_x509_crt_parse(&cacert,
                                         (const unsigned char *) mbedtls_test_cas[i],
                                         mbedtls_test_cas_len[i]);
            if (ret != 0) {
                break;
            }
        }
#endif /* MBEDTLS_PEM_PARSE_C */
        if (ret == 0) {
            for (i = 0; mbedtls_test_cas_der[i] != NULL; i++) {
                ret = mbedtls_x509_crt_parse_der(&cacert,
                                                 (const unsigned char *) mbedtls_test_cas_der[i],
                                                 mbedtls_test_cas_der_len[i]);
                if (ret != 0) {
                    break;
                }
            }
        }
    }
    if (ret < 0) {
        mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n",
                       (unsigned int) -ret);
        goto exit;
    }

    mbedtls_printf(" ok (%d skipped)\n", ret);

    /*
     * 1.2. Load own certificate and private key
     */
    mbedtls_printf("  . Loading the server cert. and key...");
    fflush(stdout);

#if defined(MBEDTLS_FS_IO)
    if (strlen(opt.crt_file) && strcmp(opt.crt_file, "none") != 0) {
        key_cert_init++;
        if ((ret = mbedtls_x509_crt_parse_file(&srvcert, opt.crt_file)) != 0) {
            mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse_file returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }
    }
    if (strlen(opt.key_file) && strcmp(opt.key_file, "none") != 0) {
        key_cert_init++;
        if ((ret = mbedtls_pk_parse_keyfile(&pkey, opt.key_file,
                                            opt.key_pwd)) != 0) {
            mbedtls_printf(" failed\n  !  mbedtls_pk_parse_keyfile returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }
    }
    if (key_cert_init == 1) {
        mbedtls_printf(" failed\n  !  crt_file without key_file or vice-versa\n\n");
        goto exit;
    }

    if (strlen(opt.crt_file2) && strcmp(opt.crt_file2, "none") != 0) {
        key_cert_init2++;
        if ((ret = mbedtls_x509_crt_parse_file(&srvcert2, opt.crt_file2)) != 0) {
            mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse_file(2) returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }
    }
    if (strlen(opt.key_file2) && strcmp(opt.key_file2, "none") != 0) {
        key_cert_init2++;
        if ((ret = mbedtls_pk_parse_keyfile(&pkey2, opt.key_file2,
                                            opt.key_pwd2)) != 0) {
            mbedtls_printf(" failed\n  !  mbedtls_pk_parse_keyfile(2) returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }
    }
    if (key_cert_init2 == 1) {
        mbedtls_printf(" failed\n  !  crt_file2 without key_file2 or vice-versa\n\n");
        goto exit;
    }
#endif
    if (key_cert_init == 0 &&
        strcmp(opt.crt_file, "none") != 0 &&
        strcmp(opt.key_file, "none") != 0 &&
        key_cert_init2 == 0 &&
        strcmp(opt.crt_file2, "none") != 0 &&
        strcmp(opt.key_file2, "none") != 0) {
#if defined(MBEDTLS_RSA_C)
        if ((ret = mbedtls_x509_crt_parse(&srvcert,
                                          (const unsigned char *) mbedtls_test_srv_crt_rsa,
                                          mbedtls_test_srv_crt_rsa_len)) != 0) {
            mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }
        if ((ret = mbedtls_pk_parse_key(&pkey,
                                        (const unsigned char *) mbedtls_test_srv_key_rsa,
                                        mbedtls_test_srv_key_rsa_len, NULL, 0)) != 0) {
            mbedtls_printf(" failed\n  !  mbedtls_pk_parse_key returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }
        key_cert_init = 2;
#endif /* MBEDTLS_RSA_C */
#if defined(PSA_HAVE_ALG_SOME_ECDSA) && defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT)
        if ((ret = mbedtls_x509_crt_parse(&srvcert2,
                                          (const unsigned char *) mbedtls_test_srv_crt_ec,
                                          mbedtls_test_srv_crt_ec_len)) != 0) {
            mbedtls_printf(" failed\n  !  x509_crt_parse2 returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }
        if ((ret = mbedtls_pk_parse_key(&pkey2,
                                        (const unsigned char *) mbedtls_test_srv_key_ec,
                                        mbedtls_test_srv_key_ec_len, NULL, 0)) != 0) {
            mbedtls_printf(" failed\n  !  pk_parse_key2 returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }
        key_cert_init2 = 2;
#endif /* PSA_HAVE_ALG_SOME_ECDSA && PSA_WANT_KEY_TYPE_ECC_KEY_PAIR_IMPORT */
    }

    if (opt.key_opaque != 0) {
        psa_algorithm_t psa_alg, psa_alg2 = PSA_ALG_NONE;
        psa_key_usage_t psa_usage = 0;

        if (key_opaque_set_alg_usage(opt.key1_opaque_alg1,
                                     opt.key1_opaque_alg2,
                                     &psa_alg, &psa_alg2,
                                     &psa_usage,
                                     mbedtls_pk_get_type(&pkey)) == 0) {
            ret = pk_wrap_as_opaque(&pkey, psa_alg, psa_alg2, psa_usage, &key_slot);
            if (ret != 0) {
                mbedtls_printf(" failed\n  !  "
                               "pk_wrap_as_opaque returned -0x%x\n\n",
                               (unsigned int)  -ret);
                goto exit;
            }
        }

        psa_alg = PSA_ALG_NONE; psa_alg2 = PSA_ALG_NONE;
        psa_usage = 0;

        if (key_opaque_set_alg_usage(opt.key2_opaque_alg1,
                                     opt.key2_opaque_alg2,
                                     &psa_alg, &psa_alg2,
                                     &psa_usage,
                                     mbedtls_pk_get_type(&pkey2)) == 0) {
            ret = pk_wrap_as_opaque(&pkey2, psa_alg, psa_alg2, psa_usage, &key_slot2);
            if (ret != 0) {
                mbedtls_printf(" failed\n  !  "
                               "mbedtls_pk_get_psa_attributes returned -0x%x\n\n",
                               (unsigned int)  -ret);
                goto exit;
            }
        }
    }

    mbedtls_printf(" ok (key types: %s, %s)\n",
                   key_cert_init ? mbedtls_x509_pk_type_as_string(&pkey) : "none",
                   key_cert_init2 ? mbedtls_x509_pk_type_as_string(&pkey2) : "none");
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */

#if defined(SNI_OPTION)
    if (opt.sni != NULL) {
        mbedtls_printf("  . Setting up SNI information...");
        fflush(stdout);

        if ((sni_info = sni_parse(opt.sni)) == NULL) {
            mbedtls_printf(" failed\n");
            goto exit;
        }

        mbedtls_printf(" ok\n");
    }
#endif /* SNI_OPTION */

    /*
     * 2. Setup stuff
     */
    mbedtls_printf("  . Setting up the SSL/TLS structure...");
    fflush(stdout);

    if ((ret = mbedtls_ssl_config_defaults(&conf,
                                           MBEDTLS_SSL_IS_SERVER,
                                           opt.transport,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_config_defaults returned -0x%x\n\n",
                       (unsigned int) -ret);
        goto exit;
    }

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    /* The default algorithms profile disables SHA-1, but our tests still
       rely on it heavily. Hence we allow it here. A real-world server
       should use the default profile unless there is a good reason not to. */
    if (opt.allow_sha1 > 0) {
        crt_profile_for_test.allowed_mds |= MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA1);
        mbedtls_ssl_conf_cert_profile(&conf, &crt_profile_for_test);
        mbedtls_ssl_conf_sig_algs(&conf, ssl_sig_algs_for_test);
    }
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */

    if (opt.auth_mode != DFL_AUTH_MODE) {
        mbedtls_ssl_conf_authmode(&conf, opt.auth_mode);
    }

    if (opt.cert_req_ca_list != DFL_CERT_REQ_CA_LIST) {
        mbedtls_ssl_conf_cert_req_ca_list(&conf, opt.cert_req_ca_list);
    }

#if defined(MBEDTLS_SSL_EARLY_DATA)
    if (opt.early_data != DFL_EARLY_DATA) {
        mbedtls_ssl_conf_early_data(&conf, opt.early_data);
    }
    if (opt.max_early_data_size != DFL_MAX_EARLY_DATA_SIZE) {
        mbedtls_ssl_conf_max_early_data_size(
            &conf, opt.max_early_data_size);
    }
#endif /* MBEDTLS_SSL_EARLY_DATA */

#if defined(MBEDTLS_KEY_EXCHANGE_CERT_REQ_ALLOWED_ENABLED)
    /* exercise setting DN hints for server certificate request
     * (Intended for use where the client cert expected has been signed by
     *  a specific CA which is an intermediate in a CA chain, not the root) */
    if (opt.cert_req_dn_hint == 2 && key_cert_init2) {
        mbedtls_ssl_conf_dn_hints(&conf, &srvcert2);
    }
#endif

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if (opt.hs_to_min != DFL_HS_TO_MIN || opt.hs_to_max != DFL_HS_TO_MAX) {
        mbedtls_ssl_conf_handshake_timeout(&conf, opt.hs_to_min, opt.hs_to_max);
    }

    if (opt.dgram_packing != DFL_DGRAM_PACKING) {
        mbedtls_ssl_set_datagram_packing(&ssl, opt.dgram_packing);
    }
#endif /* MBEDTLS_SSL_PROTO_DTLS */

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    if ((ret = mbedtls_ssl_conf_max_frag_len(&conf, opt.mfl_code)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_conf_max_frag_len returned %d\n\n", ret);
        goto exit;
    }
#endif

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    if (opt.cid_enabled == 1 || opt.cid_enabled_renego == 1) {
        if (opt.cid_enabled == 1        &&
            opt.cid_enabled_renego == 1 &&
            cid_len != cid_renego_len) {
            mbedtls_printf("CID length must not change during renegotiation\n");
            goto usage;
        }

        if (opt.cid_enabled == 1) {
            ret = mbedtls_ssl_conf_cid(&conf, cid_len,
                                       MBEDTLS_SSL_UNEXPECTED_CID_IGNORE);
        } else {
            ret = mbedtls_ssl_conf_cid(&conf, cid_renego_len,
                                       MBEDTLS_SSL_UNEXPECTED_CID_IGNORE);
        }

        if (ret != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_conf_cid_len returned -%#04x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }
    }
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

#if defined(MBEDTLS_SSL_DTLS_SRTP)
    const mbedtls_ssl_srtp_profile forced_profile[] =
    { opt.force_srtp_profile, MBEDTLS_TLS_SRTP_UNSET };
    if (opt.use_srtp == 1) {
        if (opt.force_srtp_profile != 0) {
            ret = mbedtls_ssl_conf_dtls_srtp_protection_profiles(&conf, forced_profile);
        } else {
            ret = mbedtls_ssl_conf_dtls_srtp_protection_profiles(&conf, default_profiles);
        }

        if (ret != 0) {
            mbedtls_printf(
                " failed\n  ! mbedtls_ssl_conf_dtls_srtp_protection_profiles returned %d\n\n",
                ret);
            goto exit;
        }

        mbedtls_ssl_conf_srtp_mki_value_supported(&conf,
                                                  opt.support_mki ?
                                                  MBEDTLS_SSL_DTLS_SRTP_MKI_SUPPORTED :
                                                  MBEDTLS_SSL_DTLS_SRTP_MKI_UNSUPPORTED);

    } else if (opt.force_srtp_profile != 0) {
        mbedtls_printf(" failed\n  ! must enable use_srtp to force srtp profile\n\n");
        goto exit;
    }
#endif /* MBEDTLS_SSL_DTLS_SRTP */

#if defined(MBEDTLS_SSL_EXTENDED_MASTER_SECRET)
    if (opt.extended_ms != DFL_EXTENDED_MS) {
        mbedtls_ssl_conf_extended_master_secret(&conf, opt.extended_ms);
    }
#endif

#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
    if (opt.etm != DFL_ETM) {
        mbedtls_ssl_conf_encrypt_then_mac(&conf, opt.etm);
    }
#endif

#if defined(MBEDTLS_SSL_ALPN)
    if (opt.alpn_string != NULL) {
        if ((ret = mbedtls_ssl_conf_alpn_protocols(&conf, alpn_list)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_conf_alpn_protocols returned %d\n\n", ret);
            goto exit;
        }
    }
#endif

    if (opt.reproducible) {
#if defined(MBEDTLS_HAVE_TIME)
#if defined(MBEDTLS_PLATFORM_TIME_ALT)
        mbedtls_platform_set_time(dummy_constant_time);
#else
        fprintf(stderr, "Warning: reproducible option used without constant time\n");
#endif
#endif  /* MBEDTLS_HAVE_TIME */
    }
    mbedtls_ssl_conf_dbg(&conf, my_debug, stdout);

#if defined(MBEDTLS_SSL_CACHE_C)
    if (opt.cache_max != -1) {
        mbedtls_ssl_cache_set_max_entries(&cache, opt.cache_max);
    }

#if defined(MBEDTLS_HAVE_TIME)
    if (opt.cache_timeout != -1) {
        mbedtls_ssl_cache_set_timeout(&cache, opt.cache_timeout);
    }
#endif

    mbedtls_ssl_conf_session_cache(&conf, &cache,
                                   mbedtls_ssl_cache_get,
                                   mbedtls_ssl_cache_set);
#endif

#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_TICKET_C)
    if (opt.tickets != MBEDTLS_SSL_SESSION_TICKETS_DISABLED) {
#if defined(MBEDTLS_HAVE_TIME)
        if (opt.dummy_ticket) {
            mbedtls_ssl_conf_session_tickets_cb(&conf,
                                                dummy_ticket_write,
                                                dummy_ticket_parse,
                                                NULL);
        } else
#endif /* MBEDTLS_HAVE_TIME */
        {
            if ((ret = mbedtls_ssl_ticket_setup(&ticket_ctx,
                                                opt.ticket_alg,
                                                opt.ticket_key_type,
                                                opt.ticket_key_bits,
                                                opt.ticket_timeout)) != 0) {
                mbedtls_printf(
                    " failed\n  ! mbedtls_ssl_ticket_setup returned %d\n\n",
                    ret);
                goto exit;
            }

            mbedtls_ssl_conf_session_tickets_cb(&conf,
                                                mbedtls_ssl_ticket_write,
                                                mbedtls_ssl_ticket_parse,
                                                &ticket_ctx);
        }

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
        mbedtls_ssl_conf_new_session_tickets(&conf, opt.tickets);
#endif
        /* exercise manual ticket rotation (not required for typical use)
         * (used for external synchronization of session ticket encryption keys)
         */
        if (opt.ticket_rotate) {
            unsigned char kbuf[MBEDTLS_SSL_TICKET_MAX_KEY_BYTES];
            unsigned char name[MBEDTLS_SSL_TICKET_KEY_NAME_BYTES];
            if ((ret = psa_generate_random(name, sizeof(name))) != 0 ||
                (ret = psa_generate_random(kbuf, sizeof(kbuf))) != 0 ||
                (ret = mbedtls_ssl_ticket_rotate(&ticket_ctx,
                                                 name, sizeof(name), kbuf, sizeof(kbuf),
                                                 opt.ticket_timeout)) != 0) {
                mbedtls_printf(" failed\n  ! mbedtls_ssl_ticket_rotate returned %d\n\n", ret);
                goto exit;
            }
        }
    }
#endif

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if (opt.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
#if defined(MBEDTLS_SSL_COOKIE_C)
        if (opt.cookies > 0) {
            if ((ret = mbedtls_ssl_cookie_setup(&cookie_ctx)) != 0) {
                mbedtls_printf(" failed\n  ! mbedtls_ssl_cookie_setup returned %d\n\n", ret);
                goto exit;
            }

            mbedtls_ssl_conf_dtls_cookies(&conf, mbedtls_ssl_cookie_write, mbedtls_ssl_cookie_check,
                                          &cookie_ctx);
        } else
#endif /* MBEDTLS_SSL_COOKIE_C */
#if defined(MBEDTLS_SSL_DTLS_HELLO_VERIFY)
        if (opt.cookies == 0) {
            mbedtls_ssl_conf_dtls_cookies(&conf, NULL, NULL, NULL);
        } else
#endif /* MBEDTLS_SSL_DTLS_HELLO_VERIFY */
        {
            ; /* Nothing to do */
        }

#if defined(MBEDTLS_SSL_DTLS_ANTI_REPLAY)
        if (opt.anti_replay != DFL_ANTI_REPLAY) {
            mbedtls_ssl_conf_dtls_anti_replay(&conf, opt.anti_replay);
        }
#endif

        if (opt.badmac_limit != DFL_BADMAC_LIMIT) {
            mbedtls_ssl_conf_dtls_badmac_limit(&conf, opt.badmac_limit);
        }
    }
#endif /* MBEDTLS_SSL_PROTO_DTLS */

    if (opt.force_ciphersuite[0] != DFL_FORCE_CIPHER) {
        mbedtls_ssl_conf_ciphersuites(&conf, opt.force_ciphersuite);
    }

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    mbedtls_ssl_conf_tls13_key_exchange_modes(&conf, opt.tls13_kex_modes);
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

    if (opt.allow_legacy != DFL_ALLOW_LEGACY) {
        mbedtls_ssl_conf_legacy_renegotiation(&conf, opt.allow_legacy);
    }
#if defined(MBEDTLS_SSL_RENEGOTIATION)
    mbedtls_ssl_conf_renegotiation(&conf, opt.renegotiation);

    if (opt.renego_delay != DFL_RENEGO_DELAY) {
        mbedtls_ssl_conf_renegotiation_enforced(&conf, opt.renego_delay);
    }

    if (opt.renego_period != DFL_RENEGO_PERIOD) {
        PUT_UINT64_BE(renego_period, opt.renego_period, 0);
        mbedtls_ssl_conf_renegotiation_period(&conf, renego_period);
    }
#endif

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    if (strcmp(opt.ca_path, "none") != 0 &&
        strcmp(opt.ca_file, "none") != 0) {
#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
        if (opt.ca_callback != 0) {
            mbedtls_ssl_conf_ca_cb(&conf, ca_callback, &cacert);
        } else
#endif
        mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    }
    if (key_cert_init) {
        mbedtls_pk_context *pk = &pkey;
#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
        if (opt.async_private_delay1 >= 0) {
            ret = ssl_async_set_key(&ssl_async_keys, &srvcert, pk, 0,
                                    opt.async_private_delay1);
            if (ret < 0) {
                mbedtls_printf("  Test error: ssl_async_set_key failed (%d)\n",
                               ret);
                goto exit;
            }
            pk = NULL;
        }
#endif /* MBEDTLS_SSL_ASYNC_PRIVATE */
        if ((ret = mbedtls_ssl_conf_own_cert(&conf, &srvcert, pk)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret);
            goto exit;
        }
    }
    if (key_cert_init2) {
        mbedtls_pk_context *pk = &pkey2;
#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
        if (opt.async_private_delay2 >= 0) {
            ret = ssl_async_set_key(&ssl_async_keys, &srvcert2, pk, 0,
                                    opt.async_private_delay2);
            if (ret < 0) {
                mbedtls_printf("  Test error: ssl_async_set_key failed (%d)\n",
                               ret);
                goto exit;
            }
            pk = NULL;
        }
#endif /* MBEDTLS_SSL_ASYNC_PRIVATE */
        if ((ret = mbedtls_ssl_conf_own_cert(&conf, &srvcert2, pk)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n", ret);
            goto exit;
        }
    }

#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
    if (opt.async_operations[0] != '-') {
        mbedtls_ssl_async_sign_t *sign = NULL;
        const char *r;
        for (r = opt.async_operations; *r; r++) {
            switch (*r) {
                case 's':
                    sign = ssl_async_sign;
                    break;
            }
        }
        ssl_async_keys.inject_error = (opt.async_private_error < 0 ?
                                       -opt.async_private_error :
                                       opt.async_private_error);
        mbedtls_ssl_conf_async_private_cb(&conf,
                                          sign,
                                          ssl_async_resume,
                                          ssl_async_cancel,
                                          &ssl_async_keys);
    }
#endif /* MBEDTLS_SSL_ASYNC_PRIVATE */
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */

#if defined(SNI_OPTION)
    if (opt.sni != NULL) {
        mbedtls_ssl_conf_sni(&conf, sni_callback, sni_info);
        mbedtls_ssl_conf_cert_cb(&conf, cert_callback);
#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
        if (opt.async_private_delay2 >= 0) {
            sni_entry *cur;
            for (cur = sni_info; cur != NULL; cur = cur->next) {
                ret = ssl_async_set_key(&ssl_async_keys,
                                        cur->cert, cur->key, 1,
                                        opt.async_private_delay2);
                if (ret < 0) {
                    mbedtls_printf("  Test error: ssl_async_set_key failed (%d)\n",
                                   ret);
                    goto exit;
                }
                cur->key = NULL;
            }
        }
#endif /* MBEDTLS_SSL_ASYNC_PRIVATE */
    }
#endif

#if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) || \
    (defined(MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_SOME_EPHEMERAL_ENABLED) && \
    defined(PSA_WANT_ALG_FFDH))
    if (opt.groups != NULL &&
        strcmp(opt.groups, "default") != 0) {
        mbedtls_ssl_conf_groups(&conf, group_list);
    }
#endif

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    if (opt.sig_algs != NULL) {
        mbedtls_ssl_conf_sig_algs(&conf, sig_alg_list);
    }
#endif

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED)

    if (strlen(opt.psk) != 0 && strlen(opt.psk_identity) != 0) {
        if (opt.psk_opaque != 0) {
            /* The algorithm has already been determined earlier. */
            status = psa_setup_psk_key_slot(&psk_slot, alg, psk, psk_len);
            if (status != PSA_SUCCESS) {
                fprintf(stderr, "SETUP FAIL\n");
                ret = MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
                goto exit;
            }
            if ((ret = mbedtls_ssl_conf_psk_opaque(&conf, psk_slot,
                                                   (const unsigned char *) opt.psk_identity,
                                                   strlen(opt.psk_identity))) != 0) {
                mbedtls_printf(" failed\n  ! mbedtls_ssl_conf_psk_opaque returned %d\n\n",
                               ret);
                goto exit;
            }
        } else
        if (psk_len > 0) {
            ret = mbedtls_ssl_conf_psk(&conf, psk, psk_len,
                                       (const unsigned char *) opt.psk_identity,
                                       strlen(opt.psk_identity));
            if (ret != 0) {
                mbedtls_printf("  failed\n  mbedtls_ssl_conf_psk returned -0x%04X\n\n",
                               (unsigned int) -ret);
                goto exit;
            }
        }
    }

    if (opt.psk_list != NULL) {
        if (opt.psk_list_opaque != 0) {
            psk_entry *cur_psk;
            for (cur_psk = psk_info; cur_psk != NULL; cur_psk = cur_psk->next) {

                status = psa_setup_psk_key_slot(&cur_psk->slot, alg,
                                                cur_psk->key,
                                                cur_psk->key_len);
                if (status != PSA_SUCCESS) {
                    ret = MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
                    goto exit;
                }
            }
        }

        mbedtls_ssl_conf_psk_cb(&conf, psk_callback, psk_info);
    }
#endif

    if (opt.min_version != DFL_MIN_VERSION) {
        mbedtls_ssl_conf_min_tls_version(&conf, opt.min_version);
    }

    if (opt.max_version != DFL_MIN_VERSION) {
        mbedtls_ssl_conf_max_tls_version(&conf, opt.max_version);
    }

    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_setup returned -0x%x\n\n", (unsigned int) -ret);
        goto exit;
    }

    if (opt.eap_tls != 0) {
        mbedtls_ssl_set_export_keys_cb(&ssl, eap_tls_key_derivation,
                                       &eap_tls_keying);
    } else if (opt.nss_keylog != 0) {
        mbedtls_ssl_set_export_keys_cb(&ssl,
                                       nss_keylog_export,
                                       NULL);
    }
#if defined(MBEDTLS_SSL_DTLS_SRTP)
    else if (opt.use_srtp != 0) {
        mbedtls_ssl_set_export_keys_cb(&ssl, dtls_srtp_key_derivation,
                                       &dtls_srtp_keying);
    }
#endif /* MBEDTLS_SSL_DTLS_SRTP */

    io_ctx.ssl = &ssl;
    io_ctx.net = &client_fd;
    mbedtls_ssl_set_bio(&ssl, &io_ctx, send_cb, recv_cb,
                        opt.nbio == 0 ? recv_timeout_cb : NULL);

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    if (opt.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
        if ((ret = mbedtls_ssl_set_cid(&ssl, opt.cid_enabled,
                                       cid, cid_len)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_set_cid returned %d\n\n",
                           ret);
            goto exit;
        }
    }
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if (opt.dtls_mtu != DFL_DTLS_MTU) {
        mbedtls_ssl_set_mtu(&ssl, opt.dtls_mtu);
    }
#endif

#if defined(MBEDTLS_TIMING_C)
    mbedtls_ssl_set_timer_cb(&ssl, &timer, mbedtls_timing_set_delay,
                             mbedtls_timing_get_delay);
#endif

    mbedtls_printf(" ok\n");

    /*
     * 3. Setup the listening TCP socket
     */
    mbedtls_printf("  . Bind on %s://%s:%s/ ...",
                   opt.transport == MBEDTLS_SSL_TRANSPORT_STREAM ? "tcp" : "udp",
                   opt.server_addr ? opt.server_addr : "*",
                   opt.server_port);
    fflush(stdout);

    if ((ret = mbedtls_net_bind(&listen_fd, opt.server_addr, opt.server_port,
                                opt.transport == MBEDTLS_SSL_TRANSPORT_STREAM ?
                                MBEDTLS_NET_PROTO_TCP : MBEDTLS_NET_PROTO_UDP)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_net_bind returned -0x%x\n\n", (unsigned int) -ret);
        goto exit;
    }
    mbedtls_printf(" ok\n");

reset:
#if !defined(_WIN32)
    if (received_sigterm) {
        mbedtls_printf(" interrupted by SIGTERM (not in net_accept())\n");
        if (ret == MBEDTLS_ERR_NET_INVALID_CONTEXT) {
            ret = 0;
        }

        goto exit;
    }
#endif

    if (ret == MBEDTLS_ERR_SSL_CLIENT_RECONNECT) {
        mbedtls_printf("  ! Client initiated reconnection from same port\n");
        goto handshake;
    }

#ifdef MBEDTLS_ERROR_C
    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, 100);
        mbedtls_printf("Last error was: %d - %s\n\n", ret, error_buf);
    }
#endif

    mbedtls_net_free(&client_fd);

    mbedtls_ssl_session_reset(&ssl);

    /*
     * 3. Wait until a client connects
     */
    mbedtls_printf("  . Waiting for a remote connection ...");
    fflush(stdout);

    if ((ret = mbedtls_net_accept(&listen_fd, &client_fd,
                                  client_ip, sizeof(client_ip), &cliip_len)) != 0) {
#if !defined(_WIN32)
        if (received_sigterm) {
            mbedtls_printf(" interrupted by SIGTERM (in net_accept())\n");
            if (ret == MBEDTLS_ERR_NET_ACCEPT_FAILED) {
                ret = 0;
            }

            goto exit;
        }
#endif

        mbedtls_printf(" failed\n  ! mbedtls_net_accept returned -0x%x\n\n", (unsigned int) -ret);
        goto exit;
    }

    if (opt.nbio > 0) {
        ret = mbedtls_net_set_nonblock(&client_fd);
    } else {
        ret = mbedtls_net_set_block(&client_fd);
    }
    if (ret != 0) {
        mbedtls_printf(" failed\n  ! net_set_(non)block() returned -0x%x\n\n", (unsigned int) -ret);
        goto exit;
    }

    mbedtls_ssl_conf_read_timeout(&conf, opt.read_timeout);

#if defined(MBEDTLS_SSL_DTLS_HELLO_VERIFY)
    if (opt.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
        if ((ret = mbedtls_ssl_set_client_transport_id(&ssl,
                                                       client_ip, cliip_len)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_set_client_transport_id() returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }
    }
#endif /* MBEDTLS_SSL_DTLS_HELLO_VERIFY */

#if defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
    if (opt.ecjpake_pw != DFL_ECJPAKE_PW) {
        if (opt.ecjpake_pw_opaque != DFL_ECJPAKE_PW_OPAQUE) {
            psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

            psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DERIVE);
            psa_set_key_algorithm(&attributes, PSA_ALG_JPAKE(PSA_ALG_SHA_256));
            psa_set_key_type(&attributes, PSA_KEY_TYPE_PASSWORD);

            status = psa_import_key(&attributes,
                                    (const unsigned char *) opt.ecjpake_pw,
                                    strlen(opt.ecjpake_pw),
                                    &ecjpake_pw_slot);
            if (status != PSA_SUCCESS) {
                mbedtls_printf(" failed\n  ! psa_import_key returned %d\n\n",
                               status);
                goto exit;
            }
            if ((ret = mbedtls_ssl_set_hs_ecjpake_password_opaque(&ssl,
                                                                  ecjpake_pw_slot)) != 0) {
                mbedtls_printf(
                    " failed\n  ! mbedtls_ssl_set_hs_ecjpake_password_opaque returned %d\n\n",
                    ret);
                goto exit;
            }
            mbedtls_printf("using opaque password\n");
        } else {
            if ((ret = mbedtls_ssl_set_hs_ecjpake_password(&ssl,
                                                           (const unsigned char *) opt.ecjpake_pw,
                                                           strlen(opt.ecjpake_pw))) != 0) {
                mbedtls_printf(" failed\n  ! mbedtls_ssl_set_hs_ecjpake_password returned %d\n\n",
                               ret);
                goto exit;
            }
        }
    }
#endif /* MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED */

#if defined(MBEDTLS_SSL_SERVER_NAME_INDICATION)
#if defined(MBEDTLS_KEY_EXCHANGE_CERT_REQ_ALLOWED_ENABLED)
    /* exercise setting DN hints for server certificate request
     * (Intended for use where the client cert expected has been signed by
     *  a specific CA which is an intermediate in a CA chain, not the root)
     * (Additionally, the CA choice would typically be influenced by SNI
     *  if being set per-handshake using mbedtls_ssl_set_hs_dn_hints()) */
    if (opt.cert_req_dn_hint == 3 && key_cert_init2) {
        mbedtls_ssl_set_hs_dn_hints(&ssl, &srvcert2);
    }
#endif
#endif

    mbedtls_printf(" ok\n");

    /*
     * 4. Handshake
     */
handshake:
    mbedtls_printf("  . Performing the SSL/TLS handshake...");
    fflush(stdout);

    while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
#if defined(MBEDTLS_SSL_EARLY_DATA)
        if (ret == MBEDTLS_ERR_SSL_RECEIVED_EARLY_DATA) {
            memset(buf, 0, opt.buffer_size);
            ret = mbedtls_ssl_read_early_data(&ssl, buf, opt.buffer_size);
            if (ret > 0) {
                buf[ret] = '\0';
                mbedtls_printf(" %d early data bytes read\n\n%s\n",
                               ret, (char *) buf);
            }
            continue;
        }
#endif /* MBEDTLS_SSL_EARLY_DATA */

#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
        if (ret == MBEDTLS_ERR_SSL_ASYNC_IN_PROGRESS &&
            ssl_async_keys.inject_error == SSL_ASYNC_INJECT_ERROR_CANCEL) {
            mbedtls_printf(" cancelling on injected error\n");
            break;
        }
#endif /* MBEDTLS_SSL_ASYNC_PRIVATE */

        if (!mbedtls_status_is_ssl_in_progress(ret)) {
            break;
        }

        /* For event-driven IO, wait for socket to become available */
        if (opt.event == 1 /* level triggered IO */) {
#if defined(MBEDTLS_TIMING_C)
            ret = idle(&client_fd, &timer, ret);
#else
            ret = idle(&client_fd, ret);
#endif
            if (ret != 0) {
                goto reset;
            }
        }
    }

    if (ret == MBEDTLS_ERR_SSL_HELLO_VERIFY_REQUIRED) {
        mbedtls_printf(" hello verification requested\n");
        ret = 0;
        goto reset;
    } else if (ret != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n",
                       (unsigned int) -ret);

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
        if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED ||
            ret == MBEDTLS_ERR_SSL_BAD_CERTIFICATE) {
            char vrfy_buf[512];
            flags = mbedtls_ssl_get_verify_result(&ssl);

            x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);

            mbedtls_printf("%s\n", vrfy_buf);
        }
#endif

#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
        if (opt.async_private_error < 0) {
            /* Injected error only the first time round, to test reset */
            ssl_async_keys.inject_error = SSL_ASYNC_INJECT_ERROR_NONE;
        }
#endif
        goto reset;
    } else { /* ret == 0 */
        int suite_id = mbedtls_ssl_get_ciphersuite_id_from_ssl(&ssl);
        const mbedtls_ssl_ciphersuite_t *ciphersuite_info;
        ciphersuite_info = mbedtls_ssl_ciphersuite_from_id(suite_id);

        mbedtls_printf(" ok\n    [ Protocol is %s ]\n"
                       "    [ Ciphersuite is %s ]\n"
                       "    [ Key size is %u ]\n",
                       mbedtls_ssl_get_version(&ssl),
                       mbedtls_ssl_ciphersuite_get_name(ciphersuite_info),
                       (unsigned int)
                       mbedtls_ssl_ciphersuite_get_cipher_key_bitlen(ciphersuite_info));
    }

    if ((ret = mbedtls_ssl_get_record_expansion(&ssl)) >= 0) {
        mbedtls_printf("    [ Record expansion is %d ]\n", ret);
    } else {
        mbedtls_printf("    [ Record expansion is unknown ]\n");
    }

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH) || defined(MBEDTLS_SSL_RECORD_SIZE_LIMIT)
    mbedtls_printf("    [ Maximum incoming record payload length is %u ]\n",
                   (unsigned int) mbedtls_ssl_get_max_in_record_payload(&ssl));
    mbedtls_printf("    [ Maximum outgoing record payload length is %u ]\n",
                   (unsigned int) mbedtls_ssl_get_max_out_record_payload(&ssl));
#endif

#if defined(MBEDTLS_SSL_ALPN)
    if (opt.alpn_string != NULL) {
        const char *alp = mbedtls_ssl_get_alpn_protocol(&ssl);
        mbedtls_printf("    [ Application Layer Protocol is %s ]\n",
                       alp ? alp : "(none)");
    }
#endif

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    /*
     * 5. Verify the client certificate
     */
    mbedtls_printf("  . Verifying peer X.509 certificate...");
    fflush(stdout);

    if ((flags = mbedtls_ssl_get_verify_result(&ssl)) != 0) {
        char vrfy_buf[512];

        mbedtls_printf(" failed\n");

        x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);
        mbedtls_printf("%s\n", vrfy_buf);
    } else {
        mbedtls_printf(" ok\n");
    }

#if !defined(MBEDTLS_X509_REMOVE_INFO)
    if (mbedtls_ssl_get_peer_cert(&ssl) != NULL) {
        char crt_buf[512];

        mbedtls_printf("  . Peer certificate information    ...\n");
        fflush(stdout);
        mbedtls_x509_crt_info(crt_buf, sizeof(crt_buf), "      ",
                              mbedtls_ssl_get_peer_cert(&ssl));
        mbedtls_printf("%s\n", crt_buf);
    }
#endif /* MBEDTLS_X509_REMOVE_INFO */
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */

    if (opt.eap_tls != 0) {
        size_t j = 0;

        if ((ret = mbedtls_ssl_tls_prf(eap_tls_keying.tls_prf_type,
                                       eap_tls_keying.master_secret,
                                       sizeof(eap_tls_keying.master_secret),
                                       eap_tls_label,
                                       eap_tls_keying.randbytes,
                                       sizeof(eap_tls_keying.randbytes),
                                       eap_tls_keymaterial,
                                       sizeof(eap_tls_keymaterial)))
            != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_tls_prf returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto reset;
        }

        mbedtls_printf("    EAP-TLS key material is:");
        for (j = 0; j < sizeof(eap_tls_keymaterial); j++) {
            if (j % 8 == 0) {
                mbedtls_printf("\n    ");
            }
            mbedtls_printf("%02x ", eap_tls_keymaterial[j]);
        }
        mbedtls_printf("\n");

        if ((ret = mbedtls_ssl_tls_prf(eap_tls_keying.tls_prf_type, NULL, 0,
                                       eap_tls_label,
                                       eap_tls_keying.randbytes,
                                       sizeof(eap_tls_keying.randbytes),
                                       eap_tls_iv,
                                       sizeof(eap_tls_iv))) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_tls_prf returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto reset;
        }

        mbedtls_printf("    EAP-TLS IV is:");
        for (j = 0; j < sizeof(eap_tls_iv); j++) {
            if (j % 8 == 0) {
                mbedtls_printf("\n    ");
            }
            mbedtls_printf("%02x ", eap_tls_iv[j]);
        }
        mbedtls_printf("\n");
    }

#if defined(MBEDTLS_SSL_KEYING_MATERIAL_EXPORT)
    if (opt.exp_label != NULL && opt.exp_len > 0) {
        unsigned char *exported_key = mbedtls_calloc((size_t) opt.exp_len, sizeof(unsigned char));
        if (exported_key == NULL) {
            mbedtls_printf("Could not allocate %d bytes\n", opt.exp_len);
            ret = 3;
            goto exit;
        }
        ret = mbedtls_ssl_export_keying_material(&ssl, exported_key, (size_t) opt.exp_len,
                                                 opt.exp_label, strlen(opt.exp_label),
                                                 NULL, 0, 0);
        if (ret != 0) {
            mbedtls_free(exported_key);
            goto exit;
        }
        mbedtls_printf("Exporting key of length %d with label \"%s\": 0x",
                       opt.exp_len,
                       opt.exp_label);
        for (i = 0; i < opt.exp_len; i++) {
            mbedtls_printf("%02X", exported_key[i]);
        }
        mbedtls_printf("\n\n");
        fflush(stdout);
        mbedtls_free(exported_key);
    }
#endif /* defined(MBEDTLS_SSL_KEYING_MATERIAL_EXPORT) */

#if defined(MBEDTLS_SSL_DTLS_SRTP)
    else if (opt.use_srtp != 0) {
        size_t j = 0;
        mbedtls_dtls_srtp_info dtls_srtp_negotiation_result;
        mbedtls_ssl_get_dtls_srtp_negotiation_result(&ssl, &dtls_srtp_negotiation_result);

        if (dtls_srtp_negotiation_result.chosen_dtls_srtp_profile
            == MBEDTLS_TLS_SRTP_UNSET) {
            mbedtls_printf("    Unable to negotiate "
                           "the use of DTLS-SRTP\n");
        } else {
            if ((ret = mbedtls_ssl_tls_prf(dtls_srtp_keying.tls_prf_type,
                                           dtls_srtp_keying.master_secret,
                                           sizeof(dtls_srtp_keying.master_secret),
                                           dtls_srtp_label,
                                           dtls_srtp_keying.randbytes,
                                           sizeof(dtls_srtp_keying.randbytes),
                                           dtls_srtp_key_material,
                                           sizeof(dtls_srtp_key_material)))
                != 0) {
                mbedtls_printf(" failed\n  ! mbedtls_ssl_tls_prf returned -0x%x\n\n",
                               (unsigned int) -ret);
                goto exit;
            }

            mbedtls_printf("    DTLS-SRTP key material is:");
            for (j = 0; j < sizeof(dtls_srtp_key_material); j++) {
                if (j % 8 == 0) {
                    mbedtls_printf("\n    ");
                }
                mbedtls_printf("%02x ", dtls_srtp_key_material[j]);
            }
            mbedtls_printf("\n");

            /* produce a less readable output used to perform automatic checks
             * - compare client and server output
             * - interop test with openssl which client produces this kind of output
             */
            mbedtls_printf("    Keying material: ");
            for (j = 0; j < sizeof(dtls_srtp_key_material); j++) {
                mbedtls_printf("%02X", dtls_srtp_key_material[j]);
            }
            mbedtls_printf("\n");

            if (dtls_srtp_negotiation_result.mki_len > 0) {
                mbedtls_printf("    DTLS-SRTP mki value: ");
                for (j = 0; j < dtls_srtp_negotiation_result.mki_len; j++) {
                    mbedtls_printf("%02X", dtls_srtp_negotiation_result.mki_value[j]);
                }
            } else {
                mbedtls_printf("    DTLS-SRTP no mki value negotiated");
            }
            mbedtls_printf("\n");

        }
    }
#endif /* MBEDTLS_SSL_DTLS_SRTP */

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    ret = report_cid_usage(&ssl, "initial handshake");
    if (ret != 0) {
        goto exit;
    }

    if (opt.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
        if ((ret = mbedtls_ssl_set_cid(&ssl, opt.cid_enabled_renego,
                                       cid_renego, cid_renego_len)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_set_cid returned %d\n\n",
                           ret);
            goto exit;
        }
    }
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

#if defined(MBEDTLS_MEMORY_DEBUG)
    mbedtls_memory_buffer_alloc_cur_get(&current_heap_memory, &heap_blocks);
    mbedtls_memory_buffer_alloc_max_get(&peak_heap_memory, &heap_blocks);
    mbedtls_printf("Heap memory usage after handshake: %lu bytes. Peak memory usage was %lu\n",
                   (unsigned long) current_heap_memory, (unsigned long) peak_heap_memory);
#endif  /* MBEDTLS_MEMORY_DEBUG */

    if (opt.exchanges == 0) {
        goto close_notify;
    }

    exchanges_left = opt.exchanges;
data_exchange:
    /*
     * 6. Read the HTTP Request
     */
    mbedtls_printf("  < Read from client:");
    fflush(stdout);

    /*
     * TLS and DTLS need different reading styles (stream vs datagram)
     */
    if (opt.transport == MBEDTLS_SSL_TRANSPORT_STREAM) {
        do {
            int terminated = 0;
            len = opt.buffer_size;
            memset(buf, 0, opt.buffer_size);
            ret = mbedtls_ssl_read(&ssl, buf, len);

            if (mbedtls_status_is_ssl_in_progress(ret)) {
                if (opt.event == 1 /* level triggered IO */) {
#if defined(MBEDTLS_TIMING_C)
                    idle(&client_fd, &timer, ret);
#else
                    idle(&client_fd, ret);
#endif
                }

                continue;
            }

            if (ret <= 0) {
                switch (ret) {
                    case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
                        mbedtls_printf(" connection was closed gracefully\n");
                        goto close_notify;

                    case 0:
                    case MBEDTLS_ERR_NET_CONN_RESET:
                        mbedtls_printf(" connection was reset by peer\n");
                        ret = MBEDTLS_ERR_NET_CONN_RESET;
                        goto reset;

                    default:
                        mbedtls_printf(" mbedtls_ssl_read returned -0x%x\n", (unsigned int) -ret);
                        goto reset;
                }
            }

            if (mbedtls_ssl_get_bytes_avail(&ssl) == 0) {
                len = ret;
                buf[len] = '\0';
                mbedtls_printf(" %d bytes read\n\n%s\n", len, (char *) buf);

                /* End of message should be detected according to the syntax of the
                 * application protocol (eg HTTP), just use a dummy test here. */
                if (buf[len - 1] == '\n') {
                    terminated = 1;
                }
            } else {
                int extra_len, ori_len;
                unsigned char *larger_buf;

                ori_len = ret;
                extra_len = (int) mbedtls_ssl_get_bytes_avail(&ssl);

                larger_buf = mbedtls_calloc(1, ori_len + extra_len + 1);
                if (larger_buf == NULL) {
                    mbedtls_printf("  ! memory allocation failed\n");
                    ret = 1;
                    goto reset;
                }

                memset(larger_buf, 0, ori_len + extra_len);
                memcpy(larger_buf, buf, ori_len);

                /* This read should never fail and get the whole cached data */
                ret = mbedtls_ssl_read(&ssl, larger_buf + ori_len, extra_len);
                if (ret != extra_len ||
                    mbedtls_ssl_get_bytes_avail(&ssl) != 0) {
                    mbedtls_printf("  ! mbedtls_ssl_read failed on cached data\n");
                    ret = 1;
                    goto reset;
                }

                larger_buf[ori_len + extra_len] = '\0';
                mbedtls_printf(" %d bytes read (%d + %d)\n\n%s\n",
                               ori_len + extra_len, ori_len, extra_len,
                               (char *) larger_buf);

                /* End of message should be detected according to the syntax of the
                 * application protocol (eg HTTP), just use a dummy test here. */
                if (larger_buf[ori_len + extra_len - 1] == '\n') {
                    terminated = 1;
                }

                mbedtls_free(larger_buf);
            }

            if (terminated) {
                ret = 0;
                break;
            }
        } while (1);
    } else { /* Not stream, so datagram */
        len = opt.buffer_size;
        memset(buf, 0, opt.buffer_size);

        do {
            /* Without the call to `mbedtls_ssl_check_pending`, it might
             * happen that the client sends application data in the same
             * datagram as the Finished message concluding the handshake.
             * In this case, the application data would be ready to be
             * processed while the underlying transport wouldn't signal
             * any further incoming data.
             *
             * See the test 'Event-driven I/O: session-id resume, UDP packing'
             * in tests/ssl-opt.sh.
             */

            /* For event-driven IO, wait for socket to become available */
            if (mbedtls_ssl_check_pending(&ssl) == 0 &&
                opt.event == 1 /* level triggered IO */) {
#if defined(MBEDTLS_TIMING_C)
                idle(&client_fd, &timer, MBEDTLS_ERR_SSL_WANT_READ);
#else
                idle(&client_fd, MBEDTLS_ERR_SSL_WANT_READ);
#endif
            }

            ret = mbedtls_ssl_read(&ssl, buf, len);

            /* Note that even if `mbedtls_ssl_check_pending` returns true,
             * it can happen that the subsequent call to `mbedtls_ssl_read`
             * returns `MBEDTLS_ERR_SSL_WANT_READ`, because the pending messages
             * might be discarded (e.g. because they are retransmissions). */
        } while (mbedtls_status_is_ssl_in_progress(ret));

        if (ret <= 0) {
            switch (ret) {
                case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
                    mbedtls_printf(" connection was closed gracefully\n");
                    goto close_notify;

                default:
                    mbedtls_printf(" mbedtls_ssl_read returned -0x%x\n", (unsigned int) -ret);
                    goto reset;
            }
        }

        len = ret;
        buf[len] = '\0';
        mbedtls_printf(" %d bytes read\n\n%s", len, (char *) buf);
        ret = 0;
    }

    /*
     * 7a. Request renegotiation while client is waiting for input from us.
     * (only on the first exchange, to be able to test retransmission)
     */
#if defined(MBEDTLS_SSL_RENEGOTIATION)
    if (opt.renegotiate && exchanges_left == opt.exchanges) {
        mbedtls_printf("  . Requestion renegotiation...");
        fflush(stdout);

        while ((ret = mbedtls_ssl_renegotiate(&ssl)) != 0) {
            if (!mbedtls_status_is_ssl_in_progress(ret)) {
                mbedtls_printf(" failed\n  ! mbedtls_ssl_renegotiate returned %d\n\n", ret);
                goto reset;
            }

            /* For event-driven IO, wait for socket to become available */
            if (opt.event == 1 /* level triggered IO */) {
#if defined(MBEDTLS_TIMING_C)
                idle(&client_fd, &timer, ret);
#else
                idle(&client_fd, ret);
#endif
            }
        }

        mbedtls_printf(" ok\n");
    }
#endif /* MBEDTLS_SSL_RENEGOTIATION */

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    ret = report_cid_usage(&ssl, "after renegotiation");
    if (ret != 0) {
        goto exit;
    }
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

    /*
     * 7. Write the 200 Response
     */
    mbedtls_printf("  > Write to client:");
    fflush(stdout);

    /* If the format of the response changes, make sure there is enough
     * room in buf (buf_content_size calculation above). */
    len = sprintf((char *) buf, HTTP_RESPONSE,
                  mbedtls_ssl_get_ciphersuite(&ssl));

    /* Add padding to the response to reach opt.response_size in length */
    if (opt.response_size != DFL_RESPONSE_SIZE &&
        len < opt.response_size) {
        memset(buf + len, 'B', opt.response_size - len);
        len += opt.response_size - len;
    }

    /* Truncate if response size is smaller than the "natural" size */
    if (opt.response_size != DFL_RESPONSE_SIZE &&
        len > opt.response_size) {
        len = opt.response_size;

        /* Still end with \r\n unless that's really not possible */
        if (len >= 2) {
            buf[len - 2] = '\r';
        }
        if (len >= 1) {
            buf[len - 1] = '\n';
        }
    }

    if (opt.transport == MBEDTLS_SSL_TRANSPORT_STREAM) {
        for (written = 0, frags = 0; written < len; written += ret, frags++) {
            while ((ret = mbedtls_ssl_write(&ssl, buf + written, len - written))
                   <= 0) {
                if (ret == MBEDTLS_ERR_NET_CONN_RESET) {
                    mbedtls_printf(" failed\n  ! peer closed the connection\n\n");
                    goto reset;
                }

                if (!mbedtls_status_is_ssl_in_progress(ret)) {
                    mbedtls_printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret);
                    goto reset;
                }

                /* For event-driven IO, wait for socket to become available */
                if (opt.event == 1 /* level triggered IO */) {
#if defined(MBEDTLS_TIMING_C)
                    idle(&client_fd, &timer, ret);
#else
                    idle(&client_fd, ret);
#endif
                }
            }
        }
    } else { /* Not stream, so datagram */
        while (1) {
            ret = mbedtls_ssl_write(&ssl, buf, len);

            if (!mbedtls_status_is_ssl_in_progress(ret)) {
                break;
            }

            /* For event-driven IO, wait for socket to become available */
            if (opt.event == 1 /* level triggered IO */) {
#if defined(MBEDTLS_TIMING_C)
                idle(&client_fd, &timer, ret);
#else
                idle(&client_fd, ret);
#endif
            }
        }

        if (ret < 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n", ret);
            goto reset;
        }

        frags = 1;
        written = ret;
    }

    buf[written] = '\0';
    mbedtls_printf(" %d bytes written in %d fragments\n\n%s\n", written, frags, (char *) buf);
    ret = 0;

    /*
     * 7b. Simulate serialize/deserialize and go back to data exchange
     */
#if defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION)
    if (opt.serialize != 0) {
        size_t buf_len;

        mbedtls_printf("  . Serializing live connection...");
        fflush(stdout);

        ret = mbedtls_ssl_context_save(&ssl, NULL, 0, &buf_len);
        if (ret != MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_context_save returned "
                           "-0x%x\n\n", (unsigned int) -ret);

            goto exit;
        }

        if ((context_buf = mbedtls_calloc(1, buf_len)) == NULL) {
            mbedtls_printf(" failed\n  ! Couldn't allocate buffer for "
                           "serialized context");

            goto exit;
        }
        context_buf_len = buf_len;

        if ((ret = mbedtls_ssl_context_save(&ssl, context_buf,
                                            buf_len, &buf_len)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_context_save returned "
                           "-0x%x\n\n", (unsigned int) -ret);

            goto exit;
        }

        mbedtls_printf(" ok\n");

        /* Save serialized context to the 'opt.context_file' as a base64 code */
        if (0 < strlen(opt.context_file)) {
            FILE *b64_file;
            uint8_t *b64_buf;
            size_t b64_len;

            mbedtls_printf("  . Save serialized context to a file... ");
            fflush(stdout);

            mbedtls_base64_encode(NULL, 0, &b64_len, context_buf, buf_len);

            if ((b64_buf = mbedtls_calloc(1, b64_len)) == NULL) {
                mbedtls_printf("failed\n  ! Couldn't allocate buffer for "
                               "the base64 code\n");
                goto exit;
            }

            if ((ret = mbedtls_base64_encode(b64_buf, b64_len, &b64_len,
                                             context_buf, buf_len)) != 0) {
                mbedtls_printf("failed\n  ! mbedtls_base64_encode returned "
                               "-0x%x\n", (unsigned int) -ret);
                mbedtls_free(b64_buf);
                goto exit;
            }

            if ((b64_file = fopen(opt.context_file, "w")) == NULL) {
                mbedtls_printf("failed\n  ! Cannot open '%s' for writing.\n",
                               opt.context_file);
                mbedtls_free(b64_buf);
                goto exit;
            }

            if (b64_len != fwrite(b64_buf, 1, b64_len, b64_file)) {
                mbedtls_printf("failed\n  ! fwrite(%ld bytes) failed\n",
                               (long) b64_len);
                mbedtls_free(b64_buf);
                fclose(b64_file);
                goto exit;
            }

            mbedtls_free(b64_buf);
            fclose(b64_file);

            mbedtls_printf("ok\n");
        }

        /*
         * This simulates a workflow where you have a long-lived server
         * instance, potentially with a pool of ssl_context objects, and you
         * just want to re-use one while the connection is inactive: in that
         * case you can just reset() it, and then it's ready to receive
         * serialized data from another connection (or the same here).
         */
        if (opt.serialize == 1) {
            /* nothing to do here, done by context_save() already */
            mbedtls_printf("  . Context has been reset... ok\n");
            fflush(stdout);
        }

        /*
         * This simulates a workflow where you have one server instance per
         * connection, and want to release it entire when the connection is
         * inactive, and spawn it again when needed again - this would happen
         * between ssl_free() and ssl_init() below, together with any other
         * teardown/startup code needed - for example, preparing the
         * ssl_config again (see section 3 "setup stuff" in this file).
         */
        if (opt.serialize == 2) {
            mbedtls_printf("  . Freeing and reinitializing context...");
            fflush(stdout);

            mbedtls_ssl_free(&ssl);

            mbedtls_ssl_init(&ssl);

            if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
                mbedtls_printf(" failed\n  ! mbedtls_ssl_setup returned "
                               "-0x%x\n\n", (unsigned int) -ret);
                goto exit;
            }

            /*
             * This illustrates the minimum amount of things you need to set
             * up, however you could set up much more if desired, for example
             * if you want to share your set up code between the case of
             * establishing a new connection and this case.
             */
            if (opt.nbio == 2) {
                mbedtls_ssl_set_bio(&ssl, &client_fd, delayed_send,
                                    delayed_recv, NULL);
            } else {
                mbedtls_ssl_set_bio(&ssl, &client_fd, mbedtls_net_send,
                                    mbedtls_net_recv,
                                    opt.nbio == 0 ? mbedtls_net_recv_timeout : NULL);
            }

#if defined(MBEDTLS_TIMING_C)
            mbedtls_ssl_set_timer_cb(&ssl, &timer,
                                     mbedtls_timing_set_delay,
                                     mbedtls_timing_get_delay);
#endif /* MBEDTLS_TIMING_C */

            mbedtls_printf(" ok\n");
        }

        mbedtls_printf("  . Deserializing connection...");
        fflush(stdout);

        if ((ret = mbedtls_ssl_context_load(&ssl, context_buf,
                                            buf_len)) != 0) {
            mbedtls_printf("failed\n  ! mbedtls_ssl_context_load returned "
                           "-0x%x\n\n", (unsigned int) -ret);

            goto exit;
        }

        mbedtls_free(context_buf);
        context_buf = NULL;
        context_buf_len = 0;

        mbedtls_printf(" ok\n");
    }
#endif /* MBEDTLS_SSL_CONTEXT_SERIALIZATION */

    /*
     * 7c. Continue doing data exchanges?
     */
    if (--exchanges_left > 0) {
        goto data_exchange;
    }

    /*
     * 8. Done, cleanly close the connection
     */
close_notify:
    mbedtls_printf("  . Closing the connection...");
    fflush(stdout);

    /* No error checking, the connection might be closed already */
    do {
        ret = mbedtls_ssl_close_notify(&ssl);
    } while (ret == MBEDTLS_ERR_SSL_WANT_WRITE);
    ret = 0;

    mbedtls_printf(" done\n");

#if defined(MBEDTLS_SSL_CACHE_C)
    if (opt.cache_remove > 0) {
        mbedtls_ssl_cache_remove(&cache, ssl.session->id, ssl.session->id_len);
    }
#endif

    goto reset;

    /*
     * Cleanup and exit
     */
exit:
#ifdef MBEDTLS_ERROR_C
    if (ret != 0) {
        char error_buf[100];
        mbedtls_strerror(ret, error_buf, 100);
        mbedtls_printf("Last error was: -0x%X - %s\n\n", (unsigned int) -ret, error_buf);
    }
#endif

    if (opt.query_config_mode == DFL_QUERY_CONFIG_MODE) {
        mbedtls_printf("  . Cleaning up...");
        fflush(stdout);
    }

    mbedtls_net_free(&client_fd);
    mbedtls_net_free(&listen_fd);

    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);

#if defined(MBEDTLS_SSL_CACHE_C)
    mbedtls_ssl_cache_free(&cache);
#endif
#if defined(MBEDTLS_SSL_SESSION_TICKETS) && defined(MBEDTLS_SSL_TICKET_C)
    mbedtls_ssl_ticket_free(&ticket_ctx);
#endif
#if defined(MBEDTLS_SSL_COOKIE_C)
    mbedtls_ssl_cookie_free(&cookie_ctx);
#endif

#if defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION)
    if (context_buf != NULL) {
        mbedtls_platform_zeroize(context_buf, context_buf_len);
    }
    mbedtls_free(context_buf);
#endif

#if defined(SNI_OPTION)
    sni_free(sni_info);
#endif

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED)
    ret = psk_free(psk_info);
    if ((ret != 0) && (opt.query_config_mode == DFL_QUERY_CONFIG_MODE)) {
        mbedtls_printf("Failed to list of opaque PSKs - error was %d\n", ret);
    }
#endif

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    mbedtls_x509_crt_free(&cacert);
    mbedtls_x509_crt_free(&srvcert);
    mbedtls_pk_free(&pkey);
    mbedtls_x509_crt_free(&srvcert2);
    mbedtls_pk_free(&pkey2);
    psa_destroy_key(key_slot);
    psa_destroy_key(key_slot2);
#endif

#if defined(MBEDTLS_SSL_ASYNC_PRIVATE)
    for (i = 0; (size_t) i < ssl_async_keys.slots_used; i++) {
        if (ssl_async_keys.slots[i].pk_owned) {
            mbedtls_pk_free(ssl_async_keys.slots[i].pk);
            mbedtls_free(ssl_async_keys.slots[i].pk);
            ssl_async_keys.slots[i].pk = NULL;
        }
    }
#endif

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED)
    if (opt.psk_opaque != 0) {
        /* This is ok even if the slot hasn't been
         * initialized (we might have jumed here
         * immediately because of bad cmd line params,
         * for example). */
        status = psa_destroy_key(psk_slot);
        if ((status != PSA_SUCCESS) &&
            (opt.query_config_mode == DFL_QUERY_CONFIG_MODE)) {
            mbedtls_printf("Failed to destroy key slot %u - error was %d",
                           (unsigned) MBEDTLS_SVC_KEY_ID_GET_KEY_ID(psk_slot),
                           (int) status);
        }
    }
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED */

#if defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
    /*
     * In case opaque keys it's the user responsibility to keep the key valid
     * for the duration of the handshake and destroy it at the end
     */
    if ((opt.ecjpake_pw_opaque != DFL_ECJPAKE_PW_OPAQUE)) {
        psa_key_attributes_t check_attributes = PSA_KEY_ATTRIBUTES_INIT;

        /* Verify that the key is still valid before destroying it */
        if (psa_get_key_attributes(ecjpake_pw_slot, &check_attributes) !=
            PSA_SUCCESS) {
            if (ret == 0) {
                ret = 1;
            }
            mbedtls_printf("The EC J-PAKE password key has unexpectedly been already destroyed\n");
        } else {
            psa_destroy_key(ecjpake_pw_slot);
        }
    }
#endif  /* MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED */

    const char *message = mbedtls_test_helper_is_psa_leaking();
    if (message) {
        if (ret == 0) {
            ret = 1;
        }
        mbedtls_printf("PSA memory leak detected: %s\n",  message);
    }

    mbedtls_psa_crypto_free();
    mbedtls_free(buf);

#if defined(MBEDTLS_TEST_HOOKS)
    /* Let test hooks detect errors such as resource leaks.
     * Don't do it in query_config mode, because some test code prints
     * information to stdout and this gets mixed with the regular output. */
    if (opt.query_config_mode == DFL_QUERY_CONFIG_MODE) {
        if (test_hooks_failure_detected()) {
            if (ret == 0) {
                ret = 1;
            }
            mbedtls_printf("Test hooks detected errors.\n");
        }
    }
    test_hooks_free();
#endif /* MBEDTLS_TEST_HOOKS */

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
#if defined(MBEDTLS_MEMORY_DEBUG)
    mbedtls_memory_buffer_alloc_status();
#endif
    mbedtls_memory_buffer_alloc_free();
#endif  /* MBEDTLS_MEMORY_BUFFER_ALLOC_C */

    if (opt.query_config_mode == DFL_QUERY_CONFIG_MODE) {
        mbedtls_printf(" done.\n");
    }

    // Shell can not handle large exit numbers -> 1 for errors
    if (ret < 0) {
        ret = 1;
    }

    if (opt.query_config_mode == DFL_QUERY_CONFIG_MODE) {
        mbedtls_exit(ret);
    } else {
        mbedtls_exit(query_config_ret);
    }
}
#endif /* !MBEDTLS_SSL_TEST_IMPOSSIBLE && MBEDTLS_SSL_SRV_C */
