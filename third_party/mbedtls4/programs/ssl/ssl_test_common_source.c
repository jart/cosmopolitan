/*
 *  Common source code for SSL test programs. This file is included by
 *  both ssl_client2.c and ssl_server2.c and is intended for source
 *  code that is textually identical in both programs, but that cannot be
 *  compiled separately because it refers to types or macros that are
 *  different in the two programs, or because it would have an incomplete
 *  type.
 *
 *  This file is meant to be #include'd and cannot be compiled separately.
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

static void eap_tls_key_derivation(void *p_expkey,
                                   mbedtls_ssl_key_export_type secret_type,
                                   const unsigned char *secret,
                                   size_t secret_len,
                                   const unsigned char client_random[32],
                                   const unsigned char server_random[32],
                                   mbedtls_tls_prf_types tls_prf_type)
{
    eap_tls_keys *keys = (eap_tls_keys *) p_expkey;

    /* We're only interested in the TLS 1.2 master secret */
    if (secret_type != MBEDTLS_SSL_KEY_EXPORT_TLS12_MASTER_SECRET) {
        return;
    }
    if (secret_len != sizeof(keys->master_secret)) {
        return;
    }

    memcpy(keys->master_secret, secret, sizeof(keys->master_secret));
    memcpy(keys->randbytes, client_random, 32);
    memcpy(keys->randbytes + 32, server_random, 32);
    keys->tls_prf_type = tls_prf_type;
}

static void nss_keylog_export(void *p_expkey,
                              mbedtls_ssl_key_export_type secret_type,
                              const unsigned char *secret,
                              size_t secret_len,
                              const unsigned char client_random[32],
                              const unsigned char server_random[32],
                              mbedtls_tls_prf_types tls_prf_type)
{
    char nss_keylog_line[200];
    size_t const client_random_len = 32;
    size_t len = 0;
    size_t j;

    /* We're only interested in the TLS 1.2 master secret */
    if (secret_type != MBEDTLS_SSL_KEY_EXPORT_TLS12_MASTER_SECRET) {
        return;
    }

    ((void) p_expkey);
    ((void) server_random);
    ((void) tls_prf_type);

    len += sprintf(nss_keylog_line + len,
                   "%s", "CLIENT_RANDOM ");

    for (j = 0; j < client_random_len; j++) {
        len += sprintf(nss_keylog_line + len,
                       "%02x", client_random[j]);
    }

    len += sprintf(nss_keylog_line + len, " ");

    for (j = 0; j < secret_len; j++) {
        len += sprintf(nss_keylog_line + len,
                       "%02x", secret[j]);
    }

    len += sprintf(nss_keylog_line + len, "\n");
    nss_keylog_line[len] = '\0';

    mbedtls_printf("\n");
    mbedtls_printf("---------------- NSS KEYLOG -----------------\n");
    mbedtls_printf("%s", nss_keylog_line);
    mbedtls_printf("---------------------------------------------\n");

    if (opt.nss_keylog_file != NULL) {
        FILE *f;

        if ((f = fopen(opt.nss_keylog_file, "a")) == NULL) {
            goto exit;
        }

        /* Ensure no stdio buffering of secrets, as such buffers cannot be
         * wiped. */
        mbedtls_setbuf(f, NULL);

        if (fwrite(nss_keylog_line, 1, len, f) != len) {
            fclose(f);
            goto exit;
        }

        fclose(f);
    }

exit:
    mbedtls_platform_zeroize(nss_keylog_line,
                             sizeof(nss_keylog_line));
}

#if defined(MBEDTLS_SSL_DTLS_SRTP)
static void dtls_srtp_key_derivation(void *p_expkey,
                                     mbedtls_ssl_key_export_type secret_type,
                                     const unsigned char *secret,
                                     size_t secret_len,
                                     const unsigned char client_random[32],
                                     const unsigned char server_random[32],
                                     mbedtls_tls_prf_types tls_prf_type)
{
    dtls_srtp_keys *keys = (dtls_srtp_keys *) p_expkey;

    /* We're only interested in the TLS 1.2 master secret */
    if (secret_type != MBEDTLS_SSL_KEY_EXPORT_TLS12_MASTER_SECRET) {
        return;
    }
    if (secret_len != sizeof(keys->master_secret)) {
        return;
    }

    memcpy(keys->master_secret, secret, sizeof(keys->master_secret));
    memcpy(keys->randbytes, client_random, 32);
    memcpy(keys->randbytes + 32, server_random, 32);
    keys->tls_prf_type = tls_prf_type;
}
#endif /* MBEDTLS_SSL_DTLS_SRTP */

