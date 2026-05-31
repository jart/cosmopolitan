/*
 *  Mbed TLS SSL context deserializer from base64 code
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

/* On Mingw-w64, force the use of a C99-compliant printf() and friends.
 * This is necessary on older versions of Mingw and/or Windows runtimes
 * where snprintf does not always zero-terminate the buffer, and does
 * not support formats such as "%zu" for size_t and "%lld" for long long.
 */
#if !defined(__USE_MINGW_ANSI_STDIO)
#define __USE_MINGW_ANSI_STDIO 1
#endif

#include "mbedtls/build_info.h"
#include "mbedtls/debug.h"
#include "mbedtls/platform.h"

#include <stdio.h>
#include <stdlib.h>

#if !defined(MBEDTLS_X509_CRT_PARSE_C) || !defined(MBEDTLS_ERROR_C) || \
    !defined(MBEDTLS_SSL_TLS_C)
int main(void)
{
    printf("MBEDTLS_X509_CRT_PARSE_C and/or MBEDTLS_ERROR_C and/or "
           "MBEDTLS_SSL_TLS_C not defined.\n");
    return 0;
}
#else

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#if defined(MBEDTLS_HAVE_TIME)
#include <time.h>
#endif
#include "mbedtls/ssl.h"
#include "mbedtls/error.h"
#include "mbedtls/base64.h"
#include "mbedtls/md.h"
#include "mbedtls/x509_crt.h"
#include "mbedtls/ssl_ciphersuites.h"

/*
 * This program version
 */
#define PROG_NAME "ssl_context_info"
#define VER_MAJOR 0
#define VER_MINOR 1

/*
 * Flags copied from the Mbed TLS library.
 */
#define SESSION_CONFIG_TIME_BIT          (1 << 0)
#define SESSION_CONFIG_CRT_BIT           (1 << 1)
#define SESSION_CONFIG_CLIENT_TICKET_BIT (1 << 2)
#define SESSION_CONFIG_MFL_BIT           (1 << 3)
#define SESSION_CONFIG_TRUNC_HMAC_BIT    (1 << 4)
#define SESSION_CONFIG_ETM_BIT           (1 << 5)
#define SESSION_CONFIG_TICKET_BIT        (1 << 6)

#define CONTEXT_CONFIG_DTLS_CONNECTION_ID_BIT    (1 << 0)
#define CONTEXT_CONFIG_DTLS_BADMAC_LIMIT_BIT     (1 << 1)
#define CONTEXT_CONFIG_DTLS_ANTI_REPLAY_BIT      (1 << 2)
#define CONTEXT_CONFIG_ALPN_BIT                  (1 << 3)

#define TRANSFORM_RANDBYTE_LEN  64

/*
 * Minimum and maximum number of bytes for specific data: context, sessions,
 * certificates, tickets and buffers in the program. The context and session
 * size values have been calculated based on the 'print_deserialized_ssl_context()'
 * and 'print_deserialized_ssl_session()' content.
 */
#define MIN_CONTEXT_LEN     84
#define MIN_SESSION_LEN     88

#define MAX_CONTEXT_LEN     875     /* without session data */
#define MAX_SESSION_LEN     109     /* without certificate and ticket data */
#define MAX_CERTIFICATE_LEN ((1 << 24) - 1)
#define MAX_TICKET_LEN      ((1 << 24) - 1)

#define MIN_SERIALIZED_DATA (MIN_CONTEXT_LEN + MIN_SESSION_LEN)
#define MAX_SERIALIZED_DATA (MAX_CONTEXT_LEN + MAX_SESSION_LEN + \
                             MAX_CERTIFICATE_LEN + MAX_TICKET_LEN)

#define MIN_BASE64_LEN      (MIN_SERIALIZED_DATA * 4 / 3)
#define MAX_BASE64_LEN      (MAX_SERIALIZED_DATA * 4 / 3 + 3)

/*
 * A macro that prevents from reading out of the ssl buffer range.
 */
#define CHECK_SSL_END(LEN)            \
    do                                      \
    {                                       \
        if (end - ssl < (int) (LEN))      \
        {                                   \
            printf_err("%s", buf_ln_err); \
            return;                         \
        }                                   \
    } while (0)

/*
 * Global values
 */
