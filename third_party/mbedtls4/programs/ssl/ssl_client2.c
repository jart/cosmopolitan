/*
 *  SSL client with certificate authentication
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
#elif !defined(MBEDTLS_SSL_CLI_C)
int main(void)
{
    mbedtls_printf("MBEDTLS_SSL_CLI_C not defined.\n");
    mbedtls_exit(0);
}
#else /* !MBEDTLS_SSL_TEST_IMPOSSIBLE && MBEDTLS_SSL_CLI_C */

#include "test/psa_crypto_helpers.h"

/* Size of memory to be allocated for the heap, when using the library's memory
 * management and MBEDTLS_MEMORY_BUFFER_ALLOC_C is enabled. */
#define MEMORY_HEAP_SIZE      120000

#define MAX_REQUEST_SIZE      20000
#define MAX_REQUEST_SIZE_STR "20000"

#define DFL_SERVER_NAME         "localhost"
#define DFL_SERVER_ADDR         NULL
#define DFL_SERVER_PORT         "4433"
#define DFL_REQUEST_PAGE        "/"
#define DFL_REQUEST_SIZE        -1
#define DFL_DEBUG_LEVEL         0
#define DFL_CONTEXT_CRT_CB      0
#define DFL_NBIO                0
#define DFL_EVENT               0
#define DFL_READ_TIMEOUT        0
#define DFL_MAX_RESEND          0
#define DFL_CA_FILE             ""
#define DFL_CA_PATH             ""
#define DFL_CRT_FILE            ""
#define DFL_KEY_FILE            ""
#define DFL_KEY_OPAQUE          0
#define DFL_KEY_PWD             ""
#define DFL_PSK                 ""
#define DFL_EARLY_DATA          -1
#define DFL_PSK_OPAQUE          0
#define DFL_PSK_IDENTITY        "Client_identity"
#define DFL_ECJPAKE_PW          NULL
#define DFL_ECJPAKE_PW_OPAQUE   0
#define DFL_EC_MAX_OPS          -1
#define DFL_FORCE_CIPHER        0
#define DFL_TLS1_3_KEX_MODES    MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_ALL
#define DFL_RENEGOTIATION       MBEDTLS_SSL_RENEGOTIATION_DISABLED
#define DFL_ALLOW_LEGACY        -2
#define DFL_RENEGOTIATE         0
#define DFL_EXCHANGES           1
#define DFL_MIN_VERSION         -1
#define DFL_MAX_VERSION         -1
#define DFL_SHA1                -1
#define DFL_AUTH_MODE           -1
#define DFL_SET_HOSTNAME        1
#define DFL_MFL_CODE            MBEDTLS_SSL_MAX_FRAG_LEN_NONE
#define DFL_TRUNC_HMAC          -1
#define DFL_RECSPLIT            -1
#define DFL_RECONNECT           0
#define DFL_RECO_SERVER_NAME    NULL
#define DFL_RECO_DELAY          0
#define DFL_RECO_MODE           1
#define DFL_RENEGO_DELAY        -2
#define DFL_CID_ENABLED         0
#define DFL_CID_VALUE           ""
#define DFL_CID_ENABLED_RENEGO  -1
#define DFL_CID_VALUE_RENEGO    NULL
#define DFL_RECONNECT_HARD      0
#define DFL_TICKETS             MBEDTLS_SSL_SESSION_TICKETS_ENABLED
#define DFL_ALPN_STRING         NULL
#define DFL_GROUPS              NULL
#define DFL_SIG_ALGS            NULL
#define DFL_TRANSPORT           MBEDTLS_SSL_TRANSPORT_STREAM
#define DFL_HS_TO_MIN           0
#define DFL_HS_TO_MAX           0
#define DFL_DTLS_MTU            -1
#define DFL_DGRAM_PACKING        1
#define DFL_FALLBACK            -1
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
#define DFL_SKIP_CLOSE_NOTIFY   0
#define DFL_EXP_LABEL           NULL
#define DFL_EXP_LEN             20
#define DFL_QUERY_CONFIG_MODE   0
#define DFL_USE_SRTP            0
#define DFL_SRTP_FORCE_PROFILE  0
#define DFL_SRTP_MKI            ""
#define DFL_KEY_OPAQUE_ALG      "none"

#define GET_REQUEST "GET %s HTTP/1.0\r\nHost: %s\r\nExtra-header: "
#define GET_REQUEST_END "\r\n\r\n"

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
#define USAGE_CONTEXT_CRT_CB \
    "    context_crt_cb=%%d   This determines whether the CRT verification callback is bound\n" \
    "                        to the SSL configuration of the SSL context.\n" \
    "                        Possible values:\n" \
    "                        - 0 (default): Use CRT callback bound to configuration\n" \
    "                        - 1: Use CRT callback bound to SSL context\n"
#else
#define USAGE_CONTEXT_CRT_CB ""
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */
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
    "                        default: \"\" (pre-loaded)\n" \
    "    key_file=%%s         default: \"\" (pre-loaded)\n" \
    "    key_pwd=%%s          Password for key specified by key_file argument\n" \
    "                        default: none\n"
#else
#define USAGE_IO \
    "    No file operations available (MBEDTLS_FS_IO not defined)\n"
#endif /* MBEDTLS_FS_IO */
#else /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */
#define USAGE_IO ""
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */
#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
#define USAGE_KEY_OPAQUE \
    "    key_opaque=%%d       Handle your private key as if it were opaque\n" \
    "                        default: 0 (disabled)\n"
#else
#define USAGE_KEY_OPAQUE ""
#endif

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

#if defined(MBEDTLS_SSL_SESSION_TICKETS)
#define USAGE_TICKETS                                       \
    "    tickets=%%d          default: 1 (enabled)\n"
#else
#define USAGE_TICKETS ""
#endif /* MBEDTLS_SSL_SESSION_TICKETS */

#define USAGE_EAP_TLS                                       \
    "    eap_tls=%%d          default: 0 (disabled)\n"
#define USAGE_NSS_KEYLOG                                    \
    "    nss_keylog=%%d          default: 0 (disabled)\n"               \
    "                             This cannot be used with eap_tls=1\n"
#define USAGE_NSS_KEYLOG_FILE                               \
    "    nss_keylog_file=%%s\n"
#if defined(MBEDTLS_SSL_DTLS_SRTP)
#define USAGE_SRTP \
    "    use_srtp=%%d         default: 0 (disabled)\n" \
    "                          This cannot be used with eap_tls=1 or " \
    "                          nss_keylog=1\n"             \
    "    srtp_force_profile=%%d  default: 0 (all enabled)\n"   \
    "                        available profiles:\n"       \
    "                        1 - SRTP_AES128_CM_HMAC_SHA1_80\n"  \
    "                        2 - SRTP_AES128_CM_HMAC_SHA1_32\n"  \
    "                        3 - SRTP_NULL_HMAC_SHA1_80\n"       \
    "                        4 - SRTP_NULL_HMAC_SHA1_32\n"       \
    "    mki=%%s              default: \"\" (in hex, without 0x)\n"
#else /* MBEDTLS_SSL_DTLS_SRTP */
#define USAGE_SRTP ""
#endif

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

#if defined(PSA_WANT_KEY_TYPE_ECC_PUBLIC_KEY) || \
    (defined(MBEDTLS_SSL_TLS1_3_KEY_EXCHANGE_MODE_SOME_EPHEMERAL_ENABLED) && \
    defined(PSA_WANT_ALG_FFDH))
#define USAGE_GROUPS \
    "    groups=a,b,c,d      default: \"default\" (library default)\n"        \
    "                        example: \"secp521r1,brainpoolP512r1\"\n"        \
    "                        - use \"none\" for empty list\n"                 \
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
    "    renego_delay=%%d     default: -2 (library default)\n"
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

#if defined(MBEDTLS_ECP_RESTARTABLE)
#define USAGE_ECRESTART \
    "    ec_max_ops=%%s       default: library default (restart disabled)\n"
#else
#define USAGE_ECRESTART ""
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

#if defined(MBEDTLS_SSL_EARLY_DATA)
#define USAGE_EARLY_DATA \
    "    early_data=%%d      default: library default\n" \
    "                        options: 0 (disabled), 1 (enabled)\n"
#else
#define USAGE_EARLY_DATA ""
#endif /* MBEDTLS_SSL_EARLY_DATA && MBEDTLS_SSL_PROTO_TLS1_3 */

#define USAGE_KEY_OPAQUE_ALGS \
    "    key_opaque_algs=%%s  Allowed opaque key algorithms.\n"                        \
    "                        comma-separated pair of values among the following:\n"    \
    "                        rsa-sign-pkcs1, rsa-sign-pss, rsa-sign-pss-sha256,\n"     \
    "                        rsa-sign-pss-sha384, rsa-sign-pss-sha512,\n"              \
    "                        ecdsa-sign, ecdh, none (only acceptable for\n"            \
    "                        the second value).\n"                                     \

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
#define USAGE_TLS1_3_KEY_EXCHANGE_MODES \
    "    tls13_kex_modes=%%s   default: all\n"     \
    "                          options: psk, psk_ephemeral, psk_all, ephemeral,\n"     \
    "                                   ephemeral_all, all, psk_or_ephemeral\n"
