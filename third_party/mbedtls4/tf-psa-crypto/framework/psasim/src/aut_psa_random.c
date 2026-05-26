/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "mbedtls/build_info.h"

#include <psa/crypto.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "mbedtls/private/entropy.h"

#define BUFFER_SIZE 100

int psa_random_main(void)
{
    psa_status_t status;
    uint8_t output[BUFFER_SIZE] = { 0 };

    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        printf("psa_crypto_init failed\n");
        return EXIT_FAILURE;
    }

    status = psa_generate_random(output, BUFFER_SIZE);
    if (status != PSA_SUCCESS) {
        printf("psa_generate_random failed\n");
        return EXIT_FAILURE;
    }

    printf("Random bytes generated:\n");

    for (size_t j = 0; j < BUFFER_SIZE; j++) {
        if (j % 8 == 0) {
            printf("\n    ");
        }
        printf("%02x ", output[j]);
    }

    printf("\n");

    mbedtls_psa_crypto_free();
    return 0;
}
