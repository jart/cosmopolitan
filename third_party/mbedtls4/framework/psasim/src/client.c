/* psasim test client */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

/* Includes from mbedtls */
#include "psa/crypto.h"
#include "util.h"

int main()
{
    /* psa_crypto_init() connects to the server */
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        ERROR("psa_crypto_init returned %d", status);
        return 1;
    }

    mbedtls_psa_crypto_free();
    return 0;
}