#else
#define USAGE_TLS1_3_KEY_EXCHANGE_MODES ""
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

#if defined(MBEDTLS_SSL_KEYING_MATERIAL_EXPORT)
#define USAGE_EXPORT \
    "    exp_label=%%s       Label to input into TLS-Exporter\n" \
    "                         default: None (don't try to export a key)\n" \
    "    exp_len=%%d         Length of key to extract from TLS-Exporter \n" \
    "                         default: 20\n"
#else
#define USAGE_EXPORT ""
#endif /* defined(MBEDTLS_SSL_KEYING_MATERIAL_EXPORT) */

/* USAGE is arbitrarily split to stay under the portable string literal
 * length limit: 4095 bytes in C99. */
#define USAGE1 \
    "\n usage: ssl_client2 param=<>...\n"                   \
    "\n acceptable parameters:\n"                           \
    "    server_name=%%s      default: localhost\n"         \
    "    server_addr=%%s      default: given by name\n"     \
    "    server_port=%%d      default: 4433\n"              \
    "    request_page=%%s     default: \".\"\n"             \
    "    request_size=%%d     default: about 34 (basic request)\n"           \
    "                        (minimum: 0, max: " MAX_REQUEST_SIZE_STR ")\n"  \
                                                                      "                        If 0, in the first exchange only an empty\n"    \
                                                                      "                        application data message is sent followed by\n" \
                                                                      "                        a second non-empty message before attempting\n" \
                                                                      "                        to read a response from the server\n"           \
                                                                      "    debug_level=%%d      default: 0 (disabled)\n"             \
                                                                      "    build_version=%%d    default: none (disabled)\n"                     \
                                                                      "                        option: 1 (print build version only and stop)\n" \
                                                                      "    nbio=%%d             default: 0 (blocking I/O)\n"         \
                                                                      "                        options: 1 (non-blocking), 2 (added delays)\n"   \
                                                                      "    event=%%d            default: 0 (loop)\n"                            \
                                                                      "                        options: 1 (level-triggered, implies nbio=1),\n" \
                                                                      "    read_timeout=%%d     default: 0 ms (no timeout)\n"        \
                                                                      "    max_resend=%%d       default: 0 (no resend on timeout)\n" \
                                                                      "    skip_close_notify=%%d default: 0 (send close_notify)\n" \
                                                                      "\n"                                                    \
    USAGE_DTLS                                              \
    USAGE_CID                                               \
    USAGE_SRTP                                              \
    "\n"
#define USAGE2 \
    "    auth_mode=%%s        default: (library default: none)\n" \
    "                        options: none, optional, required\n" \
    "    set_hostname=%%s     call mbedtls_ssl_set_hostname()?" \
    "                        options: no, server_name, NULL\n" \
    "                        default: server_name (but ignored if certs disabled)\n"  \
    USAGE_IO                                                \
    USAGE_KEY_OPAQUE                                        \
    USAGE_CA_CALLBACK                                       \
    "\n"                                                    \
    USAGE_PSK                                               \
    USAGE_ECJPAKE                                           \
    USAGE_ECRESTART                                         \
    "\n"
#define USAGE3 \
    "    allow_legacy=%%d     default: (library default: no)\n"   \
    USAGE_RENEGO                                            \
    "    exchanges=%%d        default: 1\n"                 \
    "    reconnect=%%d        number of reconnections using session resumption\n" \
    "                        default: 0 (disabled)\n"       \
    "    reco_server_name=%%s  default: NULL\n"             \
    "    reco_delay=%%d       default: 0 milliseconds\n"         \
    "    reco_mode=%%d        0: copy session, 1: serialize session\n" \
    "                        default: 1\n"      \
    "    reconnect_hard=%%d   default: 0 (disabled)\n"      \
    USAGE_TICKETS                                           \
    USAGE_EAP_TLS                                           \
    USAGE_MAX_FRAG_LEN                                      \
    USAGE_CONTEXT_CRT_CB                                    \
    USAGE_ALPN                                              \
    USAGE_EMS                                               \
    USAGE_ETM                                               \
    USAGE_REPRODUCIBLE                                      \
    USAGE_GROUPS                                            \
    USAGE_SIG_ALGS                                          \
    USAGE_EARLY_DATA                                        \
    USAGE_KEY_OPAQUE_ALGS                                   \
    "\n"

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
#define TLS1_3_VERSION_OPTIONS  ", tls13"
#else /* MBEDTLS_SSL_PROTO_TLS1_3 */
#define TLS1_3_VERSION_OPTIONS  ""
#endif /* !MBEDTLS_SSL_PROTO_TLS1_3 */

#define USAGE4 \
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

/*
 * global options
 */
struct options {
    const char *server_name;    /* hostname of the server (client only)     */
    const char *server_addr;    /* address of the server (client only)      */
    const char *server_port;    /* port on which the ssl service runs       */
    int debug_level;            /* level of debugging                       */
    int nbio;                   /* should I/O be blocking?                  */
    int event;                  /* loop or event-driven IO? level or edge triggered? */
    uint32_t read_timeout;      /* timeout on mbedtls_ssl_read() in milliseconds     */
    int max_resend;             /* DTLS times to resend on read timeout     */
    const char *request_page;   /* page on server to request                */
    int request_size;           /* pad request with header to requested size */
    const char *ca_file;        /* the file with the CA certificate(s)      */
    const char *ca_path;        /* the path with the CA certificate(s) reside */
    const char *crt_file;       /* the file with the client certificate     */
    const char *key_file;       /* the file with the client key             */
    int key_opaque;             /* handle private key as if it were opaque  */
    int psk_opaque;
#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
    int ca_callback;            /* Use callback for trusted certificate list */
#endif
    const char *key_pwd;        /* the password for the client key          */
    const char *psk;            /* the pre-shared key                       */
    const char *psk_identity;   /* the pre-shared key identity              */
    const char *ecjpake_pw;     /* the EC J-PAKE password                   */
    int ecjpake_pw_opaque;      /* set to 1 to use the opaque method for setting the password */
    int ec_max_ops;             /* EC consecutive operations limit          */
    int force_ciphersuite[2];   /* protocol/ciphersuite to use, or all      */
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    int tls13_kex_modes;        /* supported TLS 1.3 key exchange modes     */
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */
    int renegotiation;          /* enable / disable renegotiation           */
    int allow_legacy;           /* allow legacy renegotiation               */
    int renegotiate;            /* attempt renegotiation?                   */
    int renego_delay;           /* delay before enforcing renegotiation     */
    int exchanges;              /* number of data exchanges                 */
    int min_version;            /* minimum protocol version accepted        */
    int max_version;            /* maximum protocol version accepted        */
    int allow_sha1;             /* flag for SHA-1 support                   */
    int auth_mode;              /* verify mode for connection               */
    int set_hostname;           /* call mbedtls_ssl_set_hostname()?         */
                                /* 0=no, 1=yes, -1=NULL */
    unsigned char mfl_code;     /* code for maximum fragment length         */
    int trunc_hmac;             /* negotiate truncated hmac or not          */
    int recsplit;               /* enable record splitting?                 */
    int reconnect;              /* attempt to resume session                */
    const char *reco_server_name;     /* hostname of the server (re-connect)     */
    int reco_delay;             /* delay in seconds before resuming session */
    int reco_mode;              /* how to keep the session around           */
    int reconnect_hard;         /* unexpectedly reconnect from the same port */
    int tickets;                /* enable / disable session tickets         */
    const char *groups;         /* list of supported groups                 */
    const char *sig_algs;       /* supported TLS 1.3 signature algorithms   */
    const char *alpn_string;    /* ALPN supported protocols                 */
    int transport;              /* TLS or DTLS?                             */
    uint32_t hs_to_min;         /* Initial value of DTLS handshake timer    */
    uint32_t hs_to_max;         /* Max value of DTLS handshake timer        */
    int dtls_mtu;               /* UDP Maximum transport unit for DTLS       */
    int fallback;               /* is this a fallback connection?           */
    int dgram_packing;          /* allow/forbid datagram packing            */
    int extended_ms;            /* negotiate extended master secret?        */
    int etm;                    /* negotiate encrypt then mac?              */
    int context_crt_cb;         /* use context-specific CRT verify callback */
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
    int skip_close_notify;      /* skip sending the close_notify alert      */
    const char *exp_label;      /* label to input into mbedtls_ssl_export_keying_material() */
    int exp_len;                /* Length of key to export using mbedtls_ssl_export_keying_material() */
#if defined(MBEDTLS_SSL_EARLY_DATA)
    int early_data;             /* early data enablement flag               */
#endif
    int query_config_mode;      /* whether to read config                   */
    int use_srtp;               /* Support SRTP                             */
    int force_srtp_profile;     /* SRTP protection profile to use or all    */
    const char *mki;            /* The dtls mki value to use                */
    const char *key_opaque_alg1; /* Allowed opaque key alg 1                */
    const char *key_opaque_alg2; /* Allowed Opaque key alg 2                */
} opt;

