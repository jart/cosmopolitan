#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "fuzz_common.h"
#include "mbedtls/ssl.h"
#if defined(MBEDTLS_SSL_PROTO_DTLS)
#include "mbedtls/timing.h"
#include "test/certs.h"

#if defined(MBEDTLS_SSL_CLI_C) && \
    defined(MBEDTLS_TIMING_C)
static int initialized = 0;
#if defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_PEM_PARSE_C)
static mbedtls_x509_crt cacert;
#endif

const char *pers = "fuzz_dtlsclient";
#endif
#endif // MBEDTLS_SSL_PROTO_DTLS



int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
#if defined(MBEDTLS_SSL_PROTO_DTLS) && \
    defined(MBEDTLS_SSL_CLI_C) && \
    defined(MBEDTLS_TIMING_C)
    int ret;
    size_t len;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_timing_delay_context timer;
    unsigned char buf[4096];
    fuzzBufferOffset_t biomemfuzz;

    if (initialized == 0) {
#if defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_PEM_PARSE_C)
        mbedtls_x509_crt_init(&cacert);
        if (mbedtls_x509_crt_parse(&cacert, (const unsigned char *) mbedtls_test_cas_pem,
                                   mbedtls_test_cas_pem_len) != 0) {
            return 1;
        }
#endif
        dummy_init();

        initialized = 1;
    }

    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);

    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    if (mbedtls_ssl_config_defaults(&conf,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT) != 0) {
        goto exit;
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_PEM_PARSE_C)
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
#endif
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);

    if (mbedtls_ssl_setup(&ssl, &conf) != 0) {
        goto exit;
    }

    mbedtls_ssl_set_timer_cb(&ssl, &timer, mbedtls_timing_set_delay,
                             mbedtls_timing_get_delay);

#if defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_PEM_PARSE_C)
    if (mbedtls_ssl_set_hostname(&ssl, "localhost") != 0) {
        goto exit;
    }
#endif

    biomemfuzz.Data = Data;
    biomemfuzz.Size = Size;
    biomemfuzz.Offset = 0;
    mbedtls_ssl_set_bio(&ssl, &biomemfuzz, dummy_send, fuzz_recv, fuzz_recv_timeout);

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

exit:
    mbedtls_ssl_config_free(&conf);
    mbedtls_ssl_free(&ssl);
    mbedtls_psa_crypto_free();

#else
    (void) Data;
    (void) Size;
#endif
    return 0;
}
