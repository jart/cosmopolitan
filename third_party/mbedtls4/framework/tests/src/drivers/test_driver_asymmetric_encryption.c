/*
 * Test driver for asymmetric encryption.
 */
/*  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <test/helpers.h>

#if defined(PSA_CRYPTO_DRIVER_TEST)
#include "psa/crypto.h"

#if !defined(MBEDTLS_VERSION_MAJOR) || MBEDTLS_VERSION_MAJOR >= 4
#include "mbedtls/private/rsa.h"
#else
#include "mbedtls/rsa.h"
#endif

#include "psa_crypto_rsa.h"
#include "string.h"
#include "test/drivers/asymmetric_encryption.h"
#include "test/drivers/key_management.h"

#if defined(MBEDTLS_TEST_LIBTESTDRIVER1)
#include LIBTESTDRIVER1_PSA_DRIVER_INTERNAL_HEADER(psa_crypto_rsa.h)
#endif

#define PSA_RSA_KEY_PAIR_MAX_SIZE \
    PSA_KEY_EXPORT_RSA_KEY_PAIR_MAX_SIZE(PSA_VENDOR_RSA_MAX_KEY_BITS)

mbedtls_test_driver_asymmetric_encryption_hooks_t mbedtls_test_driver_asymmetric_encryption_hooks =
    MBEDTLS_TEST_DRIVER_ASYMMETRIC_ENCRYPTION_INIT;

psa_status_t mbedtls_test_transparent_asymmetric_encrypt(
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg, const uint8_t *input,
    size_t input_length, const uint8_t *salt, size_t salt_length,
    uint8_t *output, size_t output_size, size_t *output_length)
{
    mbedtls_test_driver_asymmetric_encryption_hooks.hits++;

    if (mbedtls_test_driver_asymmetric_encryption_hooks.forced_output != NULL) {
        if (output_size < mbedtls_test_driver_asymmetric_encryption_hooks.forced_output_length) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }

        memcpy(output,
               mbedtls_test_driver_asymmetric_encryption_hooks.forced_output,
               mbedtls_test_driver_asymmetric_encryption_hooks.forced_output_length);
        *output_length = mbedtls_test_driver_asymmetric_encryption_hooks.forced_output_length;

        return mbedtls_test_driver_asymmetric_encryption_hooks.forced_status;
    }

    if (mbedtls_test_driver_asymmetric_encryption_hooks.forced_status != PSA_SUCCESS) {
        return mbedtls_test_driver_asymmetric_encryption_hooks.forced_status;
    }

#if defined(MBEDTLS_TEST_LIBTESTDRIVER1)
    return libtestdriver1_mbedtls_psa_asymmetric_encrypt(
        (const libtestdriver1_psa_key_attributes_t *) attributes,
        key_buffer, key_buffer_size,
        alg, input, input_length, salt, salt_length,
        output, output_size, output_length);
#else
    return mbedtls_psa_asymmetric_encrypt(
        attributes, key_buffer, key_buffer_size,
        alg, input, input_length, salt, salt_length,
        output, output_size, output_length);
#endif

    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t mbedtls_test_transparent_asymmetric_decrypt(
    const psa_key_attributes_t *attributes, const uint8_t *key_buffer,
    size_t key_buffer_size, psa_algorithm_t alg, const uint8_t *input,
    size_t input_length, const uint8_t *salt, size_t salt_length,
    uint8_t *output, size_t output_size, size_t *output_length)
{
    mbedtls_test_driver_asymmetric_encryption_hooks.hits++;

    if (mbedtls_test_driver_asymmetric_encryption_hooks.forced_output != NULL) {
        if (output_size < mbedtls_test_driver_asymmetric_encryption_hooks.forced_output_length) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }

        memcpy(output,
               mbedtls_test_driver_asymmetric_encryption_hooks.forced_output,
               mbedtls_test_driver_asymmetric_encryption_hooks.forced_output_length);
        *output_length = mbedtls_test_driver_asymmetric_encryption_hooks.forced_output_length;

        return mbedtls_test_driver_asymmetric_encryption_hooks.forced_status;
    }

    if (mbedtls_test_driver_asymmetric_encryption_hooks.forced_status != PSA_SUCCESS) {
        return mbedtls_test_driver_asymmetric_encryption_hooks.forced_status;
    }

#if defined(MBEDTLS_TEST_LIBTESTDRIVER1)
    return libtestdriver1_mbedtls_psa_asymmetric_decrypt(
        (const libtestdriver1_psa_key_attributes_t *) attributes,
        key_buffer, key_buffer_size,
        alg, input, input_length, salt, salt_length,
        output, output_size, output_length);
#else
    return mbedtls_psa_asymmetric_decrypt(
        attributes, key_buffer, key_buffer_size,
        alg, input, input_length, salt, salt_length,
        output, output_size, output_length);
#endif

    return PSA_ERROR_NOT_SUPPORTED;
}

/*
 * opaque versions
 */
