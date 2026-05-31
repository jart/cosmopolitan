/**
 * PSA API multi-part AEAD demonstration.
 *
 * This program AEAD-encrypts a message, using the algorithm and key size
 * specified on the command line, using the multi-part API. It serves as a
 * guide for using the PSA Crypto API, and migrating to it from the legacy
 * Cipher API.
 *
 * When used with multi-part AEAD operations, the `mbedtls_cipher_context`
 * serves a triple purpose (1) hold the key, (2) store the algorithm when no
 * operation is active, and (3) save progress information for the current
 * operation. With PSA those roles are held by disinct objects: (1) a
 * psa_key_id_t to hold the key, a (2) psa_algorithm_t to represent the
 * algorithm, and (3) a psa_operation_t for multi-part progress.
 *
 * On the other hand, with PSA, the algorithms encodes the desired tag length;
 * with Cipher the desired tag length needs to be tracked separately.
 *
 * This program illustrates the usage of the PSA Crypto API by doing a sequence
 * of multi-part AEAD computations, which were previously done with the
 * legacy Cipher API.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* If the build options we need are not enabled, compile a placeholder. */
#if !defined(MBEDTLS_PSA_CRYPTO_C) || \
    !defined(MBEDTLS_AES_C) || !defined(MBEDTLS_GCM_C) || \
    !defined(MBEDTLS_CHACHAPOLY_C) || \
    defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
int main(void)
{
    printf("MBEDTLS_PSA_CRYPTO_C and/or "
           "MBEDTLS_AES_C and/or MBEDTLS_GCM_C and/or "
           "MBEDTLS_CHACHAPOLY_C not defined, and/or "
           "MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER defined\r\n");
    return 0;
}
#else

/* The real program starts here. */

const char usage[] =
    "Usage: aead_demo [aes128-gcm|aes256-gcm|aes128-gcm_8|chachapoly]";

/* Dummy data for encryption: IV/nonce, additional data, 2-part message */
const unsigned char iv1[12] = { 0x00 };
const unsigned char add_data1[] = { 0x01, 0x02 };
const unsigned char msg1_part1[] = { 0x03, 0x04 };
const unsigned char msg1_part2[] = { 0x05, 0x06, 0x07 };

/* Dummy data (2nd message) */
const unsigned char iv2[12] = { 0x10 };
const unsigned char add_data2[] = { 0x11, 0x12 };
const unsigned char msg2_part1[] = { 0x13, 0x14 };
const unsigned char msg2_part2[] = { 0x15, 0x16, 0x17 };

/* Maximum total size of the messages */
#define MSG1_SIZE (sizeof(msg1_part1) + sizeof(msg1_part2))
#define MSG2_SIZE (sizeof(msg2_part1) + sizeof(msg2_part2))
#define MSG_MAX_SIZE (MSG1_SIZE > MSG2_SIZE ? MSG1_SIZE : MSG2_SIZE)

/* Dummy key material - never do this in production!
 * 32-byte is enough to all the key size supported by this program. */
const unsigned char key_bytes[32] = { 0x2a };

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
 * Prepare encryption material:
 * - interpret command-line argument
 * - set up key
 * - outputs: key and algorithm, which together hold all the information
 */
static psa_status_t aead_prepare(const char *info,
                                 psa_key_id_t *key,
                                 psa_algorithm_t *alg)
{
    psa_status_t status;

    /* Convert arg to alg + key_bits + key_type */
    size_t key_bits;
    psa_key_type_t key_type;
    if (strcmp(info, "aes128-gcm") == 0) {
        *alg = PSA_ALG_GCM;
        key_bits = 128;
        key_type = PSA_KEY_TYPE_AES;
    } else if (strcmp(info, "aes256-gcm") == 0) {
        *alg = PSA_ALG_GCM;
        key_bits = 256;
        key_type = PSA_KEY_TYPE_AES;
    } else if (strcmp(info, "aes128-gcm_8") == 0) {
        *alg = PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 8);
        key_bits = 128;
        key_type = PSA_KEY_TYPE_AES;
    } else if (strcmp(info, "chachapoly") == 0) {
        *alg = PSA_ALG_CHACHA20_POLY1305;
        key_bits = 256;
        key_type = PSA_KEY_TYPE_CHACHA20;
    } else {
        puts(usage);
        return PSA_ERROR_INVALID_ARGUMENT;
    }

    /* Prepare key attributes */
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_algorithm(&attributes, *alg);
    psa_set_key_type(&attributes, key_type);
    psa_set_key_bits(&attributes, key_bits);   // optional

    /* Import key */
    PSA_CHECK(psa_import_key(&attributes, key_bytes, key_bits / 8, key));

