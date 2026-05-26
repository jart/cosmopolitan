#include "mbedtls/ssl.h"
#include "test/certs.h"
#include "fuzz_common.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


#if defined(MBEDTLS_SSL_CLI_C)
static int initialized = 0;
#if defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_PEM_PARSE_C)
static mbedtls_x509_crt cacert;
#endif
const char *alpn_list[3];


#if defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
const unsigned char psk[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f
};
const char psk_id[] = "Client_identity";
#endif

const char *pers = "fuzz_client";
#endif /* MBEDTLS_SSL_CLI_C */


int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
#if defined(MBEDTLS_SSL_CLI_C)
    int ret;
    size_t len;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    unsigned char buf[4096];
    fuzzBufferOffset_t biomemfuzz;
    uint16_t options;

    if (initialized == 0) {
#if defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_PEM_PARSE_C)
        mbedtls_x509_crt_init(&cacert);
        if (mbedtls_x509_crt_parse(&cacert, (const unsigned char *) mbedtls_test_cas_pem,
                                   mbedtls_test_cas_pem_len) != 0) {
            return 1;
        }
#endif

        alpn_list[0] = "HTTP";
        alpn_list[1] = "fuzzalpn";
        alpn_list[2] = NULL;

        dummy_init();

        initialized = 1;
    }

    //we take 1 byte as options input
    if (Size < 2) {
        return 0;
    }
    options = (Data[Size - 2] << 8) | Data[Size - 1];
    //Avoid warnings if compile options imply no options
    (void) options;

    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);

    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        goto exit;
    }

    if (mbedtls_ssl_config_defaults(&conf,
                                    MBEDTLS_SSL_IS_CLIENT,
                                    MBEDTLS_SSL_TRANSPORT_STREAM,
                                    MBEDTLS_SSL_PRESET_DEFAULT) != 0) {
        goto exit;
    }

#if defined(MBEDTLS_KEY_EXCHANGE_SOME_PSK_ENABLED)
    if (options & 2) {
        mbedtls_ssl_conf_psk(&conf, psk, sizeof(psk),
                             (const unsigned char *) psk_id, sizeof(psk_id) - 1);
    }
#endif

#if defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_PEM_PARSE_C)
    if (options & 4) {
        mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
        mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    } else
#endif
    {
        mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);
    }
#if defined(MBEDTLS_SSL_EXTENDED_MASTER_SECRET)
    mbedtls_ssl_conf_extended_master_secret(&conf,
                                            (options &
                                             0x10) ? MBEDTLS_SSL_EXTENDED_MS_DISABLED : MBEDTLS_SSL_EXTENDED_MS_ENABLED);
#endif
#if defined(MBEDTLS_SSL_ENCRYPT_THEN_MAC)
    mbedtls_ssl_conf_encrypt_then_mac(&conf,
                                      (options &
                                       0x20) ? MBEDTLS_SSL_ETM_DISABLED : MBEDTLS_SSL_ETM_ENABLED);
#endif
#if defined(MBEDTLS_SSL_RENEGOTIATION)
    mbedtls_ssl_conf_renegotiation(&conf,
                                   (options &
                                    0x80) ? MBEDTLS_SSL_RENEGOTIATION_ENABLED : MBEDTLS_SSL_RENEGOTIATION_DISABLED);
#endif
#if defined(MBEDTLS_SSL_SESSION_TICKETS)
    mbedtls_ssl_conf_session_tickets(&conf,
                                     (options &
                                      0x100) ? MBEDTLS_SSL_SESSION_TICKETS_DISABLED : MBEDTLS_SSL_SESSION_TICKETS_ENABLED);
#endif
#if defined(MBEDTLS_SSL_ALPN)
    if (options & 0x200) {
        mbedtls_ssl_conf_alpn_protocols(&conf, alpn_list);
    }
#endif
    //There may be other options to add :
    // mbedtls_ssl_conf_cert_profile

    if (mbedtls_ssl_setup(&ssl, &conf) != 0) {
        goto exit;
    }

#if defined(MBEDTLS_X509_CRT_PARSE_C) && defined(MBEDTLS_PEM_PARSE_C)
    if ((options & 1) == 0) {
        if (mbedtls_ssl_set_hostname(&ssl, "localhost") != 0) {
            goto exit;
        }
    }
#endif

    biomemfuzz.Data = Data;
    biomemfuzz.Size = Size-2;
    biomemfuzz.Offset = 0;
    mbedtls_ssl_set_bio(&ssl, &biomemfuzz, dummy_send, fuzz_recv, NULL);

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
#endif /* MBEDTLS_SSL_CLI_C */

    return 0;
}
