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

int psa_hash_main(void)
{
    psa_status_t status;
    uint8_t hash[PSA_HASH_LENGTH(HASH_ALG)];
    size_t hash_length;
    psa_hash_operation_t hash_operation = PSA_HASH_OPERATION_INIT;
    psa_hash_operation_t cloned_hash_operation = PSA_HASH_OPERATION_INIT;

    mbedtls_printf("PSA Crypto API: SHA-256 example\n\n");

    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        mbedtls_printf("psa_crypto_init failed\n");
        return EXIT_FAILURE;
    }

    /* Compute hash using multi-part operation */
    status = psa_hash_setup(&hash_operation, HASH_ALG);
    if (status == PSA_ERROR_NOT_SUPPORTED) {
        mbedtls_printf("unknown hash algorithm supplied\n");
        return EXIT_FAILURE;
    } else if (status != PSA_SUCCESS) {
        mbedtls_printf("psa_hash_setup failed\n");
        return EXIT_FAILURE;
    }

    status = psa_hash_update(&hash_operation, sample_message, sample_message_length);
    if (status != PSA_SUCCESS) {
        mbedtls_printf("psa_hash_update failed\n");
        goto cleanup;
    }

    status = psa_hash_clone(&hash_operation, &cloned_hash_operation);
    if (status != PSA_SUCCESS) {
        mbedtls_printf("PSA hash clone failed\n");
        goto cleanup;
    }

    status = psa_hash_finish(&hash_operation, hash, sizeof(hash), &hash_length);
    if (status != PSA_SUCCESS) {
        mbedtls_printf("psa_hash_finish failed\n");
        goto cleanup;
    }

    /* Check the result of the operation against the sample */
    if (hash_length != expected_hash_len ||
        (memcmp(hash, expected_hash, expected_hash_len) != 0)) {
        mbedtls_printf("Multi-part hash operation gave the wrong result!\n\n");
        goto cleanup;
    }

    status =
        psa_hash_verify(&cloned_hash_operation, expected_hash,
                        expected_hash_len);
    if (status != PSA_SUCCESS) {
        mbedtls_printf("psa_hash_verify failed\n");
        goto cleanup;
    } else {
        mbedtls_printf("Multi-part hash operation successful!\n");
    }

    /* A bit of white-box testing: ensure that we can abort an operation more
     * times than there are operation slots on the simulator server.
     */
    for (int i = 0; i < 200; i++) {
        /* This should be a no-op */
        status = psa_hash_abort(&hash_operation);
        if (status != PSA_SUCCESS) {
            mbedtls_printf("psa_hash_abort failed\n");
            goto cleanup;
        }
    }

    /* Compute hash using multi-part operation using the same operation struct */
    status = psa_hash_setup(&hash_operation, HASH_ALG);
    if (status == PSA_ERROR_NOT_SUPPORTED) {
        mbedtls_printf("unknown hash algorithm supplied\n");
        goto cleanup;
    } else if (status != PSA_SUCCESS) {
        mbedtls_printf("psa_hash_setup failed: %d\n", status);
        goto cleanup;
    }

    status = psa_hash_update(&hash_operation, sample_message, sample_message_length);
    if (status != PSA_SUCCESS) {
        mbedtls_printf("psa_hash_update failed\n");
        goto cleanup;
    }

    /* Don't use psa_hash_finish() when going to check against an expected result */
    status = psa_hash_verify(&hash_operation, expected_hash, expected_hash_len);
    if (status != PSA_SUCCESS) {
        mbedtls_printf("psa_hash_verify failed: %d\n", status);
        goto cleanup;
    } else {
        mbedtls_printf("Second multi-part hash operation successful!\n");
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
    psa_hash_abort(&hash_operation);
    psa_hash_abort(&cloned_hash_operation);
    return EXIT_FAILURE;
}
