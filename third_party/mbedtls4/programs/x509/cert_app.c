/*
 *  Certificate reading application
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "mbedtls/build_info.h"

#include "mbedtls/platform.h"

#if !defined(MBEDTLS_BIGNUM_C) || \
    !defined(MBEDTLS_SSL_TLS_C) || !defined(MBEDTLS_SSL_CLI_C) || \
    !defined(MBEDTLS_NET_C) || !defined(MBEDTLS_RSA_C) ||         \
    !defined(MBEDTLS_X509_CRT_PARSE_C) || !defined(MBEDTLS_FS_IO) ||  \
    defined(MBEDTLS_X509_REMOVE_INFO)
int main(void)
{
    mbedtls_printf("MBEDTLS_BIGNUM_C and/or "
                   "MBEDTLS_SSL_TLS_C and/or MBEDTLS_SSL_CLI_C and/or "
                   "MBEDTLS_NET_C and/or MBEDTLS_RSA_C and/or "
                   "MBEDTLS_X509_CRT_PARSE_C and/or MBEDTLS_FS_IO and/or "
                   "and/or MBEDTLS_X509_REMOVE_INFO defined.\n");
    mbedtls_exit(0);
}
#else

#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/x509.h"
#include "mbedtls/debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MODE_NONE               0
#define MODE_FILE               1
#define MODE_SSL                2

#define DFL_MODE                MODE_NONE
#define DFL_FILENAME            "cert.crt"
#define DFL_CA_FILE             ""
#define DFL_CRL_FILE            ""
#define DFL_CA_PATH             ""
#define DFL_SERVER_NAME         "localhost"
#define DFL_SERVER_PORT         "4433"
#define DFL_DEBUG_LEVEL         0
#define DFL_PERMISSIVE          0

#define USAGE_IO \
    "    ca_file=%%s          The single file containing the top-level CA(s) you fully trust\n" \
    "                        default: \"\" (none)\n" \
    "    crl_file=%%s         The single CRL file you want to use\n" \
    "                        default: \"\" (none)\n" \
    "    ca_path=%%s          The path containing the top-level CA(s) you fully trust\n" \
    "                        default: \"\" (none) (overrides ca_file)\n"

#define USAGE \
    "\n usage: cert_app param=<>...\n"                  \
    "\n acceptable parameters:\n"                       \
    "    mode=file|ssl       default: none\n"           \
    "    filename=%%s         default: cert.crt\n"      \
    USAGE_IO                                            \
    "    server_name=%%s      default: localhost\n"     \
    "    server_port=%%d      default: 4433\n"          \
    "    debug_level=%%d      default: 0 (disabled)\n"  \
    "    permissive=%%d       default: 0 (disabled)\n"  \
    "\n"


/*
 * global options
 */
struct options {
    int mode;                   /* the mode to run the application in   */
    const char *filename;       /* filename of the certificate file     */
    const char *ca_file;        /* the file with the CA certificate(s)  */
    const char *crl_file;       /* the file with the CRL to use         */
    const char *ca_path;        /* the path with the CA certificate(s) reside */
    const char *server_name;    /* hostname of the server (client only) */
    const char *server_port;    /* port on which the ssl service runs   */
    int debug_level;            /* level of debugging                   */
    int permissive;             /* permissive parsing                   */
} opt;

static void my_debug(void *ctx, int level,
                     const char *file, int line,
                     const char *str)
{
    ((void) level);

    mbedtls_fprintf((FILE *) ctx, "%s:%04d: %s", file, line, str);
    fflush((FILE *) ctx);
}