exit:
    return status;
}

/*
 * Print out some information.
 *
 * All of this information was present in the command line argument, but his
 * function demonstrates how each piece can be recovered from (key, alg).
 */
static void aead_info(psa_key_id_t key, psa_algorithm_t alg)
{
    psa_key_attributes_t attr = PSA_KEY_ATTRIBUTES_INIT;
    (void) psa_get_key_attributes(key, &attr);
    psa_key_type_t key_type = psa_get_key_type(&attr);
    size_t key_bits = psa_get_key_bits(&attr);
    psa_algorithm_t base_alg = PSA_ALG_AEAD_WITH_DEFAULT_LENGTH_TAG(alg);
    size_t tag_len = PSA_AEAD_TAG_LENGTH(key_type, key_bits, alg);

    const char *type_str = key_type == PSA_KEY_TYPE_AES ? "AES"
                         : key_type == PSA_KEY_TYPE_CHACHA20 ? "Chacha"
                         : "???";
    const char *base_str = base_alg == PSA_ALG_GCM ? "GCM"
                         : base_alg == PSA_ALG_CHACHA20_POLY1305 ? "ChachaPoly"
                         : "???";

    printf("%s, %u, %s, %u\n",
           type_str, (unsigned) key_bits, base_str, (unsigned) tag_len);
}

/*
 * Encrypt a 2-part message.
 */
static int aead_encrypt(psa_key_id_t key, psa_algorithm_t alg,
                        const unsigned char *iv, size_t iv_len,
                        const unsigned char *ad, size_t ad_len,
                        const unsigned char *part1, size_t part1_len,
                        const unsigned char *part2, size_t part2_len)
{
    psa_status_t status;
    size_t olen, olen_tag;
    unsigned char out[PSA_AEAD_ENCRYPT_OUTPUT_MAX_SIZE(MSG_MAX_SIZE)];
    unsigned char *p = out, *end = out + sizeof(out);
    unsigned char tag[PSA_AEAD_TAG_MAX_SIZE];

    psa_aead_operation_t op = PSA_AEAD_OPERATION_INIT;
    PSA_CHECK(psa_aead_encrypt_setup(&op, key, alg));

    PSA_CHECK(psa_aead_set_nonce(&op, iv, iv_len));
    PSA_CHECK(psa_aead_update_ad(&op, ad, ad_len));
    PSA_CHECK(psa_aead_update(&op, part1, part1_len, p, end - p, &olen));
    p += olen;
    PSA_CHECK(psa_aead_update(&op, part2, part2_len, p, end - p, &olen));
    p += olen;
    PSA_CHECK(psa_aead_finish(&op, p, end - p, &olen,
                              tag, sizeof(tag), &olen_tag));
    p += olen;
    memcpy(p, tag, olen_tag);
    p += olen_tag;

    olen = p - out;
    print_buf("out", out, olen);

exit:
    psa_aead_abort(&op);   // required on errors, harmless on success
    return status;
}

/*
 * AEAD demo: set up key/alg, print out info, encrypt messages.
 */
static psa_status_t aead_demo(const char *info)
{
    psa_status_t status;

    psa_key_id_t key;
    psa_algorithm_t alg;

    PSA_CHECK(aead_prepare(info, &key, &alg));

    aead_info(key, alg);

    PSA_CHECK(aead_encrypt(key, alg,
                           iv1, sizeof(iv1), add_data1, sizeof(add_data1),
                           msg1_part1, sizeof(msg1_part1),
                           msg1_part2, sizeof(msg1_part2)));
    PSA_CHECK(aead_encrypt(key, alg,
                           iv2, sizeof(iv2), add_data2, sizeof(add_data2),
                           msg2_part1, sizeof(msg2_part1),
                           msg2_part2, sizeof(msg2_part2)));

exit:
    psa_destroy_key(key);

    return status;
}

/*
 * Main function
 */
int main(int argc, char **argv)
{
    psa_status_t status = PSA_SUCCESS;

    /* Check usage */
    if (argc != 2) {
        puts(usage);
        return EXIT_FAILURE;
    }

    /* Initialize the PSA crypto library. */
    PSA_CHECK(psa_crypto_init());

    /* Run the demo */
    PSA_CHECK(aead_demo(argv[1]));

    /* Deinitialize the PSA crypto library. */
    mbedtls_psa_crypto_free();

exit:
    return status == PSA_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}

#endif
