/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */


#include "psa/crypto.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "mbedtls/build_info.h"
#include "mbedtls/debug.h"
#include "mbedtls/platform.h"

#define BUFFER_SIZE 500

#define SERVER_PK_VALUE {                                     \
        0x04, 0xde, 0xa5, 0xe4, 0x5d, 0x0e, 0xa3, 0x7f, 0xc5, \
        0x66, 0x23, 0x2a, 0x50, 0x8f, 0x4a, 0xd2, 0x0e, 0xa1, \
        0x3d, 0x47, 0xe4, 0xbf, 0x5f, 0xa4, 0xd5, 0x4a, 0x57, \
        0xa0, 0xba, 0x01, 0x20, 0x42, 0x08, 0x70, 0x97, 0x49, \
        0x6e, 0xfc, 0x58, 0x3f, 0xed, 0x8b, 0x24, 0xa5, 0xb9, \
        0xbe, 0x9a, 0x51, 0xde, 0x06, 0x3f, 0x5a, 0x00, 0xa8, \
        0xb6, 0x98, 0xa1, 0x6f, 0xd7, 0xf2, 0x9b, 0x54, 0x85, \
        0xf3, 0x20                                            \
}

#define KEY_BITS 256

int psa_key_agreement_main(void)
{
    psa_status_t status;
    psa_key_attributes_t client_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t server_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t check_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t client_key_id = 0;
    psa_key_id_t server_key_id = 0;
    uint8_t client_pk[BUFFER_SIZE] = { 0 };
    size_t client_pk_len;
    size_t key_bits;
    psa_key_type_t key_type;

    const uint8_t server_pk[] = SERVER_PK_VALUE;
    uint8_t derived_key[BUFFER_SIZE] = { 0 };
    size_t derived_key_len;

    status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        mbedtls_printf("psa_crypto_init failed\n");
        return EXIT_FAILURE;
    }

    psa_set_key_usage_flags(&client_attributes, PSA_KEY_USAGE_DERIVE);
    psa_set_key_algorithm(&client_attributes, PSA_ALG_ECDH);
    psa_set_key_type(&client_attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&client_attributes, KEY_BITS);

    /* Generate ephemeral key pair */
    status = psa_generate_key(&client_attributes, &client_key_id);
    if (status != PSA_SUCCESS) {
        mbedtls_printf("psa_generate_key failed\n");
        return EXIT_FAILURE;
    }
    status = psa_export_public_key(client_key_id,
                                   client_pk, sizeof(client_pk),
                                   &client_pk_len);
    if (status != PSA_SUCCESS) {
        mbedtls_printf("psa_export_public_key failed\n");
        return EXIT_FAILURE;
    }

    mbedtls_printf("Client Public Key (%zu bytes):\n", client_pk_len);

    for (size_t j = 0; j < client_pk_len; j++) {
        if (j % 8 == 0) {
            mbedtls_printf("\n    ");
        }
        mbedtls_printf("%02x ", client_pk[j]);
    }
    mbedtls_printf("\n\n");

    psa_set_key_usage_flags(&server_attributes, PSA_KEY_USAGE_DERIVE | PSA_KEY_USAGE_EXPORT);
    psa_set_key_algorithm(&server_attributes, PSA_ALG_ECDSA_ANY);
    psa_set_key_type(&server_attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));

    /* Import server public key */
    status = psa_import_key(&server_attributes, server_pk, sizeof(server_pk), &server_key_id);
    if (status != PSA_SUCCESS) {
        mbedtls_printf("psa_import_key failed\n");
        return EXIT_FAILURE;
    }

    status = psa_get_key_attributes(server_key_id, &check_attributes);
    if (status != PSA_SUCCESS) {
        mbedtls_printf("psa_get_key_attributes failed\n");
        return EXIT_FAILURE;
    }

    key_bits = psa_get_key_bits(&check_attributes);
    if (key_bits != 256) {
        mbedtls_printf("Incompatible key size!\n");
        return EXIT_FAILURE;
    }

    key_type = psa_get_key_type(&check_attributes);
    if (key_type != PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1)) {
        mbedtls_printf("Unsupported key type!\n");
        return EXIT_FAILURE;
    }

    mbedtls_printf("Server Public Key (%zu bytes):\n", sizeof(server_pk));

    for (size_t j = 0; j < sizeof(server_pk); j++) {
        if (j % 8 == 0) {
            mbedtls_printf("\n    ");
        }
        mbedtls_printf("%02x ", server_pk[j]);
    }
    mbedtls_printf("\n\n");

    /* Generate ECDHE derived key */
    status = psa_raw_key_agreement(PSA_ALG_ECDH,                        // algorithm
                                   client_key_id,                   // client secret key
                                   server_pk, sizeof(server_pk),        // server public key
                                   derived_key, sizeof(derived_key),    // buffer to store derived key
                                   &derived_key_len);
    if (status != PSA_SUCCESS) {
        mbedtls_printf("psa_raw_key_agreement failed\n");
        return EXIT_FAILURE;
    }

    mbedtls_printf("Derived Key (%zu bytes):\n", derived_key_len);

    for (size_t j = 0; j < derived_key_len; j++) {
        if (j % 8 == 0) {
            mbedtls_printf("\n    ");
        }
        mbedtls_printf("%02x ", derived_key[j]);
    }
    mbedtls_printf("\n");

    psa_destroy_key(server_key_id);
    psa_destroy_key(client_key_id);
    mbedtls_psa_crypto_free();
    return EXIT_SUCCESS;
}