#include "ssl_test_common_source.c"

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
static unsigned char peer_crt_info[1024];

/*
 * Enabled if debug_level > 1 in code below
 */
static int my_verify(void *data, mbedtls_x509_crt *crt,
                     int depth, uint32_t *flags)
{
    char buf[1024];
    ((void) data);

    mbedtls_printf("\nVerify requested for (Depth %d):\n", depth);

#if !defined(MBEDTLS_X509_REMOVE_INFO)
    mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);
    if (depth == 0) {
        memcpy(peer_crt_info, buf, sizeof(buf));
    }

    if (opt.debug_level == 0) {
        return 0;
    }

    mbedtls_printf("%s", buf);
#else
    ((void) crt);
    ((void) depth);
#endif

    if ((*flags) == 0) {
        mbedtls_printf("  This certificate has no flags\n");
    } else {
        x509_crt_verify_info(buf, sizeof(buf), "  ! ", *flags);
        mbedtls_printf("%s\n", buf);
    }

    return 0;
}
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */

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

    /* Check if the use of a CID has been negotiated,
     * but don't ask for the CID value and length.
     *
     * Note: Here and below, we're demonstrating the various ways
     *       in which mbedtls_ssl_get_peer_cid() can be called,
     *       depending on whether or not the length/value of the
     *       peer's CID is needed.
     *
     *       An actual application, however, should use
     *       just one call to mbedtls_ssl_get_peer_cid(). */
    ret = mbedtls_ssl_get_peer_cid(ssl, &cid_negotiated,
                                   NULL, NULL);
    if (ret != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_get_peer_cid returned -0x%x\n\n",
                       (unsigned int) -ret);
        return ret;
    }

    if (cid_negotiated == MBEDTLS_SSL_CID_DISABLED) {
        if (opt.cid_enabled == MBEDTLS_SSL_CID_ENABLED) {
            mbedtls_printf("(%s) Use of Connection ID was rejected by the server.\n",
                           additional_description);
        }
    } else {
        size_t idx = 0;
        mbedtls_printf("(%s) Use of Connection ID has been negotiated.\n",
                       additional_description);

        /* Ask for just the length of the peer's CID. */
        ret = mbedtls_ssl_get_peer_cid(ssl, &cid_negotiated,
                                       NULL, &peer_cid_len);
        if (ret != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_get_peer_cid returned -0x%x\n\n",
                           (unsigned int) -ret);
            return ret;
        }

        /* Ask for just length + value of the peer's CID. */
        ret = mbedtls_ssl_get_peer_cid(ssl, &cid_negotiated,
                                       peer_cid, &peer_cid_len);
        if (ret != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_get_peer_cid returned -0x%x\n\n",
                           (unsigned int) -ret);
            return ret;
        }
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

static int ssl_save_session_serialize(mbedtls_ssl_context *ssl,
                                      unsigned char **session_data,
                                      size_t *session_data_len)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    mbedtls_ssl_session exported_session;

    /* free any previously saved data */
    if (*session_data != NULL) {
        mbedtls_platform_zeroize(*session_data, *session_data_len);
        mbedtls_free(*session_data);
        *session_data = NULL;
        *session_data_len = 0;
    }

    mbedtls_ssl_session_init(&exported_session);
    ret = mbedtls_ssl_get_session(ssl, &exported_session);
    if (ret != 0) {
        mbedtls_printf(
            "failed\n  ! mbedtls_ssl_get_session() returned -%#02x\n",
            (unsigned) -ret);
        goto exit;
    }

    /* get size of the buffer needed */
    (void) mbedtls_ssl_session_save(&exported_session, NULL, 0, session_data_len);
    *session_data = mbedtls_calloc(1, *session_data_len);
    if (*session_data == NULL) {
        mbedtls_printf(" failed\n  ! alloc %u bytes for session data\n",
                       (unsigned) *session_data_len);
        ret = MBEDTLS_ERR_SSL_ALLOC_FAILED;
        goto exit;
    }

    /* actually save session data */
    if ((ret = mbedtls_ssl_session_save(&exported_session,
                                        *session_data, *session_data_len,
                                        session_data_len)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_session_saved returned -0x%04x\n\n",
                       (unsigned int) -ret);
        goto exit;
    }

exit:
    mbedtls_ssl_session_free(&exported_session);
    return ret;
}

/*
 * Build HTTP request
 */
static int build_http_request(unsigned char *buf, size_t buf_size, size_t *request_len)
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t len, tail_len, request_size;

    ret = mbedtls_snprintf((char *) buf, buf_size, GET_REQUEST, opt.request_page, opt.server_name);
    if (ret < 0) {
        return ret;
    }

    len = (size_t) ret;
    tail_len = strlen(GET_REQUEST_END);
    if (opt.request_size != DFL_REQUEST_SIZE) {
        request_size = (size_t) opt.request_size;
    } else {
        request_size = len + tail_len;
    }

    if (request_size > buf_size) {
        return MBEDTLS_ERR_SSL_BUFFER_TOO_SMALL;
    }

    /* Add padding to GET request to reach opt.request_size in length */
    if (opt.request_size != DFL_REQUEST_SIZE &&
        len + tail_len < request_size) {
        memset(buf + len, 'A', request_size - len - tail_len);
        len = request_size - tail_len;
    }

    strncpy((char *) buf + len, GET_REQUEST_END, buf_size - len);
    len += tail_len;

    /* Truncate if request size is smaller than the "natural" size */
    if (opt.request_size != DFL_REQUEST_SIZE &&
        len > request_size) {
        len = request_size;

        /* Still end with \r\n unless that's really not possible */
        if (len >= 2) {
            buf[len - 2] = '\r';
        }
        if (len >= 1) {
            buf[len - 1] = '\n';
        }
    }

    *request_len = len;

    return 0;
}

int main(int argc, char *argv[])
{
    int ret = 0, i;
    size_t len, written, frags, retry_left;
    int query_config_ret = 0;
    mbedtls_net_context server_fd;
    io_ctx_t io_ctx;

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    uint16_t sig_alg_list[SIG_ALG_LIST_SIZE];
#endif

    unsigned char buf[MAX_REQUEST_SIZE + 1];

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED)
    unsigned char psk[MBEDTLS_PSK_MAX_LEN];
    size_t psk_len = 0;
#endif

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    unsigned char cid[MBEDTLS_SSL_CID_IN_LEN_MAX];
    unsigned char cid_renego[MBEDTLS_SSL_CID_IN_LEN_MAX];
    size_t cid_len = 0;
    size_t cid_renego_len = 0;
#endif

#if defined(MBEDTLS_SSL_ALPN)
    const char *alpn_list[ALPN_LIST_SIZE];
#endif

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
    unsigned char alloc_buf[MEMORY_HEAP_SIZE];
#endif
    uint16_t group_list[GROUP_LIST_SIZE];
#if defined(MBEDTLS_SSL_DTLS_SRTP)
    unsigned char mki[MBEDTLS_TLS_SRTP_MAX_MKI_LENGTH];
    size_t mki_len = 0;
#endif

    const char *pers = "ssl_client2";

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED)
    mbedtls_svc_key_id_t slot = MBEDTLS_SVC_KEY_ID_INIT;
    psa_algorithm_t alg = 0;
    psa_key_attributes_t key_attributes;
#endif
    psa_status_t status;

    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_ssl_session saved_session;
    unsigned char *session_data = NULL;
    size_t session_data_len = 0;
#if defined(MBEDTLS_TIMING_C)
    mbedtls_timing_delay_context timer;
#endif
#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    uint32_t flags;
    mbedtls_x509_crt cacert;
    mbedtls_x509_crt clicert;
    mbedtls_pk_context pkey;
    mbedtls_x509_crt_profile crt_profile_for_test = mbedtls_x509_crt_profile_default;
    mbedtls_svc_key_id_t key_slot = MBEDTLS_SVC_KEY_ID_INIT; /* invalid key slot */
#endif  /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */
    char *p, *q;
    const int *list;
#if defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION)
    unsigned char *context_buf = NULL;
    size_t context_buf_len;
#endif
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
#if defined(MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED)
    mbedtls_svc_key_id_t ecjpake_pw_slot = MBEDTLS_SVC_KEY_ID_INIT; /* ecjpake password key slot */
#endif /* MBEDTLS_KEY_EXCHANGE_ECJPAKE_ENABLED */

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
    mbedtls_memory_buffer_alloc_init(alloc_buf, sizeof(alloc_buf));
#endif

#if defined(MBEDTLS_TEST_HOOKS)
    test_hooks_init();
#endif /* MBEDTLS_TEST_HOOKS */

    /*
     * Make sure memory references are valid.
     */
    mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_ssl_session_init(&saved_session);