FILE *b64_file = NULL;                  /* file with base64 codes to deserialize */
char conf_keep_peer_certificate = 1;    /* MBEDTLS_SSL_KEEP_PEER_CERTIFICATE from mbedTLS configuration */
char conf_dtls_proto = 1;               /* MBEDTLS_SSL_PROTO_DTLS from mbedTLS configuration */
char debug = 0;                         /* flag for debug messages */
const char alloc_err[] = "Cannot allocate memory\n";
const char buf_ln_err[] = "Buffer does not have enough data to complete the parsing\n";

/*
 * Basic printing functions
 */
static void print_version(void)
{
    printf("%s v%d.%d\n", PROG_NAME, VER_MAJOR, VER_MINOR);
}

static void print_usage(void)
{
    print_version();
    printf("\nThis program is used to deserialize an Mbed TLS SSL session from the base64 code provided\n"
           "in the text file. The program can deserialize many codes from one file, but they must be\n"
           "separated, e.g. by a newline.\n\n");
    printf(
        "Usage:\n"
        "\t-f path            - Path to the file with base64 code\n"
        "\t-v                 - Show version\n"
        "\t-h                 - Show this usage\n"
        "\t-d                 - Print more information\n"
        "\t--keep-peer-cert=0 - Use this option if you know that the Mbed TLS library\n"
        "\t                     has been compiled with the MBEDTLS_SSL_KEEP_PEER_CERTIFICATE\n"
        "\t                     flag. You can also use it if there are some problems with reading\n"
        "\t                     the information about certificate\n"
        "\t--dtls-protocol=0  - Use this option if you know that the Mbed TLS library\n"
        "\t                     has been compiled without the MBEDTLS_SSL_PROTO_DTLS flag\n"
        "\n"
        );
}

static void printf_dbg(const char *str, ...)
{
    if (debug) {
        va_list args;
        va_start(args, str);
        printf("debug: ");
        vprintf(str, args);
        fflush(stdout);
        va_end(args);
    }
}

MBEDTLS_PRINTF_ATTRIBUTE(1, 2)
static void printf_err(const char *str, ...)
{
    va_list args;
    va_start(args, str);
    fflush(stdout);
    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, str, args);
    fflush(stderr);
    va_end(args);
}

/*
 * Exit from the program in case of error
 */
static void error_exit(void)
{
    if (NULL != b64_file) {
        fclose(b64_file);
    }
    exit(-1);
}

/*
 * This function takes the input arguments of this program
 */
static void parse_arguments(int argc, char *argv[])
{
    int i = 1;

    if (argc < 2) {
        print_usage();
        error_exit();
    }

    while (i < argc) {
        if (strcmp(argv[i], "-d") == 0) {
            debug = 1;
        } else if (strcmp(argv[i], "-h") == 0) {
            print_usage();
        } else if (strcmp(argv[i], "-v") == 0) {
            print_version();
        } else if (strcmp(argv[i], "-f") == 0) {
            if (++i >= argc) {
                printf_err("File path is empty\n");
                error_exit();
            }

            if (NULL != b64_file) {
                printf_err("Cannot specify more than one file with -f\n");
                error_exit();
            }

            if ((b64_file = fopen(argv[i], "r")) == NULL) {
                printf_err("Cannot find file \"%s\"\n", argv[i]);
                error_exit();
            }
        } else if (strcmp(argv[i], "--keep-peer-cert=0") == 0) {
            conf_keep_peer_certificate = 0;
        } else if (strcmp(argv[i], "--dtls-protocol=0") == 0) {
            conf_dtls_proto = 0;
        } else {
            print_usage();
            error_exit();
        }

        i++;
    }
}

/*
 * This function prints base64 code to the stdout
 */
static void print_b64(const uint8_t *b, size_t len)
{
    size_t i = 0;
    const uint8_t *end = b + len;
    printf("\t");
    while (b < end) {
        if (++i > 75) {
            printf("\n\t");
            i = 0;
        }
        printf("%c", *b++);
    }
    printf("\n");
    fflush(stdout);
}

/*
 * This function prints hex code from the buffer to the stdout.
 *
 * /p b         buffer with data to print
 * /p len       number of bytes to print
 * /p in_line   number of bytes in one line
 * /p prefix    prefix for the new lines
 */