static int ssl_check_record(mbedtls_ssl_context const *ssl,
                            unsigned char const *buf, size_t len)
{
    int my_ret = 0, ret_cr1, ret_cr2;
    unsigned char *tmp_buf;

    /* Record checking may modify the input buffer,
     * so make a copy. */
    tmp_buf = mbedtls_calloc(1, len);
    if (tmp_buf == NULL) {
        return MBEDTLS_ERR_SSL_ALLOC_FAILED;
    }
    memcpy(tmp_buf, buf, len);

    ret_cr1 = mbedtls_ssl_check_record(ssl, tmp_buf, len);
    if (ret_cr1 != MBEDTLS_ERR_SSL_FEATURE_UNAVAILABLE) {
        /* Test-only: Make sure that mbedtls_ssl_check_record()
         *            doesn't alter state. */
        memcpy(tmp_buf, buf, len);   /* Restore buffer */
        ret_cr2 = mbedtls_ssl_check_record(ssl, tmp_buf, len);
        if (ret_cr2 != ret_cr1) {
            mbedtls_printf("mbedtls_ssl_check_record() returned inconsistent results.\n");
            my_ret = -1;
            goto cleanup;
        }

        switch (ret_cr1) {
            case 0:
                break;

            case MBEDTLS_ERR_SSL_INVALID_RECORD:
                if (opt.debug_level > 1) {
                    mbedtls_printf("mbedtls_ssl_check_record() detected invalid record.\n");
                }
                break;

            case MBEDTLS_ERR_SSL_INVALID_MAC:
                if (opt.debug_level > 1) {
                    mbedtls_printf("mbedtls_ssl_check_record() detected unauthentic record.\n");
                }
                break;

            case MBEDTLS_ERR_SSL_UNEXPECTED_RECORD:
                if (opt.debug_level > 1) {
                    mbedtls_printf("mbedtls_ssl_check_record() detected unexpected record.\n");
                }
                break;

            default:
                mbedtls_printf("mbedtls_ssl_check_record() failed fatally with -%#04x.\n",
                               (unsigned int) -ret_cr1);
                my_ret = -1;
                goto cleanup;
        }

        /* Regardless of the outcome, forward the record to the stack. */
    }

cleanup:
    mbedtls_free(tmp_buf);

    return my_ret;
}

static int recv_cb(void *ctx, unsigned char *buf, size_t len)
{
    io_ctx_t *io_ctx = (io_ctx_t *) ctx;
    size_t recv_len;
    int ret;

    if (opt.nbio == 2) {
        ret = delayed_recv(io_ctx->net, buf, len);
    } else {
        ret = mbedtls_net_recv(io_ctx->net, buf, len);
    }
    if (ret < 0) {
        return ret;
    }
    recv_len = (size_t) ret;

    if (opt.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
        /* Here's the place to do any datagram/record checking
         * in between receiving the packet from the underlying
         * transport and passing it on to the TLS stack. */
        if (ssl_check_record(io_ctx->ssl, buf, recv_len) != 0) {
            return -1;
        }
    }

    return (int) recv_len;
}

static int recv_timeout_cb(void *ctx, unsigned char *buf, size_t len,
                           uint32_t timeout)
{
    io_ctx_t *io_ctx = (io_ctx_t *) ctx;
    int ret;
    size_t recv_len;

    ret = mbedtls_net_recv_timeout(io_ctx->net, buf, len, timeout);
    if (ret < 0) {
        return ret;
    }
    recv_len = (size_t) ret;

    if (opt.transport == MBEDTLS_SSL_TRANSPORT_DATAGRAM) {
        /* Here's the place to do any datagram/record checking
         * in between receiving the packet from the underlying
         * transport and passing it on to the TLS stack. */
        if (ssl_check_record(io_ctx->ssl, buf, recv_len) != 0) {
            return -1;
        }
    }

    return (int) recv_len;
}

static int send_cb(void *ctx, unsigned char const *buf, size_t len)
{
    io_ctx_t *io_ctx = (io_ctx_t *) ctx;

    if (opt.nbio == 2) {
        return delayed_send(io_ctx->net, buf, len);
    }

    return mbedtls_net_send(io_ctx->net, buf, len);
}

#if defined(MBEDTLS_X509_CRT_PARSE_C)
#if defined(PSA_HAVE_ALG_SOME_ECDSA) && defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
/*
 *   When GnuTLS/Openssl server is configured in TLS 1.2 mode with a certificate
 *   declaring an RSA public key and Mbed TLS is configured in hybrid mode, if
 *   `rsa_pss_rsae_*` algorithms are before `rsa_pkcs1_*` ones in this list then
 *   the GnuTLS/Openssl server chooses an `rsa_pss_rsae_*` signature algorithm
 *   for its signature in the key exchange message. As Mbed TLS 1.2 does not
 *   support them, the handshake fails.
 */
#define MBEDTLS_SSL_SIG_ALG(hash) ((hash << 8) | MBEDTLS_SSL_SIG_ECDSA), \
    ((hash << 8) | MBEDTLS_SSL_SIG_RSA), \
    (0x800 | hash),
#else
#define MBEDTLS_SSL_SIG_ALG(hash) ((hash << 8) | MBEDTLS_SSL_SIG_ECDSA), \
    ((hash << 8) | MBEDTLS_SSL_SIG_RSA),