#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    mbedtls_x509_crt_init(&cacert);
    mbedtls_x509_crt_init(&clicert);
    mbedtls_pk_init(&pkey);
#endif
#if defined(MBEDTLS_SSL_ALPN)
    memset((void *) alpn_list, 0, sizeof(alpn_list));
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

    opt.server_name         = DFL_SERVER_NAME;
    opt.server_addr         = DFL_SERVER_ADDR;
    opt.server_port         = DFL_SERVER_PORT;
    opt.debug_level         = DFL_DEBUG_LEVEL;
    opt.cid_enabled         = DFL_CID_ENABLED;
    opt.cid_val             = DFL_CID_VALUE;
    opt.cid_enabled_renego  = DFL_CID_ENABLED_RENEGO;
    opt.cid_val_renego      = DFL_CID_VALUE_RENEGO;
    opt.nbio                = DFL_NBIO;
    opt.event               = DFL_EVENT;
    opt.context_crt_cb      = DFL_CONTEXT_CRT_CB;
    opt.read_timeout        = DFL_READ_TIMEOUT;
    opt.max_resend          = DFL_MAX_RESEND;
    opt.request_page        = DFL_REQUEST_PAGE;
    opt.request_size        = DFL_REQUEST_SIZE;
    opt.ca_file             = DFL_CA_FILE;
    opt.ca_path             = DFL_CA_PATH;
    opt.crt_file            = DFL_CRT_FILE;
    opt.key_file            = DFL_KEY_FILE;
    opt.key_opaque          = DFL_KEY_OPAQUE;
    opt.key_pwd             = DFL_KEY_PWD;
    opt.psk                 = DFL_PSK;
    opt.psk_opaque          = DFL_PSK_OPAQUE;
#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
    opt.ca_callback         = DFL_CA_CALLBACK;
#endif
    opt.psk_identity        = DFL_PSK_IDENTITY;
    opt.ecjpake_pw          = DFL_ECJPAKE_PW;
    opt.ecjpake_pw_opaque   = DFL_ECJPAKE_PW_OPAQUE;
    opt.ec_max_ops          = DFL_EC_MAX_OPS;
    opt.force_ciphersuite[0] = DFL_FORCE_CIPHER;
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
    opt.tls13_kex_modes     = DFL_TLS1_3_KEX_MODES;
#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */
    opt.renegotiation       = DFL_RENEGOTIATION;
    opt.allow_legacy        = DFL_ALLOW_LEGACY;
    opt.renegotiate         = DFL_RENEGOTIATE;
    opt.renego_delay        = DFL_RENEGO_DELAY;
    opt.exchanges           = DFL_EXCHANGES;
    opt.min_version         = DFL_MIN_VERSION;
    opt.max_version         = DFL_MAX_VERSION;
    opt.allow_sha1          = DFL_SHA1;
    opt.auth_mode           = DFL_AUTH_MODE;
    opt.set_hostname        = DFL_SET_HOSTNAME;
    opt.mfl_code            = DFL_MFL_CODE;
    opt.trunc_hmac          = DFL_TRUNC_HMAC;
    opt.recsplit            = DFL_RECSPLIT;
    opt.reconnect           = DFL_RECONNECT;
    opt.reco_server_name    = DFL_RECO_SERVER_NAME;
    opt.reco_delay          = DFL_RECO_DELAY;
    opt.reco_mode           = DFL_RECO_MODE;
    opt.reconnect_hard      = DFL_RECONNECT_HARD;
    opt.tickets             = DFL_TICKETS;
    opt.alpn_string         = DFL_ALPN_STRING;
    opt.groups              = DFL_GROUPS;
    opt.sig_algs            = DFL_SIG_ALGS;
#if defined(MBEDTLS_SSL_EARLY_DATA)
    opt.early_data          = DFL_EARLY_DATA;
#endif
    opt.transport           = DFL_TRANSPORT;
    opt.hs_to_min           = DFL_HS_TO_MIN;
    opt.hs_to_max           = DFL_HS_TO_MAX;
    opt.dtls_mtu            = DFL_DTLS_MTU;
    opt.fallback            = DFL_FALLBACK;
    opt.extended_ms         = DFL_EXTENDED_MS;
    opt.etm                 = DFL_ETM;
    opt.dgram_packing       = DFL_DGRAM_PACKING;
    opt.serialize           = DFL_SERIALIZE;
    opt.context_file        = DFL_CONTEXT_FILE;
    opt.eap_tls             = DFL_EAP_TLS;
    opt.reproducible        = DFL_REPRODUCIBLE;
    opt.nss_keylog          = DFL_NSS_KEYLOG;
    opt.nss_keylog_file     = DFL_NSS_KEYLOG_FILE;
    opt.skip_close_notify   = DFL_SKIP_CLOSE_NOTIFY;
    opt.exp_label           = DFL_EXP_LABEL;
    opt.exp_len             = DFL_EXP_LEN;
    opt.query_config_mode   = DFL_QUERY_CONFIG_MODE;
    opt.use_srtp            = DFL_USE_SRTP;
    opt.force_srtp_profile  = DFL_SRTP_FORCE_PROFILE;
    opt.mki                 = DFL_SRTP_MKI;
    opt.key_opaque_alg1     = DFL_KEY_OPAQUE_ALG;
    opt.key_opaque_alg2     = DFL_KEY_OPAQUE_ALG;

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

        if (strcmp(p, "server_name") == 0) {
            opt.server_name = q;
        } else if (strcmp(p, "server_addr") == 0) {
            opt.server_addr = q;
        } else if (strcmp(p, "server_port") == 0) {
            opt.server_port = q;
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
        } else if (strcmp(p, "context_crt_cb") == 0) {
            opt.context_crt_cb = atoi(q);
            if (opt.context_crt_cb != 0 && opt.context_crt_cb != 1) {
                goto usage;
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
        } else if (strcmp(p, "max_resend") == 0) {
            opt.max_resend = atoi(q);
            if (opt.max_resend < 0) {
                goto usage;
            }
        } else if (strcmp(p, "request_page") == 0) {
            opt.request_page = q;
        } else if (strcmp(p, "request_size") == 0) {
            opt.request_size = atoi(q);
            if (opt.request_size < 0 ||
                opt.request_size > MAX_REQUEST_SIZE) {
                goto usage;
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
        }
#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
        else if (strcmp(p, "ca_callback") == 0) {
            opt.ca_callback = atoi(q);
        }
#endif
        else if (strcmp(p, "psk_identity") == 0) {
            opt.psk_identity = q;
        } else if (strcmp(p, "ecjpake_pw") == 0) {
            opt.ecjpake_pw = q;
        } else if (strcmp(p, "ecjpake_pw_opaque") == 0) {
            opt.ecjpake_pw_opaque = atoi(q);
        } else if (strcmp(p, "ec_max_ops") == 0) {
            opt.ec_max_ops = atoi(q);
        } else if (strcmp(p, "force_ciphersuite") == 0) {
            opt.force_ciphersuite[0] = mbedtls_ssl_get_ciphersuite_id(q);

            if (opt.force_ciphersuite[0] == 0) {
                ret = 2;
                goto usage;
            }
            opt.force_ciphersuite[1] = 0;
        } else if (strcmp(p, "renegotiation") == 0) {
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
        } else if (strcmp(p, "renego_delay") == 0) {
            opt.renego_delay = (atoi(q));
        } else if (strcmp(p, "renegotiate") == 0) {
            opt.renegotiate = atoi(q);
            if (opt.renegotiate < 0 || opt.renegotiate > 1) {
                goto usage;
            }
        } else if (strcmp(p, "exchanges") == 0) {
            opt.exchanges = atoi(q);
            if (opt.exchanges < 1) {
                goto usage;
            }
        } else if (strcmp(p, "reconnect") == 0) {
            opt.reconnect = atoi(q);
            if (opt.reconnect < 0 || opt.reconnect > 2) {
                goto usage;
            }
        } else if (strcmp(p, "reco_server_name") == 0) {
            opt.reco_server_name = q;
        } else if (strcmp(p, "reco_delay") == 0) {
            opt.reco_delay = atoi(q);
            if (opt.reco_delay < 0) {
                goto usage;
            }
        } else if (strcmp(p, "reco_mode") == 0) {
            opt.reco_mode = atoi(q);
            if (opt.reco_mode < 0) {
                goto usage;
            }
        } else if (strcmp(p, "reconnect_hard") == 0) {
            opt.reconnect_hard = atoi(q);
            if (opt.reconnect_hard < 0 || opt.reconnect_hard > 1) {
                goto usage;
            }
        } else if (strcmp(p, "tickets") == 0) {
            opt.tickets = atoi(q);
            if (opt.tickets < 0) {
                goto usage;
            }
        } else if (strcmp(p, "alpn") == 0) {
            opt.alpn_string = q;
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
        } else if (strcmp(p, "groups") == 0) {
            opt.groups = q;
        }
#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
        else if (strcmp(p, "sig_algs") == 0) {
            opt.sig_algs = q;
        }
#endif
        else if (strcmp(p, "etm") == 0) {
            switch (atoi(q)) {
                case 0: opt.etm = MBEDTLS_SSL_ETM_DISABLED; break;
                case 1: opt.etm = MBEDTLS_SSL_ETM_ENABLED; break;
                default: goto usage;
            }
        }

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
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
        }
#endif /* MBEDTLS_SSL_EARLY_DATA */

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
            } else if (strcmp(q, "psk_or_ephemeral") == 0) {
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
            if (strcmp(q, "none") == 0) {
                opt.auth_mode = MBEDTLS_SSL_VERIFY_NONE;
            } else if (strcmp(q, "optional") == 0) {
                opt.auth_mode = MBEDTLS_SSL_VERIFY_OPTIONAL;
            } else if (strcmp(q, "required") == 0) {
                opt.auth_mode = MBEDTLS_SSL_VERIFY_REQUIRED;
            } else {
                goto usage;
            }
        } else if (strcmp(p, "set_hostname") == 0) {
            if (strcmp(q, "no") == 0) {
                opt.set_hostname = 0;
            } else if (strcmp(q, "server_name") == 0) {
                opt.set_hostname = 1;
            } else if (strcmp(q, "NULL") == 0) {
                opt.set_hostname = -1;
            } else {
                goto usage;
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
        } else if (strcmp(p, "trunc_hmac") == 0) {
            switch (atoi(q)) {
                case 0: opt.trunc_hmac = MBEDTLS_SSL_TRUNC_HMAC_DISABLED; break;
                case 1: opt.trunc_hmac = MBEDTLS_SSL_TRUNC_HMAC_ENABLED; break;
                default: goto usage;
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
        } else if (strcmp(p, "recsplit") == 0) {
            opt.recsplit = atoi(q);
            if (opt.recsplit < 0 || opt.recsplit > 1) {
                goto usage;
            }
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
        } else if (strcmp(p, "skip_close_notify") == 0) {
            opt.skip_close_notify = atoi(q);
            if (opt.skip_close_notify < 0 || opt.skip_close_notify > 1) {
                goto usage;
            }
        } else if (strcmp(p, "exp_label") == 0) {
            opt.exp_label = q;
        } else if (strcmp(p, "exp_len") == 0) {
            opt.exp_len = atoi(q);
        } else if (strcmp(p, "use_srtp") == 0) {
            opt.use_srtp = atoi(q);
        } else if (strcmp(p, "srtp_force_profile") == 0) {
            opt.force_srtp_profile = atoi(q);
        } else if (strcmp(p, "mki") == 0) {
            opt.mki = q;
        } else if (strcmp(p, "key_opaque_algs") == 0) {
            if (key_opaque_alg_parse(q, &opt.key_opaque_alg1,
                                     &opt.key_opaque_alg2) != 0) {
                goto usage;
            }
        } else {
            /* This signals that the problem is with p not q */
            q = NULL;
            goto usage;
        }
    }
    /* This signals that any further errors are not with a single option */
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

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED)
    /*
     * Unhexify the pre-shared key if any is given
     */
    if (strlen(opt.psk)) {
        if (mbedtls_test_unhexify(psk, sizeof(psk),
                                  opt.psk, &psk_len) != 0) {
            mbedtls_printf("pre-shared key not valid\n");
            goto exit;
        }
    }
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED */

    if (opt.psk_opaque != 0) {
        if (opt.psk == NULL) {
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

        /* If the server selects a version that's not supported by
         * this suite, then there will be no common ciphersuite... */
        if (opt.max_version == -1 ||
            opt.max_version > ciphersuite_info->max_tls_version) {
            opt.max_version = ciphersuite_info->max_tls_version;
        }
        if (opt.min_version < ciphersuite_info->min_tls_version) {
            opt.min_version = ciphersuite_info->min_tls_version;
            /* DTLS starts with TLS 1.2 */
            if (opt.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM &&
                opt.min_version < MBEDTLS_SSL_VERSION_TLS1_2) {
                opt.min_version = MBEDTLS_SSL_VERSION_TLS1_2;
            }
        }

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED)
        if (opt.psk_opaque != 0) {
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
        mbedtls_printf("CID not valid\n");
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
        mbedtls_printf("CID not valid\n");
        goto exit;
    }
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

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
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */

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
     *
     * (can be skipped if client authentication is not required)
     */
    mbedtls_printf("  . Loading the client cert. and key...");
    fflush(stdout);

    if (strcmp(opt.crt_file, "none") == 0) {
        ret = 0;
    } else
#if defined(MBEDTLS_FS_IO)
    if (strlen(opt.crt_file)) {
        ret = mbedtls_x509_crt_parse_file(&clicert, opt.crt_file);
    } else
#endif
    { ret = mbedtls_x509_crt_parse(&clicert,
                                   (const unsigned char *) mbedtls_test_cli_crt,
                                   mbedtls_test_cli_crt_len); }
    if (ret != 0) {
        mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse returned -0x%x\n\n",
                       (unsigned int) -ret);
        goto exit;
    }

    if (strcmp(opt.key_file, "none") == 0) {
        ret = 0;
    } else
