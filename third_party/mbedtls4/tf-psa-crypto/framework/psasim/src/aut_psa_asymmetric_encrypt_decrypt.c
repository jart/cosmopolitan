/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "psa/crypto.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define KEY_BITS        4096
#define BUFFER_SIZE     PSA_BITS_TO_BYTES(KEY_BITS)

static void print_bytestr(const uint8_t *bytes, size_t len)
{
    for (unsigned int idx = 0; idx < len; idx++) {
        printf("%02X", bytes[idx]);
    }
}

int psa_asymmetric_encrypt_decrypt_main(void)
{
    psa_status_t status;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key_id = 0;
    uint8_t original[BUFFER_SIZE/2] = { 0 };
    uint8_t encrypt[BUFFER_SIZE] = { 0 };
    uint8_t decrypt[BUFFER_SIZE] = { 0 };
    size_t encrypted_length;
    size_t decrypted_length;

    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        printf("psa_crypto_init failed\n");
        return EXIT_FAILURE;
    }

    status = psa_generate_random(original, sizeof(original));
    if (status != PSA_SUCCESS) {
        printf("psa_generate_random() failed\n");
        return EXIT_FAILURE;
    }

    psa_set_key_usage_flags(&attributes,
                            PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, PSA_ALG_RSA_PKCS1V15_CRYPT);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_RSA_KEY_PAIR);
    psa_set_key_bits(&attributes, KEY_BITS);

    status = psa_generate_key(&attributes, &key_id);
    if (status != PSA_SUCCESS) {
        printf("psa_generate_key failed (%d)\n", status);
        return EXIT_FAILURE;
    }

    status = psa_asymmetric_encrypt(key_id, PSA_ALG_RSA_PKCS1V15_CRYPT,
                                    original, sizeof(original), NULL, 0,
                                    encrypt, sizeof(encrypt), &encrypted_length);
    if (status != PSA_SUCCESS) {
        printf("psa_asymmetric_encrypt failed (%d)\n", status);
        return EXIT_FAILURE;
    }

    status = psa_asymmetric_decrypt(key_id, PSA_ALG_RSA_PKCS1V15_CRYPT,
                                    encrypt, encrypted_length, NULL, 0,
                                    decrypt, sizeof(decrypt), &decrypted_length);
    if (status != PSA_SUCCESS) {
        printf("psa_cipher_decrypt failed (%d)\n", status);
        return EXIT_FAILURE;
    }

    if (memcmp(original, decrypt, sizeof(original)) != 0) {
        printf("\nEncryption/Decryption failed!\n");
    } else {
        printf("\nEncryption/Decryption successful!\n");
    }

    psa_destroy_key(key_id);
    mbedtls_psa_crypto_free();
    return 0;
}
