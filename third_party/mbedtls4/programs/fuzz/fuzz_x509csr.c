#include <stdint.h>
#include "mbedtls/x509_csr.h"
#include "fuzz_common.h"

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
#ifdef MBEDTLS_X509_CSR_PARSE_C
    int ret;
    mbedtls_x509_csr csr;
    unsigned char buf[4096];

    mbedtls_x509_csr_init(&csr);
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        goto exit;
    }
    ret = mbedtls_x509_csr_parse(&csr, Data, Size);
#if !defined(MBEDTLS_X509_REMOVE_INFO)
    if (ret == 0) {
        ret = mbedtls_x509_csr_info((char *) buf, sizeof(buf) - 1, " ", &csr);
    }
#else /* !MBEDTLS_X509_REMOVE_INFO */
    ((void) ret);
    ((void) buf);
#endif /* !MBEDTLS_X509_REMOVE_INFO */

exit:
    mbedtls_psa_crypto_free();
    mbedtls_x509_csr_free(&csr);
#else /* MBEDTLS_X509_CSR_PARSE_C */
    (void) Data;
    (void) Size;
#endif /* MBEDTLS_X509_CSR_PARSE_C */

    return 0;
}