#if defined(MBEDTLS_FS_IO)
    if (strlen(opt.key_file)) {
        ret = mbedtls_pk_parse_keyfile(&pkey, opt.key_file, opt.key_pwd);
    } else
#endif
    { ret = mbedtls_pk_parse_key(&pkey,
                                 (const unsigned char *) mbedtls_test_cli_key,
                                 mbedtls_test_cli_key_len, NULL, 0); }
    if (ret != 0) {
        mbedtls_printf(" failed\n  !  mbedtls_pk_parse_key returned -0x%x\n\n",
                       (unsigned int) -ret);
        goto exit;
    }

    if (opt.key_opaque != 0) {
        psa_algorithm_t psa_alg, psa_alg2 = PSA_ALG_NONE;
        psa_key_usage_t usage = 0;

        if (key_opaque_set_alg_usage(opt.key_opaque_alg1,
                                     opt.key_opaque_alg2,
                                     &psa_alg, &psa_alg2,
                                     &usage,
                                     mbedtls_pk_get_type(&pkey)) == 0) {
            ret = pk_wrap_as_opaque(&pkey, psa_alg, psa_alg2, usage, &key_slot);
            if (ret != 0) {
                mbedtls_printf(" failed\n  !  "
                               "mbedtls_pk_get_psa_attributes returned -0x%x\n\n",
                               (unsigned int)  -ret);
                goto exit;
            }
        }
    }

    mbedtls_printf(" ok (key type: %s)\n",
                   strlen(opt.key_file) || strlen(opt.key_opaque_alg1) ?
                   mbedtls_x509_pk_type_as_string(&pkey) : "none");
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */

    /*
     * 2. Setup stuff
     */
    mbedtls_printf("  . Setting up the SSL/TLS structure...");
    fflush(stdout);

    if ((ret = mbedtls_ssl_config_defaults(&conf,
                                           MBEDTLS_SSL_IS_CLIENT,
                                           opt.transport,
                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_config_defaults returned -0x%x\n\n",
                       (unsigned int) -ret);
        goto exit;
    }

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    /* The default algorithms profile disables SHA-1, but our tests still
       rely on it heavily. */
    if (opt.allow_sha1 > 0) {
        crt_profile_for_test.allowed_mds |= MBEDTLS_X509_ID_FLAG(MBEDTLS_MD_SHA1);
        mbedtls_ssl_conf_cert_profile(&conf, &crt_profile_for_test);
        mbedtls_ssl_conf_sig_algs(&conf, ssl_sig_algs_for_test);
    }
    if (opt.context_crt_cb == 0) {
        mbedtls_ssl_conf_verify(&conf, my_verify, NULL);
    }

    memset(peer_crt_info, 0, sizeof(peer_crt_info));
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */

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

    if (opt.auth_mode != DFL_AUTH_MODE) {
        mbedtls_ssl_conf_authmode(&conf, opt.auth_mode);
    }

#if defined(MBEDTLS_SSL_PROTO_DTLS)
    if (opt.hs_to_min != DFL_HS_TO_MIN || opt.hs_to_max != DFL_HS_TO_MAX) {
        mbedtls_ssl_conf_handshake_timeout(&conf, opt.hs_to_min,
                                           opt.hs_to_max);
    }

    if (opt.dgram_packing != DFL_DGRAM_PACKING) {
        mbedtls_ssl_set_datagram_packing(&ssl, opt.dgram_packing);
    }
#endif /* MBEDTLS_SSL_PROTO_DTLS */

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
    if ((ret = mbedtls_ssl_conf_max_frag_len(&conf, opt.mfl_code)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_conf_max_frag_len returned %d\n\n",
                       ret);
        goto exit;
    }
#endif

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
            mbedtls_printf(" failed\n  ! "
                           "mbedtls_ssl_conf_dtls_srtp_protection_profiles returned %d\n\n",
                           ret);
            goto exit;
        }

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
            mbedtls_printf(" failed\n  ! mbedtls_ssl_conf_alpn_protocols returned %d\n\n",
                           ret);
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

    mbedtls_ssl_conf_read_timeout(&conf, opt.read_timeout);

