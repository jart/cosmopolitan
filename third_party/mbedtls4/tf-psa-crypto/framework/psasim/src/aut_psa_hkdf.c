/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "psa/crypto.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mbedtls/build_info.h"

int psa_hkdf_main(void)
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    psa_key_derivation_operation_t operation = PSA_KEY_DERIVATION_OPERATION_INIT;

    /* Example test vector from RFC 5869 */

    /* Input keying material (IKM) */
    unsigned char ikm[] = { 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
                            0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b };

    unsigned char salt[] =
    { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c };

    /* Context and application specific information, which can be of zero length */
    unsigned char info[] = { 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9 };

    /* Expected OKM based on the RFC 5869-provided test vector */
    unsigned char expected_okm[] = { 0x3c, 0xb2, 0x5f, 0x25, 0xfa, 0xac, 0xd5, 0x7a, 0x90, 0x43,
                                     0x4f, 0x64, 0xd0, 0x36, 0x2f, 0x2a, 0x2d, 0x2d, 0x0a, 0x90,
                                     0xcf, 0x1a, 0x5a, 0x4c, 0x5d, 0xb0, 0x2d, 0x56, 0xec, 0xc4,
                                     0xc5, 0xbf, 0x34, 0x00, 0x72, 0x08, 0xd5, 0xb8, 0x87, 0x18,
                                     0x58, 0x65 };

    /* The output size of the HKDF function depends on the hash function used.
     * In our case we use SHA-256, which produces a 32 byte fingerprint.
     * Therefore, we allocate a buffer of 32 bytes to hold the output keying
     * material (OKM).
     */
    unsigned char output[32];

    psa_algorithm_t alg = PSA_ALG_HKDF(PSA_ALG_SHA_256);

    printf("PSA Crypto API: HKDF SHA-256 example\n\n");

    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        printf("psa_crypto_init failed\n");
        return EXIT_FAILURE;
    }

    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_DERIVE);
    psa_set_key_algorithm(&attributes, PSA_ALG_HKDF(PSA_ALG_SHA_256));
    psa_set_key_type(&attributes, PSA_KEY_TYPE_DERIVE);

    status = psa_import_key(&attributes, ikm, sizeof(ikm), &key_id);
    if (status != PSA_SUCCESS) {
        printf("psa_import_key failed\n");
        return EXIT_FAILURE;
    }

    status = psa_key_derivation_setup(&operation, alg);
    if (status != PSA_SUCCESS) {
        printf("psa_key_derivation_setup failed");
        return EXIT_FAILURE;
    }

    status = psa_key_derivation_input_bytes(&operation, PSA_KEY_DERIVATION_INPUT_SALT,
                                            salt, sizeof(salt));
    if (status != PSA_SUCCESS) {
        printf("psa_key_derivation_input_bytes (salt) failed");
        return EXIT_FAILURE;
    }

    status = psa_key_derivation_input_key(&operation, PSA_KEY_DERIVATION_INPUT_SECRET,
                                          key_id);
    if (status != PSA_SUCCESS) {
        printf("psa_key_derivation_input_key failed");
        return EXIT_FAILURE;
    }

    status = psa_key_derivation_input_bytes(&operation, PSA_KEY_DERIVATION_INPUT_INFO,
                                            info, sizeof(info));
    if (status != PSA_SUCCESS) {
        printf("psa_key_derivation_input_bytes (info) failed");
        return EXIT_FAILURE;
    }

    status = psa_key_derivation_output_bytes(&operation, output, sizeof(output));
    if (status != PSA_SUCCESS) {
        printf("psa_key_derivation_output_bytes failed");
        return EXIT_FAILURE;
    }

    status = psa_key_derivation_abort(&operation);
    if (status != PSA_SUCCESS) {
        printf("psa_key_derivation_abort failed");
        return EXIT_FAILURE;
    }

    printf("OKM: \n");

    for (size_t j = 0; j < sizeof(output); j++) {
        if (output[j] != expected_okm[j]) {
            printf("\n --- Unexpected outcome!\n");
            return EXIT_FAILURE;
        }

        if (j % 8 == 0) {
            printf("\n    ");
        }
        printf("%02x ", output[j]);
    }

    printf("\n");
    mbedtls_psa_crypto_free();
    return EXIT_SUCCESS;
}