static void print_hex(const uint8_t *b, size_t len,
                      const size_t in_line, const char *prefix)
{
    size_t i = 0;
    const uint8_t *end = b + len;

    if (prefix == NULL) {
        prefix = "";
    }

    while (b < end) {
        if (++i > in_line) {
            printf("\n%s", prefix);
            i = 1;
        }
        printf("%02X ", (uint8_t) *b++);
    }
    printf("\n");
    fflush(stdout);
}

/*
 *  Print the value of time_t in format e.g. 2020-01-23 13:05:59
 */
static void print_time(const uint64_t *time)
{
#if defined(MBEDTLS_HAVE_TIME)
    char buf[20];
    struct tm *t = gmtime((time_t *) time);
    static const char format[] = "%Y-%m-%d %H:%M:%S";
    if (NULL != t) {
        strftime(buf, sizeof(buf), format, t);
        printf("%s\n", buf);
    } else {
        printf("unknown\n");
    }
#else
    (void) time;
    printf("not supported\n");
#endif
}

/*
 * Print the input string if the bit is set in the value
 */
static void print_if_bit(const char *str, int bit, int val)
{
    if (bit & val) {
        printf("\t%s\n", str);
    }
}

/*
 * Return pointer to hardcoded "enabled" or "disabled" depending on the input value
 */
static const char *get_enabled_str(int is_en)
{
    return (is_en) ? "enabled" : "disabled";
}

/*
 * Return pointer to hardcoded MFL string value depending on the MFL code at the input
 */
static const char *get_mfl_str(int mfl_code)
{
    switch (mfl_code) {
        case MBEDTLS_SSL_MAX_FRAG_LEN_NONE:
            return "none";
        case MBEDTLS_SSL_MAX_FRAG_LEN_512:
            return "512";
        case MBEDTLS_SSL_MAX_FRAG_LEN_1024:
            return "1024";
        case MBEDTLS_SSL_MAX_FRAG_LEN_2048:
            return "2048";
        case MBEDTLS_SSL_MAX_FRAG_LEN_4096:
            return "4096";
        default:
            return "error";
    }
}

/*
 * Read next base64 code from the 'b64_file'. The 'b64_file' must be opened
 * previously. After each call to this function, the internal file position
 * indicator of the global b64_file is advanced.
 *
 * Note - This function checks the size of the input buffer and if necessary,
 *        increases it to the maximum MAX_BASE64_LEN
 *
 * /p b64       pointer to the pointer of the buffer for input data
 * /p max_len   pointer to the current buffer capacity. It can be changed if
 *              the buffer needs to be increased
 *
 * \retval      number of bytes written in to the b64 buffer or 0 in case no more
 *              data was found
 */
static size_t read_next_b64_code(uint8_t **b64, size_t *max_len)
{
    int valid_balance = 0;  /* balance between valid and invalid characters */
    size_t len = 0;
    char pad = 0;
    int c = 0;

    while (EOF != c) {
        char c_valid = 0;

        c = fgetc(b64_file);

        if (pad > 0) {
            if (c == '=' && pad == 1) {
                c_valid = 1;
                pad = 2;
            }
        } else if ((c >= 'A' && c <= 'Z') ||
                   (c >= 'a' && c <= 'z') ||
                   (c >= '0' && c <= '9') ||
                   c == '+' || c == '/') {
            c_valid = 1;
        } else if (c == '=') {
            c_valid = 1;
            pad = 1;
        } else if (c == '-') {
            c = '+';
            c_valid = 1;
        } else if (c == '_') {
            c = '/';
            c_valid = 1;
        }

        if (c_valid) {
            /* A string of characters that could be a base64 code. */
            valid_balance++;

            if (len < *max_len) {
                (*b64)[len++] = c;
            } else if (*max_len < MAX_BASE64_LEN) {
                /* Current buffer is too small, but can be resized. */
                void *ptr;
                size_t new_size = (MAX_BASE64_LEN - 4096 > *max_len) ?
                                  *max_len + 4096 : MAX_BASE64_LEN;

                ptr = realloc(*b64, new_size);
                if (NULL == ptr) {
                    printf_err(alloc_err);
                    return 0;
                }
                *b64 = ptr;
                *max_len = new_size;
                (*b64)[len++] = c;
            } else {
                /* Too much data so it will be treated as invalid */
                len++;
            }
        } else if (len > 0) {
            /* End of a string that could be a base64 code, but need to check
             * that the length of the characters is correct. */

            valid_balance--;

            if (len < MIN_CONTEXT_LEN) {
                printf_dbg("The code found is too small to be a SSL context.\n");
                len = pad = 0;
            } else if (len > *max_len) {
                printf_err("The code found is too large by %" MBEDTLS_PRINTF_SIZET " bytes.\n",
                           len - *max_len);
                len = pad = 0;
            } else if (len % 4 != 0) {
                printf_err("The length of the base64 code found should be a multiple of 4.\n");
                len = pad = 0;
            } else {
                /* Base64 code with valid character length. */
                return len;
            }
        } else {
            valid_balance--;
        }

        /* Detection of potentially wrong file format like: binary, zip, ISO, etc. */
        if (valid_balance < -100) {
            printf_err("Too many bad symbols detected. File check aborted.\n");
            return 0;
        }
    }

    printf_dbg("End of file\n");
    return 0;
}

