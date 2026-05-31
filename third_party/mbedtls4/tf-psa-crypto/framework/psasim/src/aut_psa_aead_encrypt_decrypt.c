/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "psa/crypto.h"
/*
 * Temporary hack: psasim’s Makefile only does:
 *  -Itests/psa-client-server/psasim/include
 *  -I$(MBEDTLS_ROOT_PATH)/include
 *  -I$(MBEDTLS_ROOT_PATH)/tf-psa-crypto/include
 *  -I$(MBEDTLS_ROOT_PATH)/tf-psa-crypto/drivers/builtin/include
 * None of those cover tf-psa-crypto/core, so we rely on the
 * “-I$(MBEDTLS_ROOT_PATH)/include” entry plus a parent-relative
 * include "../tf-psa-crypto/core/tf_psa_crypto_common.h" in order to pull in tf_psa_crypto_common.h here,
 * which in turn gets MBEDTLS_ATTRIBUTE_UNTERMINATED_STRING (to silence the
 * new GCC-15 unterminated-string-initialization warning).
 * See GitHub issue #10223 for the proper long-term fix.
 * https://github.com/Mbed-TLS/mbedtls/issues/10223
 */
#include "../tf-psa-crypto/core/tf_psa_crypto_common.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 500

static void print_bytestr(const uint8_t *bytes, size_t len)
{
    for (unsigned int idx = 0; idx < len; idx++) {
        printf("%02X", bytes[idx]);
    }
}

int psa_aead_encrypt_decrypt_main(void)
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    uint8_t encrypt[BUFFER_SIZE] = { 0 };
    uint8_t decrypt[BUFFER_SIZE] = { 0 };
    const uint8_t plaintext[] = "Hello World!";
    /* We need to tell the compiler that we meant to leave out the null character. */
    const uint8_t key_bytes[32] MBEDTLS_ATTRIBUTE_UNTERMINATED_STRING =
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    uint8_t nonce[PSA_AEAD_NONCE_LENGTH(PSA_KEY_TYPE_AES, PSA_ALG_CCM)];
    size_t nonce_length = sizeof(nonce);
    size_t ciphertext_length;
    size_t plaintext_length;

    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        printf("psa_crypto_init failed\n");
        return EXIT_FAILURE;
    }

    psa_set_key_usage_flags(&attributes,
                            PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_CCM);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
    psa_set_key_bits(&attributes, 256);

    status = psa_import_key(&attributes, key_bytes, sizeof(key_bytes), &key_id);
    if (status != PSA_SUCCESS) {
        printf("psa_import_key failed\n");
        return EXIT_FAILURE;
    }

    status = psa_generate_random(nonce, nonce_length);
    if (status != PSA_SUCCESS) {
        printf("psa_generate_random failed\n");
        return EXIT_FAILURE;
    }

    status = psa_aead_encrypt(key_id,                            // key
                              PSA_ALG_CCM,                       // algorithm
                              nonce, nonce_length,               // nonce
                              NULL, 0,                           // additional data
                              plaintext, sizeof(plaintext),      // plaintext
                              encrypt, sizeof(encrypt),          // ciphertext
                              &ciphertext_length);               // length of output
    if (status != PSA_SUCCESS) {
        printf("psa_aead_encrypt failed\n");
        return EXIT_FAILURE;
    }

    printf("AES-CCM encryption:\n");
    printf("- Plaintext: '%s':\n", plaintext);
    printf("- Key: ");
    print_bytestr(key_bytes, sizeof(key_bytes));
    printf("\n- Nonce: ");
    print_bytestr(nonce, nonce_length);
    printf("\n- No additional data\n");
    printf("- Ciphertext:\n");

    for (size_t j = 0; j < ciphertext_length; j++) {
        if (j % 8 == 0) {
            printf("\n    ");
        }
        printf("%02x ", encrypt[j]);
    }

    printf("\n");

    status = psa_aead_decrypt(key_id,                       // key
                              PSA_ALG_CCM,                  // algorithm
                              nonce, nonce_length,          // nonce
                              NULL, 0,                      // additional data
                              encrypt, ciphertext_length,   // ciphertext
                              decrypt, sizeof(decrypt),     // plaintext
                              &plaintext_length);           // length of output
    if (status != PSA_SUCCESS) {
        printf("psa_aead_decrypt failed\n");
        return EXIT_FAILURE;
    }

    if (memcmp(plaintext, decrypt, sizeof(plaintext)) != 0) {
        printf("\nEncryption/Decryption failed!\n");
    } else {
        printf("\nEncryption/Decryption successful!\n");
    }

    psa_destroy_key(key_id);
    mbedtls_psa_crypto_free();
    return 0;
}
