/*
 * Test driver for MAC entry points.
 */
/*  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <test/helpers.h>

#if defined(PSA_CRYPTO_DRIVER_TEST)
#include "psa_crypto_mac.h"

#include "test/drivers/mac.h"

#if defined(MBEDTLS_TEST_LIBTESTDRIVER1)
#include LIBTESTDRIVER1_PSA_DRIVER_INTERNAL_HEADER(psa_crypto_mac.h)
#endif

mbedtls_test_driver_mac_hooks_t mbedtls_test_driver_mac_hooks =
    MBEDTLS_TEST_DRIVER_MAC_INIT;

psa_status_t mbedtls_test_transparent_mac_compute(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length)
{
    mbedtls_test_driver_mac_hooks.hits++;

    if (mbedtls_test_driver_mac_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_test_driver_mac_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_MAC)
        mbedtls_test_driver_mac_hooks.driver_status =
            libtestdriver1_mbedtls_psa_mac_compute(
                (const libtestdriver1_psa_key_attributes_t *) attributes,
                key_buffer, key_buffer_size, alg,
                input, input_length,
                mac, mac_size, mac_length);
#elif defined(MBEDTLS_PSA_BUILTIN_MAC)
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_psa_mac_compute(
                attributes, key_buffer, key_buffer_size, alg,
                input, input_length,
                mac, mac_size, mac_length);
#else
        (void) attributes;
        (void) key_buffer;
        (void) key_buffer_size;
        (void) alg;
        (void) input;
        (void) input_length;
        (void) mac;
        (void) mac_size;
        (void) mac_length;
        mbedtls_test_driver_mac_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_mac_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_mac_sign_setup(
    mbedtls_transparent_test_driver_mac_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg)
{
    mbedtls_test_driver_mac_hooks.hits++;

    if (mbedtls_test_driver_mac_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_test_driver_mac_hooks.forced_status;
    } else if (!MBEDTLS_TEST_OBJECT_IS_ALL_ZERO(operation)) {
        mbedtls_test_driver_mac_hooks.driver_status =
            PSA_ERROR_TEST_DETECTED_BAD_INITIALIZATION;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_MAC)
        mbedtls_test_driver_mac_hooks.driver_status =
            libtestdriver1_mbedtls_psa_mac_sign_setup(
                operation,
                (const libtestdriver1_psa_key_attributes_t *) attributes,
                key_buffer, key_buffer_size, alg);
#elif defined(MBEDTLS_PSA_BUILTIN_MAC)
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_psa_mac_sign_setup(
                operation, attributes, key_buffer, key_buffer_size, alg);
#else
        (void) operation;
        (void) attributes;
        (void) key_buffer;
        (void) key_buffer_size;
        (void) alg;
        mbedtls_test_driver_mac_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_mac_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_mac_verify_setup(
    mbedtls_transparent_test_driver_mac_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg)
{
    mbedtls_test_driver_mac_hooks.hits++;

    if (mbedtls_test_driver_mac_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_test_driver_mac_hooks.forced_status;
    } else if (!MBEDTLS_TEST_OBJECT_IS_ALL_ZERO(operation)) {
        mbedtls_test_driver_mac_hooks.driver_status =
            PSA_ERROR_TEST_DETECTED_BAD_INITIALIZATION;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_MAC)
        mbedtls_test_driver_mac_hooks.driver_status =
            libtestdriver1_mbedtls_psa_mac_verify_setup(
                operation,
                (const libtestdriver1_psa_key_attributes_t *) attributes,
                key_buffer, key_buffer_size, alg);
#elif defined(MBEDTLS_PSA_BUILTIN_MAC)
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_psa_mac_verify_setup(
                operation, attributes, key_buffer, key_buffer_size, alg);
#else
        (void) operation;
        (void) attributes;
        (void) key_buffer;
        (void) key_buffer_size;
        (void) alg;
        mbedtls_test_driver_mac_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_mac_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_mac_update(
    mbedtls_transparent_test_driver_mac_operation_t *operation,
    const uint8_t *input,
    size_t input_length)
{
    mbedtls_test_driver_mac_hooks.hits++;

    if (mbedtls_test_driver_mac_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_test_driver_mac_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_MAC)
        mbedtls_test_driver_mac_hooks.driver_status =
            libtestdriver1_mbedtls_psa_mac_update(
                operation, input, input_length);
#elif defined(MBEDTLS_PSA_BUILTIN_MAC)
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_psa_mac_update(
                operation, input, input_length);
#else
        (void) operation;
        (void) input;
        (void) input_length;
        mbedtls_test_driver_mac_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_mac_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_mac_sign_finish(
    mbedtls_transparent_test_driver_mac_operation_t *operation,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length)
{
    mbedtls_test_driver_mac_hooks.hits++;

    if (mbedtls_test_driver_mac_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_test_driver_mac_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_MAC)
        mbedtls_test_driver_mac_hooks.driver_status =
            libtestdriver1_mbedtls_psa_mac_sign_finish(
                operation, mac, mac_size, mac_length);
#elif defined(MBEDTLS_PSA_BUILTIN_MAC)
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_psa_mac_sign_finish(
                operation, mac, mac_size, mac_length);
#else
        (void) operation;
        (void) mac;
        (void) mac_size;
        (void) mac_length;
        mbedtls_test_driver_mac_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_mac_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_mac_verify_finish(
    mbedtls_transparent_test_driver_mac_operation_t *operation,
    const uint8_t *mac,
    size_t mac_length)
{
    mbedtls_test_driver_mac_hooks.hits++;

    if (mbedtls_test_driver_mac_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_test_driver_mac_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_MAC)
        mbedtls_test_driver_mac_hooks.driver_status =
            libtestdriver1_mbedtls_psa_mac_verify_finish(
                operation, mac, mac_length);
#elif defined(MBEDTLS_PSA_BUILTIN_MAC)
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_psa_mac_verify_finish(
                operation, mac, mac_length);
#else
        (void) operation;
        (void) mac;
        (void) mac_length;
        mbedtls_test_driver_mac_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_mac_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_mac_abort(
    mbedtls_transparent_test_driver_mac_operation_t *operation)
{
    mbedtls_test_driver_mac_hooks.hits++;

    if (mbedtls_test_driver_mac_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_test_driver_mac_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_MAC)
        mbedtls_test_driver_mac_hooks.driver_status =
            libtestdriver1_mbedtls_psa_mac_abort(operation);
#elif defined(MBEDTLS_PSA_BUILTIN_MAC)
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_psa_mac_abort(operation);
#else
        (void) operation;
        mbedtls_test_driver_mac_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_mac_hooks.driver_status;
}

psa_status_t mbedtls_test_opaque_mac_compute(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length)
{
    mbedtls_test_driver_mac_hooks.hits++;

    if (mbedtls_test_driver_mac_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_test_driver_mac_hooks.forced_status;
    } else {
        (void) attributes;
        (void) key_buffer;
        (void) key_buffer_size;
        (void) alg;
        (void) input;
        (void) input_length;
        (void) mac;
        (void) mac_size;
        (void) mac_length;
        mbedtls_test_driver_mac_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
    }

    return mbedtls_test_driver_mac_hooks.driver_status;
}

psa_status_t mbedtls_test_opaque_mac_sign_setup(
    mbedtls_opaque_test_driver_mac_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg)
{
    mbedtls_test_driver_mac_hooks.hits++;

    if (mbedtls_test_driver_mac_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_test_driver_mac_hooks.forced_status;
    } else if (!MBEDTLS_TEST_OBJECT_IS_ALL_ZERO(operation)) {
        mbedtls_test_driver_mac_hooks.driver_status =
            PSA_ERROR_TEST_DETECTED_BAD_INITIALIZATION;
    } else {
        (void) operation;
        (void) attributes;
        (void) key_buffer;
        (void) key_buffer_size;
        (void) alg;
        mbedtls_test_driver_mac_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
    }

    return mbedtls_test_driver_mac_hooks.driver_status;
}

psa_status_t mbedtls_test_opaque_mac_verify_setup(
    mbedtls_opaque_test_driver_mac_operation_t *operation,
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg)
{
    mbedtls_test_driver_mac_hooks.hits++;

    if (mbedtls_test_driver_mac_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_test_driver_mac_hooks.forced_status;
    } else if (!MBEDTLS_TEST_OBJECT_IS_ALL_ZERO(operation)) {
        mbedtls_test_driver_mac_hooks.driver_status =
            PSA_ERROR_TEST_DETECTED_BAD_INITIALIZATION;
    } else {
        (void) operation;
        (void) attributes;
        (void) key_buffer;
        (void) key_buffer_size;
        (void) alg;
        mbedtls_test_driver_mac_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
    }

    return mbedtls_test_driver_mac_hooks.driver_status;
}

psa_status_t mbedtls_test_opaque_mac_update(
    mbedtls_opaque_test_driver_mac_operation_t *operation,
    const uint8_t *input,
    size_t input_length)
{
    mbedtls_test_driver_mac_hooks.hits++;

    if (mbedtls_test_driver_mac_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_test_driver_mac_hooks.forced_status;
    } else {
        (void) operation;
        (void) input;
        (void) input_length;
        mbedtls_test_driver_mac_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
    }

    return mbedtls_test_driver_mac_hooks.driver_status;
}

psa_status_t mbedtls_test_opaque_mac_sign_finish(
    mbedtls_opaque_test_driver_mac_operation_t *operation,
    uint8_t *mac,
    size_t mac_size,
    size_t *mac_length)
{
    mbedtls_test_driver_mac_hooks.hits++;

    if (mbedtls_test_driver_mac_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_test_driver_mac_hooks.forced_status;
    } else {
        (void) operation;
        (void) mac;
        (void) mac_size;
        (void) mac_length;
        mbedtls_test_driver_mac_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
    }

    return mbedtls_test_driver_mac_hooks.driver_status;
}

psa_status_t mbedtls_test_opaque_mac_verify_finish(
    mbedtls_opaque_test_driver_mac_operation_t *operation,
    const uint8_t *mac,
    size_t mac_length)
{
    mbedtls_test_driver_mac_hooks.hits++;

    if (mbedtls_test_driver_mac_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_test_driver_mac_hooks.forced_status;
    } else {
        (void) operation;
        (void) mac;
        (void) mac_length;
        mbedtls_test_driver_mac_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
    }

    return mbedtls_test_driver_mac_hooks.driver_status;
}

psa_status_t mbedtls_test_opaque_mac_abort(
    mbedtls_opaque_test_driver_mac_operation_t *operation)
{
    mbedtls_test_driver_mac_hooks.hits++;

    if (mbedtls_test_driver_mac_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_mac_hooks.driver_status =
            mbedtls_test_driver_mac_hooks.forced_status;
    } else {
        (void) operation;
        mbedtls_test_driver_mac_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
    }

    return mbedtls_test_driver_mac_hooks.driver_status;
}

#endif /* PSA_CRYPTO_DRIVER_TEST */