#if !defined(MBEDTLS_X509_REMOVE_INFO)
/*
 * This function deserializes and prints to the stdout all obtained information
 * about the certificates from provided data.
 *
 * /p ssl   pointer to serialized certificate
 * /p len   number of bytes in the buffer
 */
static void print_deserialized_ssl_cert(const uint8_t *ssl, uint32_t len)
{
    enum { STRLEN = 4096 };
    mbedtls_x509_crt crt;
    int ret;
    char str[STRLEN];

    printf("\nCertificate:\n");

    mbedtls_x509_crt_init(&crt);
    ret = mbedtls_x509_crt_parse_der(&crt, ssl, len);
    if (0 != ret) {
        mbedtls_strerror(ret, str, STRLEN);
        printf_err("Invalid format of X.509 - %s\n", str);
        printf("Cannot deserialize:\n\t");
        print_hex(ssl, len, 25, "\t");
    } else {
        mbedtls_x509_crt *current = &crt;

        while (current != NULL) {
            ret = mbedtls_x509_crt_info(str, STRLEN, "\t", current);
            if (0 > ret) {
                mbedtls_strerror(ret, str, STRLEN);
                printf_err("Cannot write to the output - %s\n", str);
            } else {
                printf("%s", str);
            }

            current = current->next;

            if (current) {
                printf("\n");
            }

        }
    }

    mbedtls_x509_crt_free(&crt);
}
#endif /* !MBEDTLS_X509_REMOVE_INFO */

/*
 * This function deserializes and prints to the stdout all obtained information
 * about the session from provided data. This function was built based on
 * mbedtls_ssl_session_load(). mbedtls_ssl_session_load() could not be used
 * due to dependencies on the mbedTLS configuration.
 *
 * The data structure in the buffer:
 *  uint64 start_time;
 *  uint8 ciphersuite[2];        // defined by the standard
 *  uint8 compression;           // 0 or 1
 *  uint8 session_id_len;        // at most 32
 *  opaque session_id[32];
 *  opaque master[48];           // fixed length in the standard
 *  uint32 verify_result;
 *  opaque peer_cert<0..2^24-1>; // length 0 means no peer cert
 *  opaque ticket<0..2^24-1>;    // length 0 means no ticket
 *  uint32 ticket_lifetime;
 *  uint8 mfl_code;              // up to 255 according to standard
 *  uint8 trunc_hmac;            // 0 or 1
 *  uint8 encrypt_then_mac;      // 0 or 1
 *
 * /p ssl               pointer to serialized session
 * /p len               number of bytes in the buffer
 * /p session_cfg_flag  session configuration flags
 */
