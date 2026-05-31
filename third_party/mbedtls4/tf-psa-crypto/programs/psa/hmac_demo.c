/**
 * PSA API multi-part HMAC demonstration.
 *
 * This programs computes the HMAC of two messages using the multi-part API.
 * It serves as a guide for using the PSA Crypto API, and migrating to it
 * from the legacy MD API.
 *
 * When it comes to multi-part HMAC operations, the `mbedtls_md_context`
 * serves a dual purpose (1) hold the key, and (2) save progress information
 * for the current operation. With PSA those roles are held by two disinct
 * objects: (1) a psa_key_id_t to hold the key, and (2) a psa_operation_t for
 * multi-part progress.
 *
 * This program illustrates the usage of the PSA Crypto API by doing a sequence
 * of multi-part HMAC computations, which were previously done with the
 * legacy MD API.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

/* First include Mbed TLS headers to get the Mbed TLS configuration and
 * platform definitions that we'll use in this program. Also include
 * standard C headers for functions we'll use here. */
#include "tf-psa-crypto/build_info.h"

#include "psa/crypto.h"

#include "mbedtls/platform_util.h" // for mbedtls_platform_zeroize

#include <stdlib.h>
#include <stdio.h>

/* If the build options we need are not enabled, compile a placeholder. */
#if !defined(MBEDTLS_PSA_CRYPTO_C) || \
    defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
int main(void)
{
    printf("MBEDTLS_PSA_CRYPTO_C not defined, "
           "and/or MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER defined\r\n");
    return 0;
}
#else

/* The real program starts here. */

/* Dummy inputs for HMAC */
const unsigned char msg1_part1[] = { 0x01, 0x02 };
const unsigned char msg1_part2[] = { 0x03, 0x04 };
const unsigned char msg2_part1[] = { 0x05, 0x05 };
const unsigned char msg2_part2[] = { 0x06, 0x06 };

/* Dummy key material - never do this in production!
 * This example program uses SHA-256, so a 32-byte key makes sense. */
const unsigned char key_bytes[32] = { 0 };

/* Print the contents of a buffer in hex */
static void print_buf(const char *title, uint8_t *buf, size_t len)
{
    printf("%s:", title);
    for (size_t i = 0; i < len; i++) {
        printf(" %02x", buf[i]);
    }
    printf("\n");
}

/* Run a PSA function and bail out if it fails.
 * The symbolic name of the error code can be recovered using:
 * programs/psa/psa_constant_name status <value> */
#define PSA_CHECK(expr)                                       \
    do                                                          \
    {                                                           \
        status = (expr);                                      \
        if (status != PSA_SUCCESS)                             \
        {                                                       \
            printf("Error %d at line %d: %s\n",                \
                   (int) status,                               \
                   __LINE__,                                   \
                   #expr);                                    \
            goto exit;                                          \
        }                                                       \
    }                                                           \
    while (0)

/*
 * This function demonstrates computation of the HMAC of two messages using
 * the multipart API.
 */
static psa_status_t hmac_demo(void)
{
    psa_status_t status;
    const psa_algorithm_t alg = PSA_ALG_HMAC(PSA_ALG_SHA_256);
    uint8_t out[PSA_MAC_MAX_SIZE]; // safe but not optimal
    /* PSA_MAC_LENGTH(PSA_KEY_TYPE_HMAC, 8 * sizeof( key_bytes ), alg)
     * should work but see https://github.com/Mbed-TLS/mbedtls/issues/4320 */

    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_id_t key = 0;

    /* prepare key */
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_SIGN_MESSAGE);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_HMAC);
    psa_set_key_bits(&attributes, 8 * sizeof(key_bytes));     // optional

    status = psa_import_key(&attributes,
                            key_bytes, sizeof(key_bytes), &key);
    if (status != PSA_SUCCESS) {
        return status;
    }

    /* prepare operation */
    psa_mac_operation_t op = PSA_MAC_OPERATION_INIT;
    size_t out_len = 0;

    /* compute HMAC(key, msg1_part1 | msg1_part2) */
    PSA_CHECK(psa_mac_sign_setup(&op, key, alg));
    PSA_CHECK(psa_mac_update(&op, msg1_part1, sizeof(msg1_part1)));
    PSA_CHECK(psa_mac_update(&op, msg1_part2, sizeof(msg1_part2)));
    PSA_CHECK(psa_mac_sign_finish(&op, out, sizeof(out), &out_len));
    print_buf("msg1", out, out_len);

    /* compute HMAC(key, msg2_part1 | msg2_part2) */
    PSA_CHECK(psa_mac_sign_setup(&op, key, alg));
    PSA_CHECK(psa_mac_update(&op, msg2_part1, sizeof(msg2_part1)));
    PSA_CHECK(psa_mac_update(&op, msg2_part2, sizeof(msg2_part2)));
    PSA_CHECK(psa_mac_sign_finish(&op, out, sizeof(out), &out_len));
    print_buf("msg2", out, out_len);

exit:
    psa_mac_abort(&op);   // needed on error, harmless on success
    psa_destroy_key(key);
    mbedtls_platform_zeroize(out, sizeof(out));

    return status;
}

int main(void)
{
    psa_status_t status = PSA_SUCCESS;

    /* Initialize the PSA crypto library. */
    PSA_CHECK(psa_crypto_init());

    /* Run the demo */
    PSA_CHECK(hmac_demo());

    /* Deinitialize the PSA crypto library. */
    mbedtls_psa_crypto_free();

exit:
    return status == PSA_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}

#endif
