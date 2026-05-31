#define MBEDTLS_DECLARE_PRIVATE_IDENTIFIERS

#include <stdint.h>
#include <stdlib.h>
#include "mbedtls/pk.h"
#include "fuzz_common.h"
#include "mbedtls/private/pk_private.h"

#if defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_PK_WRITE_C)

#define MAX_LEN 0x1000
static uint8_t out_buf[MAX_LEN];

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    int ret;
    mbedtls_pk_context pk;

    mbedtls_pk_init(&pk);
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        goto exit;
    }
    ret = mbedtls_pk_parse_public_key(&pk, Data, Size);
    if (ret == 0) {
        ret = mbedtls_pk_write_pubkey_der(&pk, out_buf, Size);
        if (ret <= 0) {
            abort();
        }
    }
exit:
    mbedtls_psa_crypto_free();
    mbedtls_pk_free(&pk);

    return 0;
}

#else /* MBEDTLS_PK_PARSE_C && MBEDTLS_PK_WRITE_C */

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    (void) Data;
    (void) Size;
    return 0;
}

#endif /* MBEDTLS_PK_PARSE_C && MBEDTLS_PK_WRITE_C */
