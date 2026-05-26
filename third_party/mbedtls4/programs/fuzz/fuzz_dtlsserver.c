#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "fuzz_common.h"
#include "mbedtls/ssl.h"
#include "test/certs.h"
#if defined(MBEDTLS_SSL_PROTO_DTLS)
#include "mbedtls/timing.h"
#include "mbedtls/ssl_cookie.h"

#if defined(MBEDTLS_SSL_SRV_C) && \
    defined(MBEDTLS_TIMING_C) && \
    (defined(PSA_WANT_ALG_SHA_384) || \
    defined(PSA_WANT_ALG_SHA_256))
const char *pers = "fuzz_dtlsserver";
const unsigned char client_ip[4] = { 0x7F, 0, 0, 1 };
static int initialized = 0;
#if defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_PEM_PARSE_C)
static mbedtls_x509_crt srvcert;
static mbedtls_pk_context pkey;
#endif
#endif
#endif // MBEDTLS_SSL_PROTO_DTLS

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
#if defined(MBEDTLS_SSL_PROTO_DTLS) && \
    defined(MBEDTLS_SSL_SRV_C) && \
    defined(MBEDTLS_TIMING_C) && \
    (defined(PSA_WANT_ALG_SHA_384) || \
    defined(PSA_WANT_ALG_SHA_256))
    int ret;
    size_t len;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_timing_delay_context timer;
    mbedtls_ssl_cookie_ctx cookie_ctx;
    unsigned char buf[4096];
    fuzzBufferOffset_t biomemfuzz;

#if defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_PEM_PARSE_C)
    mbedtls_x509_crt_init(&srvcert);
    mbedtls_pk_init(&pkey);
#endif
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_ssl_cookie_init(&cookie_ctx);

    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    if (initialized == 0) {
#if defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_PEM_PARSE_C)

        if (mbedtls_x509_crt_parse(&srvcert, (const unsigned char *) mbedtls_test_srv_crt,
                                   mbedtls_test_srv_crt_len) != 0) {
            return 1;
        }
        if (mbedtls_x509_crt_parse(&srvcert, (const unsigned char *) mbedtls_test_cas_pem,
                                   mbedtls_test_cas_pem_len) != 0) {
            return 1;
        }
        if (mbedtls_pk_parse_key(&pkey, (const unsigned char *) mbedtls_test_srv_key,
                                 mbedtls_test_srv_key_len, NULL, 0) != 0) {
            return 1;
        }
#endif
        dummy_init();

        initialized = 1;
    }

    if (mbedtls_ssl_config_defaults(&conf,
                                    MBEDTLS_SSL_IS_SERVER,
                                    MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT) != 0) {
        goto exit;
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_PEM_PARSE_C)
    mbedtls_ssl_conf_ca_chain(&conf, srvcert.next, NULL);
    if (mbedtls_ssl_conf_own_cert(&conf, &srvcert, &pkey) != 0) {
        goto exit;
    }
#endif

    if (mbedtls_ssl_cookie_setup(&cookie_ctx) != 0) {
        goto exit;
    }

    mbedtls_ssl_conf_dtls_cookies(&conf,
                                  mbedtls_ssl_cookie_write,
                                  mbedtls_ssl_cookie_check,
                                  &cookie_ctx);

    if (mbedtls_ssl_setup(&ssl, &conf) != 0) {
        goto exit;
    }

    mbedtls_ssl_set_timer_cb(&ssl, &timer, mbedtls_timing_set_delay,
                             mbedtls_timing_get_delay);

    biomemfuzz.Data = Data;
    biomemfuzz.Size = Size;
    biomemfuzz.Offset = 0;
    mbedtls_ssl_set_bio(&ssl, &biomemfuzz, dummy_send, fuzz_recv, fuzz_recv_timeout);
    if (mbedtls_ssl_set_client_transport_id(&ssl, client_ip, sizeof(client_ip)) != 0) {
        goto exit;
    }

    ret = mbedtls_ssl_handshake(&ssl);

    if (ret == MBEDTLS_ERR_SSL_HELLO_VERIFY_REQUIRED) {
        biomemfuzz.Offset = ssl.MBEDTLS_PRIVATE(next_record_offset);
        mbedtls_ssl_session_reset(&ssl);
        mbedtls_ssl_set_bio(&ssl, &biomemfuzz, dummy_send, fuzz_recv, fuzz_recv_timeout);
        if (mbedtls_ssl_set_client_transport_id(&ssl, client_ip, sizeof(client_ip)) != 0) {
            goto exit;
        }

        ret = mbedtls_ssl_handshake(&ssl);

        if (ret == 0) {
            //keep reading data from server until the end
            do {
                len = sizeof(buf) - 1;
                ret = mbedtls_ssl_read(&ssl, buf, len);
                if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
                    continue;
                } else if (ret <= 0) {
                    //EOF or error
                    break;
                }
            } while (1);
        }
    }

exit:
    mbedtls_ssl_cookie_free(&cookie_ctx);
#if defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_PEM_PARSE_C)
    mbedtls_pk_free(&pkey);
    mbedtls_x509_crt_free(&srvcert);
#endif
    mbedtls_ssl_config_free(&conf);
    mbedtls_ssl_free(&ssl);
    mbedtls_psa_crypto_free();

#else
    (void) Data;
    (void) Size;
#endif
    return 0;
}