#if defined(MBEDTLS_SSL_SESSION_TICKETS)
    mbedtls_ssl_conf_session_tickets(&conf, opt.tickets);
#endif

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
    if (strcmp(opt.crt_file, "none") != 0 &&
        strcmp(opt.key_file, "none") != 0) {
        if ((ret = mbedtls_ssl_conf_own_cert(&conf, &clicert, &pkey)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_conf_own_cert returned %d\n\n",
                           ret);
            goto exit;
        }
    }
#endif  /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */

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
    if (opt.psk_opaque != 0) {
        key_attributes = psa_key_attributes_init();
        psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_DERIVE);
        psa_set_key_algorithm(&key_attributes, alg);
        psa_set_key_type(&key_attributes, PSA_KEY_TYPE_DERIVE);

        status = psa_import_key(&key_attributes, psk, psk_len, &slot);
        if (status != PSA_SUCCESS) {
            ret = MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
            goto exit;
        }

        if ((ret = mbedtls_ssl_conf_psk_opaque(&conf, slot,
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
            mbedtls_printf(" failed\n  ! mbedtls_ssl_conf_psk returned %d\n\n", ret);
            goto exit;
        }
    }
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED */

    if (opt.min_version != DFL_MIN_VERSION) {
        mbedtls_ssl_conf_min_tls_version(&conf, opt.min_version);
    }

    if (opt.max_version != DFL_MAX_VERSION) {
        mbedtls_ssl_conf_max_tls_version(&conf, opt.max_version);
    }

#if defined(MBEDTLS_SSL_EARLY_DATA)
    if (opt.early_data != DFL_EARLY_DATA) {
        mbedtls_ssl_conf_early_data(&conf, opt.early_data);
    }
#endif /* MBEDTLS_SSL_EARLY_DATA */

    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_ssl_setup returned -0x%x\n\n",
                       (unsigned int) -ret);
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

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    switch (opt.set_hostname) {
        case -1:
            if ((ret = mbedtls_ssl_set_hostname(&ssl, NULL)) != 0) {
                mbedtls_printf(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n",
                               ret);
                goto exit;
            }
            break;
        case 0:
            /* Skip the call */
            break;
        default:
            if ((ret = mbedtls_ssl_set_hostname(&ssl, opt.server_name)) != 0) {
                mbedtls_printf(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n",
                               ret);
                goto exit;
            }
            break;
    }
#endif

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

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    if (opt.context_crt_cb == 1) {
        mbedtls_ssl_set_verify(&ssl, my_verify, NULL);
    }
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */

    io_ctx.ssl = &ssl;
    io_ctx.net = &server_fd;
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

#if defined(MBEDTLS_ECP_RESTARTABLE)
    if (opt.ec_max_ops != DFL_EC_MAX_OPS) {
        psa_interruptible_set_max_ops(opt.ec_max_ops);
    }
#endif

#if defined(MBEDTLS_SSL_DTLS_SRTP)
    if (opt.use_srtp != 0 && strlen(opt.mki) != 0) {
        if (mbedtls_test_unhexify(mki, sizeof(mki),
                                  opt.mki, &mki_len) != 0) {
            mbedtls_printf("mki value not valid hex\n");
            goto exit;
        }

        mbedtls_ssl_conf_srtp_mki_value_supported(&conf, MBEDTLS_SSL_DTLS_SRTP_MKI_SUPPORTED);
        if ((ret = mbedtls_ssl_dtls_srtp_set_mki_value(&ssl, mki,
                                                       (uint16_t) strlen(opt.mki) / 2)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_dtls_srtp_set_mki_value returned %d\n\n", ret);
            goto exit;
        }
    }
#endif

    mbedtls_printf(" ok\n");

    /*
     * 3. Start the connection
     */
    if (opt.server_addr == NULL) {
        opt.server_addr = opt.server_name;
    }

    mbedtls_printf("  . Connecting to %s/%s/%s...",
                   opt.transport == MBEDTLS_SSL_TRANSPORT_STREAM ? "tcp" : "udp",
                   opt.server_addr, opt.server_port);
    fflush(stdout);

    if ((ret = mbedtls_net_connect(&server_fd,
                                   opt.server_addr, opt.server_port,
                                   opt.transport == MBEDTLS_SSL_TRANSPORT_STREAM ?
                                   MBEDTLS_NET_PROTO_TCP : MBEDTLS_NET_PROTO_UDP)) != 0) {
        mbedtls_printf(" failed\n  ! mbedtls_net_connect returned -0x%x\n\n",
                       (unsigned int) -ret);
        goto exit;
    }

    if (opt.nbio > 0) {
        ret = mbedtls_net_set_nonblock(&server_fd);
    } else {
        ret = mbedtls_net_set_block(&server_fd);
    }
    if (ret != 0) {
        mbedtls_printf(" failed\n  ! net_set_(non)block() returned -0x%x\n\n",
                       (unsigned int) -ret);
        goto exit;
    }

    mbedtls_printf(" ok\n");

    /*
     * 4. Handshake
     */
    mbedtls_printf("  . Performing the SSL/TLS handshake...");
    fflush(stdout);

    while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ &&
            ret != MBEDTLS_ERR_SSL_WANT_WRITE &&
            ret != MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n",
                           (unsigned int) -ret);
#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
            if (ret == MBEDTLS_ERR_X509_CERT_VERIFY_FAILED ||
                ret == MBEDTLS_ERR_SSL_BAD_CERTIFICATE) {
                mbedtls_printf(
                    "    Unable to verify the server's certificate. "
                    "Either it is invalid,\n"
                    "    or you didn't set ca_file or ca_path "
                    "to an appropriate value.\n"
                    "    Alternatively, you may want to use "
                    "auth_mode=optional for testing purposes if "
                    "not using TLS 1.3.\n"
                    "    For TLS 1.3 server, try `ca_path=/etc/ssl/certs/`"
                    "or other folder that has root certificates\n");

                flags = mbedtls_ssl_get_verify_result(&ssl);
                char vrfy_buf[512];
                x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);
                mbedtls_printf("%s\n", vrfy_buf);
            }
#endif
            mbedtls_printf("\n");
            goto exit;
        }

#if defined(MBEDTLS_ECP_RESTARTABLE)
        if (ret == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS) {
            continue;
        }
#endif

        /* For event-driven IO, wait for socket to become available */
        if (opt.event == 1 /* level triggered IO */) {
#if defined(MBEDTLS_TIMING_C)
            ret = idle(&server_fd, &timer, ret);
#else
            ret = idle(&server_fd, ret);
#endif
            if (ret != 0) {
                goto exit;
            }
        }
    }

    {
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

#if defined(MBEDTLS_SSL_MAX_FRAGMENT_LENGTH)
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
            goto exit;
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
            goto exit;
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
    if (opt.reconnect != 0 && ssl.tls_version != MBEDTLS_SSL_VERSION_TLS1_3) {
        mbedtls_printf("  . Saving session for reuse...");
        fflush(stdout);

        if (opt.reco_mode == 1) {
            if ((ret = ssl_save_session_serialize(&ssl,
                                                  &session_data, &session_data_len)) != 0) {
                mbedtls_printf(" failed\n  ! ssl_save_session_serialize returned -0x%04x\n\n",
                               (unsigned int) -ret);
                goto exit;
            }

        } else {
            if ((ret = mbedtls_ssl_get_session(&ssl, &saved_session)) != 0) {
                mbedtls_printf(" failed\n  ! mbedtls_ssl_get_session returned -0x%x\n\n",
                               (unsigned int) -ret);
                goto exit;
            }
        }

        mbedtls_printf(" ok\n");

        if (opt.reco_mode == 1) {
            mbedtls_printf("    [ Saved %u bytes of session data]\n",
                           (unsigned) session_data_len);
        }
    }

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    /*
     * 5. Verify the server certificate
     */
    mbedtls_printf("  . Verifying peer X.509 certificate...");

    if ((flags = mbedtls_ssl_get_verify_result(&ssl)) != 0) {
        char vrfy_buf[512];
        mbedtls_printf(" failed\n");

        x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf),
                             "  ! ", flags);

        mbedtls_printf("%s\n", vrfy_buf);
    } else {
        mbedtls_printf(" ok\n");
    }

#if !defined(MBEDTLS_X509_REMOVE_INFO)
    mbedtls_printf("  . Peer certificate information    ...\n");
    mbedtls_printf("%s\n", peer_crt_info);
#endif /* !MBEDTLS_X509_REMOVE_INFO */
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */

#if defined(MBEDTLS_SSL_DTLS_CONNECTION_ID)
    ret = report_cid_usage(&ssl, "initial handshake");
    if (ret != 0) {
        goto exit;
    }

    if (opt.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
        if ((ret = mbedtls_ssl_set_cid(&ssl, opt.cid_enabled_renego,
                                       cid_renego,
                                       cid_renego_len)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_set_cid returned %d\n\n",
                           ret);
            goto exit;
        }
    }
