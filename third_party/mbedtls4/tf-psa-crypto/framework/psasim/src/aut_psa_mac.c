/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "psa/crypto.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "mbedtls/build_info.h"

/* constant-time buffer comparison */
static inline int safer_memcmp(const void *a, const void *b, size_t n)
{
    size_t i;
    volatile const unsigned char *A = (volatile const unsigned char *) a;
    volatile const unsigned char *B = (volatile const unsigned char *) b;
    volatile unsigned char diff = 0;

    for (i = 0; i < n; i++) {
        /* Read volatile data in order before computing diff.
         * This avoids IAR compiler warning:
         * 'the order of volatile accesses is undefined ..' */
        unsigned char x = A[i], y = B[i];
        diff |= x ^ y;
    }

    return diff;
}


int psa_mac_main(void)
{
    uint8_t input[] = "Hello World!";
    psa_status_t status;
    size_t mac_size_real = 0;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    uint8_t mac[PSA_MAC_MAX_SIZE];
    psa_mac_operation_t operation = PSA_MAC_OPERATION_INIT;
    const uint8_t key_bytes[16] = "kkkkkkkkkkkkkkkk";
    const uint8_t mbedtls_test_hmac_sha256[] = {
        0xae, 0x72, 0x34, 0x5a, 0x10, 0x36, 0xfb, 0x71,
        0x35, 0x3c, 0x7d, 0x6c, 0x81, 0x98, 0x52, 0x86,
        0x00, 0x4a, 0x43, 0x7c, 0x2d, 0xb3, 0x1a, 0xd8,
        0x67, 0xb1, 0xad, 0x11, 0x4d, 0x18, 0x49, 0x8b
    };

    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        printf("psa_crypto_init failed\n");
        return EXIT_FAILURE;
    }

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_VERIFY_MESSAGE |
                            PSA_KEY_USAGE_SIGN_HASH |
                            PSA_KEY_USAGE_SIGN_MESSAGE);
    psa_set_key_algorithm(&attributes, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);

    status = psa_import_key(&attributes, key_bytes, sizeof(key_bytes), &key_id);
    if (status != PSA_SUCCESS) {
        printf("psa_import_key failed\n");
        return EXIT_FAILURE;
    }

    /* Single-part MAC operation with psa_mac_compute() */
    status = psa_mac_compute(key_id,
                             PSA_ALG_HMAC(PSA_ALG_SHA_256),
                             input,
                             sizeof(input),
                             mac,
                             sizeof(mac),
                             &mac_size_real);
    if (status != PSA_SUCCESS) {
        printf("psa_mac_compute failed\n");
        return EXIT_FAILURE;
    }

    printf("HMAC-SHA-256(%s) with psa_mac_compute():\n", input);

    for (size_t j = 0; j < mac_size_real; j++) {
        if (j % 8 == 0) {
            printf("\n    ");
        }
        printf("%02x ", mac[j]);
    }

    printf("\n");

    if (safer_memcmp(mac,
                     mbedtls_test_hmac_sha256,
                     mac_size_real
                     ) != 0) {
        printf("\nMAC verified incorrectly!\n");
    } else {
        printf("\nMAC verified correctly!\n");
    }

    psa_destroy_key(key_id);

    status = psa_import_key(&attributes, key_bytes, sizeof(key_bytes), &key_id);
    if (status != PSA_SUCCESS) {
        printf("psa_import_key failed\n");
        return EXIT_FAILURE;
    }

    /* Single-part MAC operation with psa_mac_verify() */
    status = psa_mac_verify(key_id,
                            PSA_ALG_HMAC(PSA_ALG_SHA_256),
                            input,
                            sizeof(input),
                            mbedtls_test_hmac_sha256,
                            sizeof(mbedtls_test_hmac_sha256));
    if (status != PSA_SUCCESS) {
        printf("psa_mac_verify failed\n");
        return EXIT_FAILURE;
    } else {
        printf("psa_mac_verify passed successfully\n");
    }

    psa_destroy_key(key_id);

    status = psa_import_key(&attributes, key_bytes, sizeof(key_bytes), &key_id);
    if (status != PSA_SUCCESS) {
        printf("psa_import_key failed\n");
        return EXIT_FAILURE;
    }

    /* Multi-part MAC operation */
    status = psa_mac_sign_setup(&operation, key_id, PSA_ALG_HMAC(PSA_ALG_SHA_256));
    if (status != PSA_SUCCESS) {
        printf("psa_mac_sign_setup failed\n");
        return EXIT_FAILURE;
    }

    status = psa_mac_update(&operation, input, sizeof(input));
    if (status != PSA_SUCCESS) {
        printf("psa_mac_update failed\n");
        return EXIT_FAILURE;
    }

    status = psa_mac_sign_finish(&operation, mac, sizeof(mac), &mac_size_real);
    if (status != PSA_SUCCESS) {
        printf("psa_mac_sign_finish failed\n");
        return EXIT_FAILURE;
    }

    if (safer_memcmp(mac,
                     mbedtls_test_hmac_sha256,
                     mac_size_real
                     ) != 0) {
        printf("MAC, calculated with multi-part MAC operation, verified incorrectly!\n");
    } else {
        printf("MAC, calculated with multi-part MAC operation, verified correctly!\n");
    }

    psa_destroy_key(key_id);
    mbedtls_psa_crypto_free();
    return EXIT_SUCCESS;
}
