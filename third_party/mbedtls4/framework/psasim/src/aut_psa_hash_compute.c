/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "psa/crypto.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "mbedtls/build_info.h"
#include "mbedtls/platform.h"

#define HASH_ALG PSA_ALG_SHA_256

static const uint8_t sample_message[] = "Hello World!";
/* sample_message is terminated with a null byte which is not part of
 * the message itself so we make sure to subtract it in order to get
 * the message length. */
static const size_t sample_message_length = sizeof(sample_message) - 1;

#define EXPECTED_HASH_VALUE {                                                    \
        0x7f, 0x83, 0xb1, 0x65, 0x7f, 0xf1, 0xfc, 0x53, 0xb9, 0x2d, 0xc1, 0x81, \
        0x48, 0xa1, 0xd6, 0x5d, 0xfc, 0x2d, 0x4b, 0x1f, 0xa3, 0xd6, 0x77, 0x28, \
        0x4a, 0xdd, 0xd2, 0x00, 0x12, 0x6d, 0x90, 0x69 \
}

static const uint8_t expected_hash[] = EXPECTED_HASH_VALUE;
static const size_t expected_hash_len = sizeof(expected_hash);

int psa_hash_compute_main(void)
{
    psa_status_t status;
    uint8_t hash[PSA_HASH_LENGTH(HASH_ALG)];
    size_t hash_length;

    mbedtls_printf("PSA Crypto API: SHA-256 example\n\n");

    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        mbedtls_printf("psa_crypto_init failed\n");
        return EXIT_FAILURE;
    }

    /* Clear local variables prior to one-shot hash demo */
    memset(hash, 0, sizeof(hash));
    hash_length = 0;

    /* Compute hash using one-shot function call */
    status = psa_hash_compute(HASH_ALG,
                              sample_message, sample_message_length,
                              hash, sizeof(hash),
                              &hash_length);
    if (status != PSA_SUCCESS) {
        mbedtls_printf("psa_hash_compute failed\n");
        goto cleanup;
    }

    if (hash_length != expected_hash_len ||
        (memcmp(hash, expected_hash, expected_hash_len) != 0)) {
        mbedtls_printf("One-shot hash operation gave the wrong result!\n\n");
        goto cleanup;
    }

    mbedtls_printf("One-shot hash operation successful!\n\n");

    /* Print out result */
    mbedtls_printf("The SHA-256( '%s' ) is: ", sample_message);

    for (size_t j = 0; j < expected_hash_len; j++) {
        mbedtls_printf("%02x", hash[j]);
    }

    mbedtls_printf("\n");

    mbedtls_psa_crypto_free();
    return EXIT_SUCCESS;

cleanup:
    return EXIT_FAILURE;
}