psa_status_t mbedtls_test_opaque_asymmetric_encrypt(
    const psa_key_attributes_t *attributes, const uint8_t *key,
    size_t key_length, psa_algorithm_t alg, const uint8_t *input,
    size_t input_length, const uint8_t *salt, size_t salt_length,
    uint8_t *output, size_t output_size, size_t *output_length)
{
    unsigned char unwrapped_key[PSA_RSA_KEY_PAIR_MAX_SIZE];
    size_t unwrapped_key_length;
    psa_status_t status;

    status = mbedtls_test_opaque_unwrap_key(key, key_length,
                                            unwrapped_key, sizeof(unwrapped_key),
                                            &unwrapped_key_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
    (defined(MBEDTLS_PSA_ACCEL_ALG_RSA_OAEP) || defined(MBEDTLS_PSA_ACCEL_ALG_RSA_PKCS1V15_CRYPT))
    return libtestdriver1_mbedtls_psa_asymmetric_encrypt(
        (const libtestdriver1_psa_key_attributes_t *) attributes,
        unwrapped_key, unwrapped_key_length,
        alg, input, input_length, salt, salt_length,
        output, output_size, output_length);
#else
    return mbedtls_psa_asymmetric_encrypt(
        attributes, unwrapped_key, unwrapped_key_length,
        alg, input, input_length, salt, salt_length,
        output, output_size, output_length);
#endif

    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t mbedtls_test_opaque_asymmetric_decrypt(
    const psa_key_attributes_t *attributes, const uint8_t *key,
    size_t key_length, psa_algorithm_t alg, const uint8_t *input,
    size_t input_length, const uint8_t *salt, size_t salt_length,
    uint8_t *output, size_t output_size, size_t *output_length)
{
    unsigned char unwrapped_key[PSA_RSA_KEY_PAIR_MAX_SIZE];
    size_t unwrapped_key_length;
    psa_status_t status;

    status = mbedtls_test_opaque_unwrap_key(key, key_length,
                                            unwrapped_key, sizeof(unwrapped_key),
                                            &unwrapped_key_length);
    if (status != PSA_SUCCESS) {
        return status;
    }

#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
    (defined(MBEDTLS_PSA_ACCEL_ALG_RSA_OAEP) || defined(MBEDTLS_PSA_ACCEL_ALG_RSA_PKCS1V15_CRYPT))
    return libtestdriver1_mbedtls_psa_asymmetric_decrypt(
        (const libtestdriver1_psa_key_attributes_t *) attributes,
        unwrapped_key, unwrapped_key_length,
        alg, input, input_length, salt, salt_length,
        output, output_size, output_length);
#else
    return mbedtls_psa_asymmetric_decrypt(
        attributes, unwrapped_key, unwrapped_key_length,
        alg, input, input_length, salt, salt_length,
        output, output_size, output_length);
#endif

    return PSA_ERROR_NOT_SUPPORTED;
}

#endif /* PSA_CRYPTO_DRIVER_TEST */