static void print_deserialized_ssl_session(const uint8_t *ssl, uint32_t len,
                                           int session_cfg_flag)
{
    const struct mbedtls_ssl_ciphersuite_t *ciphersuite_info;
    int ciphersuite_id;
    uint32_t cert_len, ticket_len;
    uint32_t verify_result, ticket_lifetime;
    const uint8_t *end = ssl + len;

    printf("\nSession info:\n");

    if (session_cfg_flag & SESSION_CONFIG_TIME_BIT) {
        uint64_t start;
        CHECK_SSL_END(8);
        start = ((uint64_t) ssl[0] << 56) |
                ((uint64_t) ssl[1] << 48) |
                ((uint64_t) ssl[2] << 40) |
                ((uint64_t) ssl[3] << 32) |
                ((uint64_t) ssl[4] << 24) |
                ((uint64_t) ssl[5] << 16) |
                ((uint64_t) ssl[6] <<  8) |
                ((uint64_t) ssl[7]);
        ssl += 8;
        printf("\tstart time     : ");
        print_time(&start);
    }

    CHECK_SSL_END(2);
    ciphersuite_id = ((int) ssl[0] << 8) | (int) ssl[1];
    printf_dbg("Ciphersuite ID: %d\n", ciphersuite_id);
    ssl += 2;

    ciphersuite_info = mbedtls_ssl_ciphersuite_from_id(ciphersuite_id);
    if (ciphersuite_info == NULL) {
        printf_err("Cannot find ciphersuite info\n");
    } else {
        printf("\tciphersuite    : %s\n", mbedtls_ssl_ciphersuite_get_name(ciphersuite_info));
        printf("\tcipher flags   : 0x%02X\n", ciphersuite_info->MBEDTLS_PRIVATE(flags));
        printf("\tcipher type     : %d\n", ciphersuite_info->MBEDTLS_PRIVATE(cipher));
        printf("\tMessage-Digest : %d\n", ciphersuite_info->MBEDTLS_PRIVATE(mac));
    }

    CHECK_SSL_END(1);
    printf("\tcompression    : %s\n", get_enabled_str(*ssl++));

    /* Note - Here we can get session ID length from serialized data, but we
     * use hardcoded 32-bytes length. This approach was taken from
     * 'mbedtls_ssl_session_load()'. */
    CHECK_SSL_END(1 + 32);
    printf_dbg("Session id length: %u\n", (uint32_t) *ssl++);
    printf("\tsession ID     : ");
    print_hex(ssl, 32, 16, "\t                 ");
    ssl += 32;

    printf("\tmaster secret  : ");
    CHECK_SSL_END(48);
    print_hex(ssl, 48, 16, "\t                 ");
    ssl += 48;

    CHECK_SSL_END(4);
    verify_result = ((uint32_t) ssl[0] << 24) |
                    ((uint32_t) ssl[1] << 16) |
                    ((uint32_t) ssl[2] <<  8) |
                    ((uint32_t) ssl[3]);
    ssl += 4;
    printf("\tverify result  : 0x%08X\n", verify_result);

    if (SESSION_CONFIG_CRT_BIT & session_cfg_flag) {
        if (conf_keep_peer_certificate) {
            CHECK_SSL_END(3);
            cert_len = ((uint32_t) ssl[0] << 16) |
                       ((uint32_t) ssl[1] <<  8) |
                       ((uint32_t) ssl[2]);
            ssl += 3;
            printf_dbg("Certificate length: %u\n", cert_len);

            if (cert_len > 0) {
                CHECK_SSL_END(cert_len);
#if !defined(MBEDTLS_X509_REMOVE_INFO)
                print_deserialized_ssl_cert(ssl, cert_len);
#endif
                ssl += cert_len;
            }
        } else {
            printf("\tPeer digest    : ");

            CHECK_SSL_END(1);
            switch ((mbedtls_md_type_t) *ssl++) {
                case MBEDTLS_MD_NONE:
                    printf("none\n");
                    break;
                case MBEDTLS_MD_MD5:
                    printf("MD5\n");
                    break;
                case MBEDTLS_MD_SHA1:
                    printf("SHA1\n");
                    break;
                case MBEDTLS_MD_SHA224:
                    printf("SHA224\n");
                    break;
                case MBEDTLS_MD_SHA256:
                    printf("SHA256\n");
                    break;
                case MBEDTLS_MD_SHA384:
                    printf("SHA384\n");
                    break;
                case MBEDTLS_MD_SHA512:
                    printf("SHA512\n");
                    break;
                case MBEDTLS_MD_RIPEMD160:
                    printf("RIPEMD160\n");
                    break;
                default:
                    printf("undefined or erroneous\n");
                    break;
            }

            CHECK_SSL_END(1);
            cert_len  = (uint32_t) *ssl++;
            printf_dbg("Message-Digest length: %u\n", cert_len);

            if (cert_len > 0) {
                printf("\tPeer digest cert : ");
                CHECK_SSL_END(cert_len);
                print_hex(ssl, cert_len, 16, "\t                   ");
                ssl += cert_len;
            }
        }
    }

    if (SESSION_CONFIG_CLIENT_TICKET_BIT & session_cfg_flag) {
        printf("\nTicket:\n");

        CHECK_SSL_END(3);
        ticket_len = ((uint32_t) ssl[0] << 16) |
                     ((uint32_t) ssl[1] <<  8) |
                     ((uint32_t) ssl[2]);
        ssl += 3;
        printf_dbg("Ticket length: %u\n", ticket_len);

        if (ticket_len > 0) {
            printf("\t");
            CHECK_SSL_END(ticket_len);
            print_hex(ssl, ticket_len, 22, "\t");
            ssl += ticket_len;
            printf("\n");
        }

        CHECK_SSL_END(4);
        ticket_lifetime = ((uint32_t) ssl[0] << 24) |
                          ((uint32_t) ssl[1] << 16) |
                          ((uint32_t) ssl[2] <<  8) |
                          ((uint32_t) ssl[3]);
        ssl += 4;
        printf("\tlifetime : %u sec.\n", ticket_lifetime);
    }

    if (ssl < end) {
        printf("\nSession others:\n");
    }

    if (SESSION_CONFIG_MFL_BIT & session_cfg_flag) {
        CHECK_SSL_END(1);
        printf("\tMFL                      : %s\n", get_mfl_str(*ssl++));
    }

    if (SESSION_CONFIG_TRUNC_HMAC_BIT & session_cfg_flag) {
        CHECK_SSL_END(1);
        printf("\tnegotiate truncated HMAC : %s\n", get_enabled_str(*ssl++));
    }

    if (SESSION_CONFIG_ETM_BIT & session_cfg_flag) {
        CHECK_SSL_END(1);
        printf("\tEncrypt-then-MAC         : %s\n", get_enabled_str(*ssl++));
    }

    if (0 != (end - ssl)) {
        printf_err("%i bytes left to analyze from session\n", (int32_t) (end - ssl));
    }
}