#endif /* MBEDTLS_SSL_DTLS_CONNECTION_ID */

#if defined(MBEDTLS_SSL_RENEGOTIATION)
    if (opt.renegotiate) {
        /*
         * Perform renegotiation (this must be done when the server is waiting
         * for input from our side).
         */
        mbedtls_printf("  . Performing renegotiation...");
        fflush(stdout);
        while ((ret = mbedtls_ssl_renegotiate(&ssl)) != 0) {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ &&
                ret != MBEDTLS_ERR_SSL_WANT_WRITE &&
                ret != MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS) {
                mbedtls_printf(" failed\n  ! mbedtls_ssl_renegotiate returned %d\n\n",
                               ret);
                goto exit;
            }

#if defined(MBEDTLS_ECP_RESTARTABLE)
            if (ret == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS) {
                continue;
            }
#endif

            /* For event-driven IO, wait for socket to become available */
            if (opt.event == 1 /* level triggered IO */) {
#if defined(MBEDTLS_TIMING_C)
                idle(&server_fd, &timer, ret);
#else
                idle(&server_fd, ret);
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

    /*
     * 6. Write the GET request
     */
    retry_left = opt.max_resend;
send_request:
    mbedtls_printf("  > Write to server:");
    fflush(stdout);

    ret = build_http_request(buf, sizeof(buf) - 1, &len);
    if (ret != 0) {
        goto exit;
    }

    if (opt.transport == MBEDTLS_SSL_TRANSPORT_STREAM) {
        written = 0;
        frags = 0;

        do {
            while ((ret = mbedtls_ssl_write(&ssl, buf + written,
                                            len - written)) < 0) {
                if (ret != MBEDTLS_ERR_SSL_WANT_READ &&
                    ret != MBEDTLS_ERR_SSL_WANT_WRITE &&
                    ret != MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS) {
                    mbedtls_printf(" failed\n  ! mbedtls_ssl_write returned -0x%x\n\n",
                                   (unsigned int) -ret);
                    goto exit;
                }

                /* For event-driven IO, wait for socket to become available */
                if (opt.event == 1 /* level triggered IO */) {
#if defined(MBEDTLS_TIMING_C)
                    idle(&server_fd, &timer, ret);
#else
                    idle(&server_fd, ret);
#endif
                }
            }

            frags++;
            written += ret;
        } while (written < len);
    } else { /* Not stream, so datagram */
        while (1) {
            ret = mbedtls_ssl_write(&ssl, buf, len);

#if defined(MBEDTLS_ECP_RESTARTABLE)
            if (ret == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS) {
                continue;
            }
#endif

            if (ret != MBEDTLS_ERR_SSL_WANT_READ &&
                ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
                break;
            }

            /* For event-driven IO, wait for socket to become available */
            if (opt.event == 1 /* level triggered IO */) {
#if defined(MBEDTLS_TIMING_C)
                idle(&server_fd, &timer, ret);
#else
                idle(&server_fd, ret);
#endif
            }
        }

        if (ret < 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_write returned %d\n\n",
                           ret);
            goto exit;
        }

        frags = 1;
        written = ret;

        if (written < len) {
            mbedtls_printf(" warning\n  ! request didn't fit into single datagram and "
                           "was truncated to size %u", (unsigned) written);
        }
    }

    buf[written] = '\0';
    mbedtls_printf(
        " %" MBEDTLS_PRINTF_SIZET " bytes written in %" MBEDTLS_PRINTF_SIZET " fragments\n\n%s\n",
        written,
        frags,
        (char *) buf);

    /* Send a non-empty request if request_size == 0 */
    if (len == 0) {
        opt.request_size = DFL_REQUEST_SIZE;
        goto send_request;
    }

    /*
     * 7. Read the HTTP response
     */

    /*
     * TLS and DTLS need different reading styles (stream vs datagram)
     */
    if (opt.transport == MBEDTLS_SSL_TRANSPORT_STREAM) {
#if defined(MBEDTLS_SSL_PROTO_TLS1_3) && defined(MBEDTLS_SSL_SESSION_TICKETS)
        int ticket_id = 0;
#endif
        do {
            len = sizeof(buf) - 1;
            memset(buf, 0, sizeof(buf));
            ret = mbedtls_ssl_read(&ssl, buf, len);

#if defined(MBEDTLS_ECP_RESTARTABLE)
            if (ret == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS) {
                continue;
            }
#endif

            if (ret == MBEDTLS_ERR_SSL_WANT_READ ||
                ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
                /* For event-driven IO, wait for socket to become available */
                if (opt.event == 1 /* level triggered IO */) {
#if defined(MBEDTLS_TIMING_C)
                    idle(&server_fd, &timer, ret);
#else
                    idle(&server_fd, ret);
#endif
                }
                continue;
            }

            if (ret <= 0) {
                switch (ret) {
                    case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
                        mbedtls_printf(" connection was closed gracefully\n");
                        ret = 0;
                        goto close_notify;

                    case 0:
                    case MBEDTLS_ERR_NET_CONN_RESET:
                        mbedtls_printf(" connection was reset by peer\n");
                        ret = 0;
                        goto reconnect;

#if defined(MBEDTLS_SSL_PROTO_TLS1_3)

#if defined(MBEDTLS_SSL_SESSION_TICKETS)
                    case MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET:
                        /* We were waiting for application data but got
                         * a NewSessionTicket instead. */
                        mbedtls_printf(" got new session ticket ( %d ).\n",
                                       ticket_id++);
                        if (opt.reconnect != 0) {
                            mbedtls_printf("  . Saving session for reuse...");
                            fflush(stdout);

                            if (opt.reco_mode == 1) {
                                if ((ret = ssl_save_session_serialize(&ssl,
                                                                      &session_data,
                                                                      &session_data_len)) != 0) {
                                    mbedtls_printf(
                                        " failed\n  ! ssl_save_session_serialize returned -0x%04x\n\n",
                                        (unsigned int) -ret);
                                    goto exit;
                                }
                            } else {
                                if ((ret = mbedtls_ssl_get_session(&ssl, &saved_session)) != 0) {
                                    mbedtls_printf(
                                        " failed\n  ! mbedtls_ssl_get_session returned -0x%x\n\n",
                                        (unsigned int) -ret);
                                    goto exit;
                                }
                            }

                            mbedtls_printf(" ok\n");

                            if (opt.reco_mode == 1) {
                                mbedtls_printf("    [ Saved %u bytes of session data]\n",
                                               (unsigned) session_data_len);
                            }
                        }
                        continue;
#endif /* MBEDTLS_SSL_SESSION_TICKETS */

#endif /* MBEDTLS_SSL_PROTO_TLS1_3 */

                    default:
                        mbedtls_printf(" mbedtls_ssl_read returned -0x%x\n",
                                       (unsigned int) -ret);
                        goto exit;
                }
            }

            len = ret;
            buf[len] = '\0';
            mbedtls_printf("  < Read from server: %" MBEDTLS_PRINTF_SIZET " bytes read\n\n%s",
                           len,
                           (char *) buf);
            fflush(stdout);
            /* End of message should be detected according to the syntax of the
             * application protocol (eg HTTP), just use a dummy test here. */
            if (ret > 0 && buf[len-1] == '\n') {
                ret = 0;
                break;
            }
        } while (1);
    } else { /* Not stream, so datagram */
        len = sizeof(buf) - 1;
        memset(buf, 0, sizeof(buf));

        while (1) {
            ret = mbedtls_ssl_read(&ssl, buf, len);

#if defined(MBEDTLS_ECP_RESTARTABLE)
            if (ret == MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS) {
                continue;
            }
#endif

            if (ret != MBEDTLS_ERR_SSL_WANT_READ &&
                ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
                break;
            }

            /* For event-driven IO, wait for socket to become available */
            if (opt.event == 1 /* level triggered IO */) {
#if defined(MBEDTLS_TIMING_C)
                idle(&server_fd, &timer, ret);
#else
                idle(&server_fd, ret);
#endif
            }
        }

        if (ret <= 0) {
            switch (ret) {
                case MBEDTLS_ERR_SSL_TIMEOUT:
                    mbedtls_printf(" timeout\n");
                    if (retry_left-- > 0) {
                        goto send_request;
                    }
                    goto exit;

                case MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY:
                    mbedtls_printf(" connection was closed gracefully\n");
                    ret = 0;
                    goto close_notify;

                default:
                    mbedtls_printf(" mbedtls_ssl_read returned -0x%x\n", (unsigned int) -ret);
                    goto exit;
            }
        }

        len = ret;
        buf[len] = '\0';
        mbedtls_printf("  < Read from server: %" MBEDTLS_PRINTF_SIZET " bytes read\n\n%s",
                       len,
                       (char *) buf);
        ret = 0;
    }

    /*
     * 7b. Simulate hard reset and reconnect from same port?
     */
    if (opt.reconnect_hard != 0) {
        opt.reconnect_hard = 0;

        mbedtls_printf("  . Restarting connection from same port...");
        fflush(stdout);

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
        memset(peer_crt_info, 0, sizeof(peer_crt_info));
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */

        if ((ret = mbedtls_ssl_session_reset(&ssl)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_session_reset returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }

        while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ &&
                ret != MBEDTLS_ERR_SSL_WANT_WRITE &&
                ret != MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS) {
                mbedtls_printf(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n",
                               (unsigned int) -ret);
                goto exit;
            }

            /* For event-driven IO, wait for socket to become available */
            if (opt.event == 1 /* level triggered IO */) {
#if defined(MBEDTLS_TIMING_C)
                idle(&server_fd, &timer, ret);
#else
                idle(&server_fd, ret);
#endif
            }
        }

        mbedtls_printf(" ok\n");

        goto send_request;
    }

    /*
     * 7c. Simulate serialize/deserialize and go back to data exchange
     */
