#include <stdint.h>
#include "mbedtls/x509_crt.h"
#include "fuzz_common.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
#ifdef MBEDTLS_X509_CRT_PARSE_C
    int ret;
    mbedtls_x509_crt crt;
    unsigned char buf[4096];

    mbedtls_x509_crt_init(&crt);
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        goto exit;
    }
    ret = mbedtls_x509_crt_parse(&crt, Data, Size);
#if !defined(MBEDTLS_X509_REMOVE_INFO)
    if (ret == 0) {
        ret = mbedtls_x509_crt_info((char *) buf, sizeof(buf) - 1, " ", &crt);
    }
#else
    ((void) ret);
    ((void) buf);
#endif /* !MBEDTLS_X509_REMOVE_INFO */

exit:
    mbedtls_psa_crypto_free();
    mbedtls_x509_crt_free(&crt);
#else /* MBEDTLS_X509_CRT_PARSE_C */
    (void) Data;
    (void) Size;
#endif /* MBEDTLS_X509_CRT_PARSE_C */

    return 0;
}