/*
 * This function deserializes and prints to the stdout all obtained information
 * about the context from provided data. This function was built based on
 * mbedtls_ssl_context_load(). mbedtls_ssl_context_load() could not be used
 * due to dependencies on the mbedTLS configuration and the configuration of
 * the context when serialization was created.
 *
 * The data structure in the buffer:
 *  // header
 *  uint8 version[3];
 *  uint8 configuration[5];
 *  // session sub-structure
 *  uint32_t session_len;
 *  opaque session<1..2^32-1>;  // see mbedtls_ssl_session_save()
 *  // transform sub-structure
 *  uint8 random[64];           // ServerHello.random+ClientHello.random
 *  uint8 in_cid_len;
 *  uint8 in_cid<0..2^8-1>      // Connection ID: expected incoming value
 *  uint8 out_cid_len;
 *  uint8 out_cid<0..2^8-1>     // Connection ID: outgoing value to use
 *  // fields from ssl_context
 *  uint32 badmac_seen;         // DTLS: number of records with failing MAC
 *  uint64 in_window_top;       // DTLS: last validated record seq_num
 *  uint64 in_window;           // DTLS: bitmask for replay protection
 *  uint8 disable_datagram_packing; // DTLS: only one record per datagram
 *  uint64 cur_out_ctr;         // Record layer: outgoing sequence number
 *  uint16 mtu;                 // DTLS: path mtu (max outgoing fragment size)
 *  uint8 alpn_chosen_len;
 *  uint8 alpn_chosen<0..2^8-1> // ALPN: negotiated application protocol
 *
 * /p ssl   pointer to serialized session
 * /p len   number of bytes in the buffer
 */