#if defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION)
    if (opt.serialize != 0) {
        size_t buf_len;

        mbedtls_printf("  . Serializing live connection...");

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

        if (opt.serialize == 1) {
            /* nothing to do here, done by context_save() already */
            mbedtls_printf("  . Context has been reset... ok\n");
        }

        if (opt.serialize == 2) {
            mbedtls_printf("  . Freeing and reinitializing context...");

            mbedtls_ssl_free(&ssl);

            mbedtls_ssl_init(&ssl);

            if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
                mbedtls_printf(" failed\n  ! mbedtls_ssl_setup returned "
                               "-0x%x\n\n", (unsigned int) -ret);
                goto exit;
            }

            if (opt.nbio == 2) {
                mbedtls_ssl_set_bio(&ssl, &server_fd, delayed_send,
                                    delayed_recv, NULL);
            } else {
                mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send,
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
     * 7d. Continue doing data exchanges?
     */
    if (--opt.exchanges > 0) {
        goto send_request;
    }

    /*
     * 8. Done, cleanly close the connection
     */
close_notify:
    mbedtls_printf("  . Closing the connection...");
    fflush(stdout);

    /*
     * Most of the time sending a close_notify before closing is the right
     * thing to do. However, when the server already knows how many messages
     * are expected and closes the connection by itself, this alert becomes
     * redundant. Sometimes with DTLS this redundancy becomes a problem by
     * leading to a race condition where the server might close the connection
     * before seeing the alert, and since UDP is connection-less when the
     * alert arrives it will be seen as a new connection, which will fail as
     * the alert is clearly not a valid ClientHello. This may cause spurious
     * failures in tests that use DTLS and resumption with ssl_server2 in
     * ssl-opt.sh, avoided by enabling skip_close_notify client-side.
     */
    if (opt.skip_close_notify == 0) {
        /* No error checking, the connection might be closed already */
        do {
            ret = mbedtls_ssl_close_notify(&ssl);
        } while (ret == MBEDTLS_ERR_SSL_WANT_WRITE);
        ret = 0;
    }

    mbedtls_printf(" done\n");

    /*
     * 9. Reconnect?
     */
reconnect:
    if (opt.reconnect != 0) {
        --opt.reconnect;

        mbedtls_net_free(&server_fd);

#if defined(MBEDTLS_TIMING_C)
        if (opt.reco_delay > 0) {
            mbedtls_net_usleep(1000 * opt.reco_delay);
        }
#endif

        mbedtls_printf("  . Reconnecting with saved session...");

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
        memset(peer_crt_info, 0, sizeof(peer_crt_info));
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */

        if ((ret = mbedtls_ssl_session_reset(&ssl)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_session_reset returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }

        if (opt.reco_mode == 1) {
            if ((ret = mbedtls_ssl_session_load(&saved_session,
                                                session_data,
                                                session_data_len)) != 0) {
                mbedtls_printf(" failed\n  ! mbedtls_ssl_session_load returned -0x%x\n\n",
                               (unsigned int) -ret);
                goto exit;
            }
        }

        if ((ret = mbedtls_ssl_set_session(&ssl, &saved_session)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_set_session returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }

#if defined(MBEDTLS_X509_CRT_PARSE_C)
        if (opt.reco_server_name != NULL &&
            (ret = mbedtls_ssl_set_hostname(&ssl,
                                            opt.reco_server_name)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n",
                           ret);
            goto exit;
        }
#endif

        if ((ret = mbedtls_net_connect(&server_fd,
                                       opt.server_addr, opt.server_port,
                                       opt.transport == MBEDTLS_SSL_TRANSPORT_STREAM ?
                                       MBEDTLS_NET_PROTO_TCP : MBEDTLS_NET_PROTO_UDP)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_net_connect returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }

        if (opt.nbio > 0) {
            ret = mbedtls_net_set_nonblock(&server_fd);
        } else {
            ret = mbedtls_net_set_block(&server_fd);
        }
        if (ret != 0) {
            mbedtls_printf(" failed\n  ! net_set_(non)block() returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }

        ret = build_http_request(buf, sizeof(buf) - 1, &len);
        if (ret != 0) {
            goto exit;
        }

#if defined(MBEDTLS_SSL_EARLY_DATA)
        if (ssl.conf->early_data_enabled == MBEDTLS_SSL_EARLY_DATA_ENABLED) {
            frags = 0;
            written = 0;
            do {
                while ((ret = mbedtls_ssl_write_early_data(&ssl, buf + written,
                                                           len - written)) < 0) {
                    if (ret == MBEDTLS_ERR_SSL_CANNOT_WRITE_EARLY_DATA) {
                        goto end_of_early_data;
                    }
                    if (ret != MBEDTLS_ERR_SSL_WANT_READ &&
                        ret != MBEDTLS_ERR_SSL_WANT_WRITE &&
                        ret != MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS) {
                        mbedtls_printf(" failed\n  ! mbedtls_ssl_write returned -0x%x\n\n",
                                       (unsigned int) -ret);
                        goto exit;
                    }

                    /* For event-driven IO, wait for socket to become available */
                    if (opt.event == 1 /* level triggered IO */) {
#if defined(MBEDTLS_TIMING_C)
                        idle(&server_fd, &timer, ret);
#else
                        idle(&server_fd, ret);
#endif
                    }
                }

                frags++;
                written += ret;
            } while (written < len);

end_of_early_data:

            buf[written] = '\0';
            mbedtls_printf(
                " %" MBEDTLS_PRINTF_SIZET " bytes of early data written in %" MBEDTLS_PRINTF_SIZET " fragments\n\n%s\n",
                written,
                frags,
                (char *) buf);
        }
#endif /* MBEDTLS_SSL_EARLY_DATA */

        while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ &&
                ret != MBEDTLS_ERR_SSL_WANT_WRITE &&
                ret != MBEDTLS_ERR_SSL_CRYPTO_IN_PROGRESS) {
                mbedtls_printf(" failed\n  ! mbedtls_ssl_handshake returned -0x%x\n\n",
                               (unsigned int) -ret);
                goto exit;
            }
        }

        mbedtls_printf(" ok\n");

        goto send_request;
    }

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

    mbedtls_net_free(&server_fd);

    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ssl_session_free(&saved_session);

    if (session_data != NULL) {
        mbedtls_platform_zeroize(session_data, session_data_len);
    }
    mbedtls_free(session_data);
#if defined(MBEDTLS_SSL_CONTEXT_SERIALIZATION)
    if (context_buf != NULL) {
        mbedtls_platform_zeroize(context_buf, context_buf_len);
    }
    mbedtls_free(context_buf);
#endif

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
    mbedtls_x509_crt_free(&clicert);
    mbedtls_x509_crt_free(&cacert);
    mbedtls_pk_free(&pkey);
    psa_destroy_key(key_slot);
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_PSK_ENABLED)
    if (opt.psk_opaque != 0) {
        /* This is ok even if the slot hasn't been
         * initialized (we might have jumed here
         * immediately because of bad cmd line params,
         * for example). */
        status = psa_destroy_key(slot);
        if ((status != PSA_SUCCESS) &&
            (opt.query_config_mode == DFL_QUERY_CONFIG_MODE)) {
            mbedtls_printf("Failed to destroy key slot %u - error was %d",
                           (unsigned) MBEDTLS_SVC_KEY_ID_GET_KEY_ID(slot),
                           (int) status);
            if (ret == 0) {
                ret = MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
            }
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

#if defined(MBEDTLS_TEST_HOOKS)
    if (test_hooks_failure_detected()) {
        if (ret == 0) {
            ret = 1;
        }
        mbedtls_printf("Test hooks detected errors.\n");
    }
    test_hooks_free();
#endif /* MBEDTLS_TEST_HOOKS */

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
#if defined(MBEDTLS_MEMORY_DEBUG)
    mbedtls_memory_buffer_alloc_status();
#endif
    mbedtls_memory_buffer_alloc_free();
#endif  /* MBEDTLS_MEMORY_BUFFER_ALLOC_C */

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
#endif /* !MBEDTLS_SSL_TEST_IMPOSSIBLE && MBEDTLS_SSL_CLI_C */
