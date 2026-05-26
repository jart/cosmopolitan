/*
 * Test driver for AEAD entry points.
 */
/*  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <test/helpers.h>

#if defined(PSA_CRYPTO_DRIVER_TEST)
#include "psa_crypto_aead.h"
#include "psa_crypto_core.h"

#include "test/drivers/aead.h"

#include "mbedtls/constant_time.h"

#if defined(MBEDTLS_TEST_LIBTESTDRIVER1)
#include LIBTESTDRIVER1_PSA_DRIVER_INTERNAL_HEADER(psa_crypto_aead.h)
#endif

mbedtls_test_driver_aead_hooks_t
    mbedtls_test_driver_aead_hooks = MBEDTLS_TEST_DRIVER_AEAD_INIT;

psa_status_t mbedtls_test_transparent_aead_encrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *nonce, size_t nonce_length,
    const uint8_t *additional_data, size_t additional_data_length,
    const uint8_t *plaintext, size_t plaintext_length,
    uint8_t *ciphertext, size_t ciphertext_size, size_t *ciphertext_length)
{
    mbedtls_test_driver_aead_hooks.hits_encrypt++;

    if (mbedtls_test_driver_aead_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_test_driver_aead_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            libtestdriver1_mbedtls_psa_aead_encrypt(
                (const libtestdriver1_psa_key_attributes_t *) attributes,
                key_buffer, key_buffer_size,
                alg,
                nonce, nonce_length,
                additional_data, additional_data_length,
                plaintext, plaintext_length,
                ciphertext, ciphertext_size, ciphertext_length);
#elif defined(MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_psa_aead_encrypt(
                attributes, key_buffer, key_buffer_size,
                alg,
                nonce, nonce_length,
                additional_data, additional_data_length,
                plaintext, plaintext_length,
                ciphertext, ciphertext_size, ciphertext_length);
#else
        (void) attributes;
        (void) key_buffer;
        (void) key_buffer_size;
        (void) alg;
        (void) nonce;
        (void) nonce_length;
        (void) additional_data;
        (void) additional_data_length;
        (void) plaintext;
        (void) plaintext_length;
        (void) ciphertext;
        (void) ciphertext_size;
        (void) ciphertext_length;
        mbedtls_test_driver_aead_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_aead_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_aead_decrypt(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *nonce, size_t nonce_length,
    const uint8_t *additional_data, size_t additional_data_length,
    const uint8_t *ciphertext, size_t ciphertext_length,
    uint8_t *plaintext, size_t plaintext_size, size_t *plaintext_length)
{
    mbedtls_test_driver_aead_hooks.hits_decrypt++;

    if (mbedtls_test_driver_aead_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_test_driver_aead_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            libtestdriver1_mbedtls_psa_aead_decrypt(
                (const libtestdriver1_psa_key_attributes_t *) attributes,
                key_buffer, key_buffer_size,
                alg,
                nonce, nonce_length,
                additional_data, additional_data_length,
                ciphertext, ciphertext_length,
                plaintext, plaintext_size, plaintext_length);
#elif defined(MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_psa_aead_decrypt(
                attributes, key_buffer, key_buffer_size,
                alg,
                nonce, nonce_length,
                additional_data, additional_data_length,
                ciphertext, ciphertext_length,
                plaintext, plaintext_size, plaintext_length);
#else
        (void) attributes;
        (void) key_buffer;
        (void) key_buffer_size;
        (void) alg;
        (void) nonce;
        (void) nonce_length;
        (void) additional_data;
        (void) additional_data_length;
        (void) ciphertext;
        (void) ciphertext_length;
        (void) plaintext;
        (void) plaintext_size;
        (void) plaintext_length;
        mbedtls_test_driver_aead_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_aead_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_aead_encrypt_setup(
    mbedtls_transparent_test_driver_aead_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg)
{
    mbedtls_test_driver_aead_hooks.hits_encrypt_setup++;

    if (mbedtls_test_driver_aead_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_test_driver_aead_hooks.forced_status;
    } else if (!MBEDTLS_TEST_OBJECT_IS_ALL_ZERO(operation)) {
        mbedtls_test_driver_aead_hooks.driver_status =
            PSA_ERROR_TEST_DETECTED_BAD_INITIALIZATION;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            libtestdriver1_mbedtls_psa_aead_encrypt_setup(operation,
                                                          (const libtestdriver1_psa_key_attributes_t
                                                           *) attributes,
                                                          key_buffer,
                                                          key_buffer_size, alg);
#elif defined(MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_psa_aead_encrypt_setup(operation, attributes, key_buffer,
                                           key_buffer_size, alg);
#else
        (void) operation;
        (void) attributes;
        (void) key_buffer;
        (void) key_buffer_size;
        (void) alg;
        mbedtls_test_driver_aead_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_aead_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_aead_decrypt_setup(
    mbedtls_transparent_test_driver_aead_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg)
{
    mbedtls_test_driver_aead_hooks.hits_decrypt_setup++;

    if (mbedtls_test_driver_aead_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_test_driver_aead_hooks.forced_status;
    } else if (!MBEDTLS_TEST_OBJECT_IS_ALL_ZERO(operation)) {
        mbedtls_test_driver_aead_hooks.driver_status =
            PSA_ERROR_TEST_DETECTED_BAD_INITIALIZATION;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            libtestdriver1_mbedtls_psa_aead_decrypt_setup(operation,
                                                          (const libtestdriver1_psa_key_attributes_t
                                                           *) attributes,
                                                          key_buffer, key_buffer_size, alg);
#elif defined(MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_psa_aead_decrypt_setup(operation, attributes, key_buffer,
                                           key_buffer_size, alg);
#else
        (void) operation;
        (void) attributes;
        (void) key_buffer;
        (void) key_buffer_size;
        (void) alg;
        mbedtls_test_driver_aead_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_aead_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_aead_set_nonce(
    mbedtls_transparent_test_driver_aead_operation_t *operation,
    const uint8_t *nonce,
    size_t nonce_length)
{
    mbedtls_test_driver_aead_hooks.hits_set_nonce++;

    if (mbedtls_test_driver_aead_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_test_driver_aead_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            libtestdriver1_mbedtls_psa_aead_set_nonce(operation, nonce, nonce_length);
#elif defined(MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_psa_aead_set_nonce(operation, nonce, nonce_length);
#else
        (void) operation;
        (void) nonce;
        (void) nonce_length;
        mbedtls_test_driver_aead_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_aead_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_aead_set_lengths(
    mbedtls_transparent_test_driver_aead_operation_t *operation,
    size_t ad_length,
    size_t plaintext_length)
{
    mbedtls_test_driver_aead_hooks.hits_set_lengths++;

    if (mbedtls_test_driver_aead_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_test_driver_aead_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            libtestdriver1_mbedtls_psa_aead_set_lengths(operation, ad_length,
                                                        plaintext_length);
#elif defined(MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_psa_aead_set_lengths(operation, ad_length,
                                         plaintext_length);
#else
        (void) operation;
        (void) ad_length;
        (void) plaintext_length;
        mbedtls_test_driver_aead_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_aead_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_aead_update_ad(
    mbedtls_transparent_test_driver_aead_operation_t *operation,
    const uint8_t *input,
    size_t input_length)
{
    mbedtls_test_driver_aead_hooks.hits_update_ad++;

    if (mbedtls_test_driver_aead_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_test_driver_aead_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            libtestdriver1_mbedtls_psa_aead_update_ad(operation, input, input_length);
#elif defined(MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_psa_aead_update_ad(operation, input, input_length);
#else
        (void) operation;
        (void) input;
        (void) input_length;
        mbedtls_test_driver_aead_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_aead_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_aead_update(
    mbedtls_transparent_test_driver_aead_operation_t *operation,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length)
{
    mbedtls_test_driver_aead_hooks.hits_update++;

    if (mbedtls_test_driver_aead_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_test_driver_aead_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            libtestdriver1_mbedtls_psa_aead_update(operation, input,
                                                   input_length, output,
                                                   output_size, output_length);
#elif defined(MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_psa_aead_update(operation, input, input_length, output,
                                    output_size, output_length);
#else
        (void) operation;
        (void) input;
        (void) input_length;
        (void) output;
        (void) output_size;
        (void) output_length;
        mbedtls_test_driver_aead_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_aead_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_aead_finish(
    mbedtls_transparent_test_driver_aead_operation_t *operation,
    uint8_t *ciphertext,
    size_t ciphertext_size,
    size_t *ciphertext_length,
    uint8_t *tag,
    size_t tag_size,
    size_t *tag_length)
{
    mbedtls_test_driver_aead_hooks.hits_finish++;

    if (mbedtls_test_driver_aead_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_test_driver_aead_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            libtestdriver1_mbedtls_psa_aead_finish(operation, ciphertext,
                                                   ciphertext_size, ciphertext_length,
                                                   tag, tag_size, tag_length);
#elif defined(MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_psa_aead_finish(operation, ciphertext, ciphertext_size,
                                    ciphertext_length, tag, tag_size,
                                    tag_length);
#else
        (void) operation;
        (void) ciphertext;
        (void) ciphertext_size;
        (void) ciphertext_length;
        (void) tag;
        (void) tag_size;
        (void) tag_length;
        mbedtls_test_driver_aead_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_aead_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_aead_verify(
    mbedtls_transparent_test_driver_aead_operation_t *operation,
    uint8_t *plaintext,
    size_t plaintext_size,
    size_t *plaintext_length,
    const uint8_t *tag,
    size_t tag_length)
{
    mbedtls_test_driver_aead_hooks.hits_verify++;

    if (mbedtls_test_driver_aead_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_test_driver_aead_hooks.forced_status;
    } else {
        uint8_t check_tag[PSA_AEAD_TAG_MAX_SIZE];
        size_t check_tag_length = 0;

#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            libtestdriver1_mbedtls_psa_aead_finish(operation,
                                                   plaintext,
                                                   plaintext_size,
                                                   plaintext_length,
                                                   check_tag,
                                                   sizeof(check_tag),
                                                   &check_tag_length);
#elif defined(MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_psa_aead_finish(operation,
                                    plaintext,
                                    plaintext_size,
                                    plaintext_length,
                                    check_tag,
                                    sizeof(check_tag),
                                    &check_tag_length);
#else
        (void) operation;
        (void) plaintext;
        (void) plaintext_size;
        (void) plaintext_length;
        mbedtls_test_driver_aead_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif

        if (mbedtls_test_driver_aead_hooks.driver_status == PSA_SUCCESS) {
            if (tag_length != check_tag_length ||
                mbedtls_ct_memcmp(tag, check_tag, tag_length)
                != 0) {
                mbedtls_test_driver_aead_hooks.driver_status =
                    PSA_ERROR_INVALID_SIGNATURE;
            }
        }

        mbedtls_platform_zeroize(check_tag, sizeof(check_tag));
    }

    return mbedtls_test_driver_aead_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_aead_abort(
    mbedtls_transparent_test_driver_aead_operation_t *operation)
{
    mbedtls_test_driver_aead_hooks.hits_abort++;

    if (mbedtls_test_driver_aead_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_test_driver_aead_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            libtestdriver1_mbedtls_psa_aead_abort(operation);
#elif defined(MBEDTLS_PSA_BUILTIN_AEAD)
        mbedtls_test_driver_aead_hooks.driver_status =
            mbedtls_psa_aead_abort(operation);
#else
        (void) operation;
        mbedtls_test_driver_aead_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_aead_hooks.driver_status;
}

#endif /* PSA_CRYPTO_DRIVER_TEST */