static int my_verify(void *data, mbedtls_x509_crt *crt, int depth, uint32_t *flags)
{
    char buf[1024];
    ((void) data);

    mbedtls_printf("\nVerify requested for (Depth %d):\n", depth);
    mbedtls_x509_crt_info(buf, sizeof(buf) - 1, "", crt);
    mbedtls_printf("%s", buf);

    if ((*flags) == 0) {
        mbedtls_printf("  This certificate has no flags\n");
    } else {
        mbedtls_x509_crt_verify_info(buf, sizeof(buf), "  ! ", *flags);
        mbedtls_printf("%s\n", buf);
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int ret = 1;
    int exit_code = MBEDTLS_EXIT_FAILURE;
    mbedtls_net_context server_fd;
    unsigned char buf[1024];
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt cacert;
    mbedtls_x509_crl cacrl;
    int i, j;
    uint32_t flags;
    int verify = 0;
    char *p, *q;

    /*
     * Set to sane values
     */
    mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_x509_crt_init(&cacert);
#if defined(MBEDTLS_X509_CRL_PARSE_C)
    mbedtls_x509_crl_init(&cacrl);
#else
    /* Zeroize structure as CRL parsing is not supported and we have to pass
       it to the verify function */
    memset(&cacrl, 0, sizeof(mbedtls_x509_crl));
#endif

    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        mbedtls_fprintf(stderr, "Failed to initialize PSA Crypto implementation: %d\n",
                        (int) status);
        goto exit;
    }

    if (argc < 2) {
usage:
        mbedtls_printf(USAGE);
        goto exit;
    }

    opt.mode                = DFL_MODE;
    opt.filename            = DFL_FILENAME;
    opt.ca_file             = DFL_CA_FILE;
    opt.crl_file            = DFL_CRL_FILE;
    opt.ca_path             = DFL_CA_PATH;
    opt.server_name         = DFL_SERVER_NAME;
    opt.server_port         = DFL_SERVER_PORT;
    opt.debug_level         = DFL_DEBUG_LEVEL;
    opt.permissive          = DFL_PERMISSIVE;

    for (i = 1; i < argc; i++) {
        p = argv[i];
        if ((q = strchr(p, '=')) == NULL) {
            goto usage;
        }
        *q++ = '\0';

        for (j = 0; p + j < q; j++) {
            if (argv[i][j] >= 'A' && argv[i][j] <= 'Z') {
                argv[i][j] |= 0x20;
            }
        }

        if (strcmp(p, "mode") == 0) {
            if (strcmp(q, "file") == 0) {
                opt.mode = MODE_FILE;
            } else if (strcmp(q, "ssl") == 0) {
                opt.mode = MODE_SSL;
            } else {
                goto usage;
            }
        } else if (strcmp(p, "filename") == 0) {
            opt.filename = q;
        } else if (strcmp(p, "ca_file") == 0) {
            opt.ca_file = q;
        } else if (strcmp(p, "crl_file") == 0) {
            opt.crl_file = q;
        } else if (strcmp(p, "ca_path") == 0) {
            opt.ca_path = q;
        } else if (strcmp(p, "server_name") == 0) {
            opt.server_name = q;
        } else if (strcmp(p, "server_port") == 0) {
            opt.server_port = q;
        } else if (strcmp(p, "debug_level") == 0) {
            opt.debug_level = atoi(q);
            if (opt.debug_level < 0 || opt.debug_level > 65535) {
                goto usage;
            }
        } else if (strcmp(p, "permissive") == 0) {
            opt.permissive = atoi(q);
            if (opt.permissive < 0 || opt.permissive > 1) {
                goto usage;
            }
        } else {
            goto usage;
        }
    }

    /*
     * 1.1. Load the trusted CA
     */
    mbedtls_printf("  . Loading the CA root certificate ...");
    fflush(stdout);

    if (strlen(opt.ca_path)) {
        if ((ret = mbedtls_x509_crt_parse_path(&cacert, opt.ca_path)) < 0) {
            mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse_path returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }

        verify = 1;
    } else if (strlen(opt.ca_file)) {
        if ((ret = mbedtls_x509_crt_parse_file(&cacert, opt.ca_file)) < 0) {
            mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse_file returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }

        verify = 1;
    }

    mbedtls_printf(" ok (%d skipped)\n", ret);

#if defined(MBEDTLS_X509_CRL_PARSE_C)
    if (strlen(opt.crl_file)) {
        if ((ret = mbedtls_x509_crl_parse_file(&cacrl, opt.crl_file)) != 0) {
            mbedtls_printf(" failed\n  !  mbedtls_x509_crl_parse returned -0x%x\n\n",
                           (unsigned int) -ret);
            goto exit;
        }

        verify = 1;
    }
#endif

    if (opt.mode == MODE_FILE) {
        mbedtls_x509_crt crt;
        mbedtls_x509_crt *cur = &crt;
        mbedtls_x509_crt_init(&crt);

        /*
         * 1.1. Load the certificate(s)
         */
        mbedtls_printf("\n  . Loading the certificate(s) ...");
        fflush(stdout);

        ret = mbedtls_x509_crt_parse_file(&crt, opt.filename);

        if (ret < 0) {
            mbedtls_printf(" failed\n  !  mbedtls_x509_crt_parse_file returned %d\n\n", ret);
            mbedtls_x509_crt_free(&crt);
            goto exit;
        }

        if (opt.permissive == 0 && ret > 0) {
            mbedtls_printf(
                " failed\n  !  mbedtls_x509_crt_parse failed to parse %d certificates\n\n",
                ret);
            mbedtls_x509_crt_free(&crt);
            goto exit;
        }

        mbedtls_printf(" ok\n");

        /*
         * 1.2 Print the certificate(s)
         */
        while (cur != NULL) {
            mbedtls_printf("  . Peer certificate information    ...\n");
            ret = mbedtls_x509_crt_info((char *) buf, sizeof(buf) - 1, "      ",
                                        cur);
            if (ret == -1) {
                mbedtls_printf(" failed\n  !  mbedtls_x509_crt_info returned %d\n\n", ret);
                mbedtls_x509_crt_free(&crt);
                goto exit;
            }

            mbedtls_printf("%s\n", buf);

            cur = cur->next;
        }

        /*
         * 1.3 Verify the certificate
         */
        if (verify) {
            mbedtls_printf("  . Verifying X.509 certificate...");

            if ((ret = mbedtls_x509_crt_verify(&crt, &cacert, &cacrl, NULL, &flags,
                                               my_verify, NULL)) != 0) {
                char vrfy_buf[512];

                mbedtls_printf(" failed\n");

                mbedtls_x509_crt_verify_info(vrfy_buf, sizeof(vrfy_buf), "  ! ", flags);

                mbedtls_printf("%s\n", vrfy_buf);
            } else {
                mbedtls_printf(" ok\n");
            }
        }

        mbedtls_x509_crt_free(&crt);
    } else if (opt.mode == MODE_SSL) {
        /*
         * 1. Initialize the RNG and the session data
         */
        mbedtls_printf("\n  . Seeding the random number generator...");
        fflush(stdout);

        mbedtls_printf(" ok\n");

#if defined(MBEDTLS_DEBUG_C)
        mbedtls_debug_set_threshold(opt.debug_level);
#endif

        /*
         * 2. Start the connection
         */
        mbedtls_printf("  . SSL connection to tcp/%s/%s...", opt.server_name,
                       opt.server_port);
        fflush(stdout);

        if ((ret = mbedtls_net_connect(&server_fd, opt.server_name,
                                       opt.server_port, MBEDTLS_NET_PROTO_TCP)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_net_connect returned %d\n\n", ret);
            goto ssl_exit;
        }

        /*
         * 3. Setup stuff
         */
        if ((ret = mbedtls_ssl_config_defaults(&conf,
                                               MBEDTLS_SSL_IS_CLIENT,
                                               MBEDTLS_SSL_TRANSPORT_STREAM,
                                               MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret);
            goto exit;
        }

        if (verify) {
            mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
            mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
            mbedtls_ssl_conf_verify(&conf, my_verify, NULL);
        } else {
            mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
        }

        mbedtls_ssl_conf_dbg(&conf, my_debug, stdout);

        if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret);
            goto ssl_exit;
        }

        if ((ret = mbedtls_ssl_set_hostname(&ssl, opt.server_name)) != 0) {
            mbedtls_printf(" failed\n  ! mbedtls_ssl_set_hostname returned %d\n\n", ret);
            goto ssl_exit;
        }

        mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

        /*
         * 4. Handshake
         */
        while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
                mbedtls_printf(" failed\n  ! mbedtls_ssl_handshake returned %d\n\n", ret);
                goto ssl_exit;
            }
        }

        mbedtls_printf(" ok\n");

        /*
         * 5. Print the certificate
         */