static void print_deserialized_ssl_context(const uint8_t *ssl, size_t len)
{
    const uint8_t *end = ssl + len;
    uint32_t session_len;
    int session_cfg_flag;
    int context_cfg_flag;

    printf("\nMbed TLS version:\n");

    CHECK_SSL_END(3 + 2 + 3);

    printf("\tmajor    %u\n", (uint32_t) *ssl++);
    printf("\tminor    %u\n", (uint32_t) *ssl++);
    printf("\tpath     %u\n", (uint32_t) *ssl++);

    printf("\nEnabled session and context configuration:\n");

    session_cfg_flag = ((int) ssl[0] << 8) | ((int) ssl[1]);
    ssl += 2;

    context_cfg_flag = ((int) ssl[0] << 16) |
                       ((int) ssl[1] <<  8) |
                       ((int) ssl[2]);
    ssl += 3;

    printf_dbg("Session config flags 0x%04X\n", session_cfg_flag);
    printf_dbg("Context config flags 0x%06X\n", context_cfg_flag);

    print_if_bit("MBEDTLS_HAVE_TIME", SESSION_CONFIG_TIME_BIT, session_cfg_flag);
    print_if_bit("MBEDTLS_X509_CRT_PARSE_C", SESSION_CONFIG_CRT_BIT, session_cfg_flag);
    print_if_bit("MBEDTLS_SSL_MAX_FRAGMENT_LENGTH", SESSION_CONFIG_MFL_BIT, session_cfg_flag);
    print_if_bit("MBEDTLS_SSL_ENCRYPT_THEN_MAC", SESSION_CONFIG_ETM_BIT, session_cfg_flag);
    print_if_bit("MBEDTLS_SSL_SESSION_TICKETS", SESSION_CONFIG_TICKET_BIT, session_cfg_flag);
    print_if_bit("MBEDTLS_SSL_SESSION_TICKETS and client",
                 SESSION_CONFIG_CLIENT_TICKET_BIT,
                 session_cfg_flag);

    print_if_bit("MBEDTLS_SSL_DTLS_CONNECTION_ID",
                 CONTEXT_CONFIG_DTLS_CONNECTION_ID_BIT,
                 context_cfg_flag);
    print_if_bit("MBEDTLS_SSL_DTLS_ANTI_REPLAY",
                 CONTEXT_CONFIG_DTLS_ANTI_REPLAY_BIT,
                 context_cfg_flag);
    print_if_bit("MBEDTLS_SSL_ALPN", CONTEXT_CONFIG_ALPN_BIT, context_cfg_flag);

    CHECK_SSL_END(4);
    session_len = ((uint32_t) ssl[0] << 24) |
                  ((uint32_t) ssl[1] << 16) |
                  ((uint32_t) ssl[2] <<  8) |
                  ((uint32_t) ssl[3]);
    ssl += 4;
    printf_dbg("Session length %u\n", session_len);

    CHECK_SSL_END(session_len);
    print_deserialized_ssl_session(ssl, session_len, session_cfg_flag);
    ssl += session_len;

    printf("\nRandom bytes:\n\t");

    CHECK_SSL_END(TRANSFORM_RANDBYTE_LEN);
    print_hex(ssl, TRANSFORM_RANDBYTE_LEN, 22, "\t");
    ssl += TRANSFORM_RANDBYTE_LEN;

    printf("\nContext others:\n");

    if (CONTEXT_CONFIG_DTLS_CONNECTION_ID_BIT & context_cfg_flag) {
        uint8_t cid_len;

        CHECK_SSL_END(1);
        cid_len = *ssl++;
        printf_dbg("In CID length %u\n", (uint32_t) cid_len);

        printf("\tin CID                             : ");
        if (cid_len > 0) {
            CHECK_SSL_END(cid_len);
            print_hex(ssl, cid_len, 20, "\t");
            ssl += cid_len;
        } else {
            printf("none\n");
        }

        CHECK_SSL_END(1);
        cid_len = *ssl++;
        printf_dbg("Out CID length %u\n", (uint32_t) cid_len);

        printf("\tout CID                            : ");
        if (cid_len > 0) {
            CHECK_SSL_END(cid_len);
            print_hex(ssl, cid_len, 20, "\t");
            ssl += cid_len;
        } else {
            printf("none\n");
        }
    }

    if (CONTEXT_CONFIG_DTLS_BADMAC_LIMIT_BIT & context_cfg_flag) {
        uint32_t badmac_seen;

        CHECK_SSL_END(4);
        badmac_seen = ((uint32_t) ssl[0] << 24) |
                      ((uint32_t) ssl[1] << 16) |
                      ((uint32_t) ssl[2] <<  8) |
                      ((uint32_t) ssl[3]);
        ssl += 4;
        printf("\tbad MAC seen number                : %u\n", badmac_seen);

        /* value 'in_window_top' from mbedtls_ssl_context */
        printf("\tlast validated record sequence no. : ");
        CHECK_SSL_END(8);
        print_hex(ssl, 8, 20, "");
        ssl += 8;

        /* value 'in_window' from mbedtls_ssl_context */
        printf("\tbitmask for replay detection       : ");
        CHECK_SSL_END(8);
        print_hex(ssl, 8, 20, "");
        ssl += 8;
    }

    if (conf_dtls_proto) {
        CHECK_SSL_END(1);
        printf("\tDTLS datagram packing              : %s\n",
               get_enabled_str(!(*ssl++)));
    }

    /* value 'cur_out_ctr' from mbedtls_ssl_context */
    printf("\toutgoing record sequence no.       : ");
    CHECK_SSL_END(8);
    print_hex(ssl, 8, 20, "");
    ssl += 8;

    if (conf_dtls_proto) {
        uint16_t mtu;
        CHECK_SSL_END(2);
        mtu = (ssl[0] << 8) | ssl[1];
        ssl += 2;
        printf("\tMTU                                : %u\n", mtu);
    }


    if (CONTEXT_CONFIG_ALPN_BIT & context_cfg_flag) {
        uint8_t alpn_len;

        CHECK_SSL_END(1);
        alpn_len = *ssl++;
        printf_dbg("ALPN length %u\n", (uint32_t) alpn_len);

        printf("\tALPN negotiation                   : ");
        CHECK_SSL_END(alpn_len);
        if (alpn_len > 0) {
            if (strlen((const char *) ssl) == alpn_len) {
                printf("%s\n", ssl);
            } else {
                printf("\n");
                printf_err("\tALPN negotiation is incorrect\n");
            }
            ssl += alpn_len;
        } else {
            printf("not selected\n");
        }
    }

    if (0 != (end - ssl)) {
        printf_err("%i bytes left to analyze from context\n", (int32_t) (end - ssl));
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    enum { SSL_INIT_LEN = 4096 };

    uint32_t b64_counter = 0;
    uint8_t *b64_buf = NULL;
    uint8_t *ssl_buf = NULL;
    size_t b64_max_len = SSL_INIT_LEN;
    size_t ssl_max_len = SSL_INIT_LEN;
    size_t ssl_len = 0;

    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        mbedtls_fprintf(stderr, "Failed to initialize PSA Crypto implementation: %d\n",
                        (int) status);
        return MBEDTLS_ERR_SSL_HW_ACCEL_FAILED;
    }

    /* The 'b64_file' is opened when parsing arguments to check that the
     * file name is correct */
    parse_arguments(argc, argv);

    if (NULL != b64_file) {
        b64_buf = malloc(SSL_INIT_LEN);
        ssl_buf = malloc(SSL_INIT_LEN);

        if (NULL == b64_buf || NULL == ssl_buf) {
            printf_err(alloc_err);
            fclose(b64_file);
            b64_file = NULL;
        }
    }

    while (NULL != b64_file) {
        size_t b64_len = read_next_b64_code(&b64_buf, &b64_max_len);
        if (b64_len > 0) {
            int ret;
            size_t ssl_required_len = b64_len * 3 / 4 + 1;

            /* Allocate more memory if necessary. */
            if (ssl_required_len > ssl_max_len) {
                void *ptr = realloc(ssl_buf, ssl_required_len);
                if (NULL == ptr) {
                    printf_err(alloc_err);
                    fclose(b64_file);
                    b64_file = NULL;
                    break;
                }
                ssl_buf = ptr;
                ssl_max_len = ssl_required_len;
            }

            printf("\nDeserializing number %u:\n",  ++b64_counter);

            printf("\nBase64 code:\n");
            print_b64(b64_buf, b64_len);

            ret = mbedtls_base64_decode(ssl_buf, ssl_max_len, &ssl_len, b64_buf, b64_len);
            if (ret != 0) {
                mbedtls_strerror(ret, (char *) b64_buf, b64_max_len);
                printf_err("base64 code cannot be decoded - %s\n", b64_buf);
                continue;
            }

            if (debug) {
                printf("\nDecoded data in hex:\n\t");
                print_hex(ssl_buf, ssl_len, 25, "\t");
            }

            print_deserialized_ssl_context(ssl_buf, ssl_len);

        } else {
            fclose(b64_file);
            b64_file = NULL;
        }
    }

    free(b64_buf);
    free(ssl_buf);

    if (b64_counter > 0) {
        printf_dbg("Finished. Found %u base64 codes\n", b64_counter);
    } else {
        printf("Finished. No valid base64 code found\n");
    }

    mbedtls_psa_crypto_free();

    return 0;
}

#endif /* MBEDTLS_X509_CRT_PARSE_C */
