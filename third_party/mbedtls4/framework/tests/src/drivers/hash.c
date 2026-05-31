/*
 * Test driver for hash entry points.
 */
/*  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <test/helpers.h>

#if defined(PSA_CRYPTO_DRIVER_TEST)
#include "psa_crypto_hash.h"

#include "test/drivers/hash.h"

#if defined(MBEDTLS_TEST_LIBTESTDRIVER1)
#include LIBTESTDRIVER1_PSA_DRIVER_INTERNAL_HEADER(psa_crypto_hash.h)
#endif

mbedtls_test_driver_hash_hooks_t
    mbedtls_test_driver_hash_hooks = MBEDTLS_TEST_DRIVER_HASH_INIT;

psa_status_t mbedtls_test_transparent_hash_compute(
    psa_algorithm_t alg,
    const uint8_t *input, size_t input_length,
    uint8_t *hash, size_t hash_size, size_t *hash_length)
{
    mbedtls_test_driver_hash_hooks.hits++;

    if (mbedtls_test_driver_hash_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_hash_hooks.driver_status =
            mbedtls_test_driver_hash_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_HASH)
        mbedtls_test_driver_hash_hooks.driver_status =
            libtestdriver1_mbedtls_psa_hash_compute(
                alg, input, input_length,
                hash, hash_size, hash_length);
#elif defined(MBEDTLS_PSA_BUILTIN_HASH)
        mbedtls_test_driver_hash_hooks.driver_status =
            mbedtls_psa_hash_compute(
                alg, input, input_length,
                hash, hash_size, hash_length);
#else
        (void) alg;
        (void) input;
        (void) input_length;
        (void) hash;
        (void) hash_size;
        (void) hash_length;
        mbedtls_test_driver_hash_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_hash_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_hash_setup(
    mbedtls_transparent_test_driver_hash_operation_t *operation,
    psa_algorithm_t alg)
{
    mbedtls_test_driver_hash_hooks.hits++;

    if (mbedtls_test_driver_hash_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_hash_hooks.driver_status =
            mbedtls_test_driver_hash_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_HASH)
        mbedtls_test_driver_hash_hooks.driver_status =
            libtestdriver1_mbedtls_psa_hash_setup(operation, alg);
#elif defined(MBEDTLS_PSA_BUILTIN_HASH)
        mbedtls_test_driver_hash_hooks.driver_status =
            mbedtls_psa_hash_setup(operation, alg);
#else
        (void) operation;
        (void) alg;
        mbedtls_test_driver_hash_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_hash_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_hash_clone(
    const mbedtls_transparent_test_driver_hash_operation_t *source_operation,
    mbedtls_transparent_test_driver_hash_operation_t *target_operation)
{
    mbedtls_test_driver_hash_hooks.hits++;

    if (mbedtls_test_driver_hash_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_hash_hooks.driver_status =
            mbedtls_test_driver_hash_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_HASH)
        mbedtls_test_driver_hash_hooks.driver_status =
            libtestdriver1_mbedtls_psa_hash_clone(source_operation,
                                                  target_operation);
#elif defined(MBEDTLS_PSA_BUILTIN_HASH)
        mbedtls_test_driver_hash_hooks.driver_status =
            mbedtls_psa_hash_clone(source_operation, target_operation);
#else
        (void) source_operation;
        (void) target_operation;
        mbedtls_test_driver_hash_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_hash_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_hash_update(
    mbedtls_transparent_test_driver_hash_operation_t *operation,
    const uint8_t *input,
    size_t input_length)
{
    mbedtls_test_driver_hash_hooks.hits++;

    if (mbedtls_test_driver_hash_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_hash_hooks.driver_status =
            mbedtls_test_driver_hash_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_HASH)
        mbedtls_test_driver_hash_hooks.driver_status =
            libtestdriver1_mbedtls_psa_hash_update(
                operation, input, input_length);
#elif defined(MBEDTLS_PSA_BUILTIN_HASH)
        mbedtls_test_driver_hash_hooks.driver_status =
            mbedtls_psa_hash_update(operation, input, input_length);
#else
        (void) operation;
        (void) input;
        (void) input_length;
        mbedtls_test_driver_hash_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_hash_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_hash_finish(
    mbedtls_transparent_test_driver_hash_operation_t *operation,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length)
{
    mbedtls_test_driver_hash_hooks.hits++;

    if (mbedtls_test_driver_hash_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_hash_hooks.driver_status =
            mbedtls_test_driver_hash_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_HASH)
        mbedtls_test_driver_hash_hooks.driver_status =
            libtestdriver1_mbedtls_psa_hash_finish(
                operation, hash, hash_size, hash_length);
#elif defined(MBEDTLS_PSA_BUILTIN_HASH)
        mbedtls_test_driver_hash_hooks.driver_status =
            mbedtls_psa_hash_finish(operation, hash, hash_size, hash_length);
#else
        (void) operation;
        (void) hash;
        (void) hash_size;
        (void) hash_length;
        mbedtls_test_driver_hash_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_hash_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_hash_abort(
    mbedtls_transparent_test_driver_hash_operation_t *operation)
{
    mbedtls_test_driver_hash_hooks.hits++;

    if (mbedtls_test_driver_hash_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_hash_hooks.driver_status =
            mbedtls_test_driver_hash_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_HASH)
        mbedtls_test_driver_hash_hooks.driver_status =
            libtestdriver1_mbedtls_psa_hash_abort(operation);
#elif defined(MBEDTLS_PSA_BUILTIN_HASH)
        mbedtls_test_driver_hash_hooks.driver_status =
            mbedtls_psa_hash_abort(operation);
#else
        (void) operation;
        mbedtls_test_driver_hash_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_hash_hooks.driver_status;
}
#endif /* PSA_CRYPTO_DRIVER_TEST */