#if !defined(MBEDTLS_SSL_KEEP_PEER_CERTIFICATE)
        mbedtls_printf("  . Peer certificate information    ... skipped\n");
#else
        mbedtls_printf("  . Peer certificate information    ...\n");
        ret = mbedtls_x509_crt_info((char *) buf, sizeof(buf) - 1, "      ",
                                    mbedtls_ssl_get_peer_cert(&ssl));
        if (ret == -1) {
            mbedtls_printf(" failed\n  !  mbedtls_x509_crt_info returned %d\n\n", ret);
            goto ssl_exit;
        }

        mbedtls_printf("%s\n", buf);
#endif /* MBEDTLS_SSL_KEEP_PEER_CERTIFICATE */

        mbedtls_ssl_close_notify(&ssl);

ssl_exit:
        mbedtls_ssl_free(&ssl);
        mbedtls_ssl_config_free(&conf);
    } else {
        goto usage;
    }

    exit_code = MBEDTLS_EXIT_SUCCESS;

exit:

    mbedtls_net_free(&server_fd);
    mbedtls_x509_crt_free(&cacert);
#if defined(MBEDTLS_X509_CRL_PARSE_C)
    mbedtls_x509_crl_free(&cacrl);
#endif
    mbedtls_psa_crypto_free();

    mbedtls_exit(exit_code);
}
#endif /* MBEDTLS_BIGNUM_C && MBEDTLS_SSL_TLS_C &&
          MBEDTLS_SSL_CLI_C && MBEDTLS_NET_C && MBEDTLS_RSA_C &&
          MBEDTLS_X509_CRT_PARSE_C && MBEDTLS_FS_IO */