#endif
#elif defined(PSA_HAVE_ALG_SOME_ECDSA)
#define MBEDTLS_SSL_SIG_ALG(hash) ((hash << 8) | MBEDTLS_SSL_SIG_ECDSA),
#elif defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_SSL_PROTO_TLS1_3)
/* See above */
#define MBEDTLS_SSL_SIG_ALG(hash) ((hash << 8) | MBEDTLS_SSL_SIG_RSA), \
    (0x800 | hash),
#else
#define MBEDTLS_SSL_SIG_ALG(hash) ((hash << 8) | MBEDTLS_SSL_SIG_RSA),
#endif
#else
#define MBEDTLS_SSL_SIG_ALG(hash)
#endif

uint16_t ssl_sig_algs_for_test[] = {
#if defined(PSA_WANT_ALG_SHA_512)
    MBEDTLS_SSL_SIG_ALG(MBEDTLS_SSL_HASH_SHA512)
#endif
#if defined(PSA_WANT_ALG_SHA_384)
    MBEDTLS_SSL_SIG_ALG(MBEDTLS_SSL_HASH_SHA384)
#endif
#if defined(PSA_WANT_ALG_SHA_256)
    MBEDTLS_SSL_SIG_ALG(MBEDTLS_SSL_HASH_SHA256)
#endif
#if defined(PSA_WANT_ALG_SHA_224)
    MBEDTLS_SSL_SIG_ALG(MBEDTLS_SSL_HASH_SHA224)
#endif
#if defined(MBEDTLS_RSA_C) && defined(PSA_WANT_ALG_SHA_256)
    MBEDTLS_TLS1_3_SIG_RSA_PSS_RSAE_SHA256,
#endif /* MBEDTLS_RSA_C && PSA_WANT_ALG_SHA_256 */
#if defined(PSA_WANT_ALG_SHA_1)
    /* Allow SHA-1 as we use it extensively in tests. */
    MBEDTLS_SSL_SIG_ALG(MBEDTLS_SSL_HASH_SHA1)
#endif
    MBEDTLS_TLS1_3_SIG_NONE
};
#endif /* MBEDTLS_X509_CRT_PARSE_C */

#if defined(MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED)
/** Functionally equivalent to mbedtls_x509_crt_verify_info, see that function
 *  for more info.
 */
static int x509_crt_verify_info(char *buf, size_t size, const char *prefix,
                                uint32_t flags)
{
#if !defined(MBEDTLS_X509_REMOVE_INFO)
    return mbedtls_x509_crt_verify_info(buf, size, prefix, flags);

#else /* !MBEDTLS_X509_REMOVE_INFO */
    int ret;
    char *p = buf;
    size_t n = size;

#define X509_CRT_ERROR_INFO(err, err_str, info)                      \
    if ((flags & err) != 0)                                         \
    {                                                                  \
        ret = mbedtls_snprintf(p, n, "%s%s\n", prefix, info);        \
        MBEDTLS_X509_SAFE_SNPRINTF;                                    \
        flags ^= err;                                                  \
    }

    MBEDTLS_X509_CRT_ERROR_INFO_LIST
#undef X509_CRT_ERROR_INFO

    if (flags != 0) {
        ret = mbedtls_snprintf(p, n, "%sUnknown reason "
                                     "(this should not happen)\n", prefix);
        MBEDTLS_X509_SAFE_SNPRINTF;
    }

    return (int) (size - n);
#endif /* MBEDTLS_X509_REMOVE_INFO */
}

static void mbedtls_print_supported_sig_algs(void)
{
    mbedtls_printf("supported signature algorithms:\n");
    mbedtls_printf("\trsa_pkcs1_sha256 ");
    mbedtls_printf("rsa_pkcs1_sha384 ");
    mbedtls_printf("rsa_pkcs1_sha512\n");
    mbedtls_printf("\tecdsa_secp256r1_sha256 ");
    mbedtls_printf("ecdsa_secp384r1_sha384 ");
    mbedtls_printf("ecdsa_secp521r1_sha512\n");
    mbedtls_printf("\trsa_pss_rsae_sha256 ");
    mbedtls_printf("rsa_pss_rsae_sha384 ");
    mbedtls_printf("rsa_pss_rsae_sha512\n");
    mbedtls_printf("\trsa_pss_pss_sha256 ");
    mbedtls_printf("rsa_pss_pss_sha384 ");
    mbedtls_printf("rsa_pss_pss_sha512\n");
    mbedtls_printf("\ted25519 ");
    mbedtls_printf("ed448 ");
    mbedtls_printf("rsa_pkcs1_sha1 ");
    mbedtls_printf("ecdsa_sha1\n");
    mbedtls_printf("\n");
}
#endif /* MBEDTLS_SSL_HANDSHAKE_WITH_CERT_ENABLED */
