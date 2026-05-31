/*
 * Test driver for xof entry points.
 */
/*  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <test/helpers.h>

#if defined(PSA_ALG_CATEGORY_XOF) && defined(PSA_CRYPTO_DRIVER_TEST)

#include "psa_crypto_xof.h"

#include "test/drivers/xof.h"

#if defined(MBEDTLS_TEST_LIBTESTDRIVER1)
#include LIBTESTDRIVER1_PSA_DRIVER_INTERNAL_HEADER(psa_crypto_xof.h)
#endif

mbedtls_test_driver_xof_hooks_t
    mbedtls_test_driver_xof_hooks = MBEDTLS_TEST_DRIVER_XOF_INIT;

psa_status_t mbedtls_test_transparent_xof_setup(
    mbedtls_transparent_test_driver_xof_operation_t *operation,
    psa_algorithm_t alg)
{
    mbedtls_test_driver_xof_hooks.hits++;

    if (mbedtls_test_driver_xof_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_xof_hooks.driver_status =
            mbedtls_test_driver_xof_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_XOF)
        mbedtls_test_driver_xof_hooks.driver_status =
            libtestdriver1_mbedtls_psa_xof_setup(operation, alg);
#elif defined(MBEDTLS_PSA_BUILTIN_XOF)
        mbedtls_test_driver_xof_hooks.driver_status =
            mbedtls_psa_xof_setup(operation, alg);
#else
        (void) operation;
        (void) alg;
        mbedtls_test_driver_xof_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_xof_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_xof_set_context(
    mbedtls_transparent_test_driver_xof_operation_t *operation,
    const uint8_t *context, size_t context_length)
{
    mbedtls_test_driver_xof_hooks.hits++;

    if (mbedtls_test_driver_xof_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_xof_hooks.driver_status =
            mbedtls_test_driver_xof_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_XOF)
        mbedtls_test_driver_xof_hooks.driver_status =
            libtestdriver1_mbedtls_psa_xof_set_context(
                operation, context, context_length);
#elif defined(MBEDTLS_PSA_BUILTIN_XOF)
        mbedtls_test_driver_xof_hooks.driver_status =
            mbedtls_psa_xof_set_context(operation, context, context_length);
#else
        (void) operation;
        (void) context;
        (void) context_length;
        mbedtls_test_driver_xof_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_xof_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_xof_update(
    mbedtls_transparent_test_driver_xof_operation_t *operation,
    const uint8_t *input,
    size_t input_length)
{
    mbedtls_test_driver_xof_hooks.hits++;

    if (mbedtls_test_driver_xof_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_xof_hooks.driver_status =
            mbedtls_test_driver_xof_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_XOF)
        mbedtls_test_driver_xof_hooks.driver_status =
            libtestdriver1_mbedtls_psa_xof_update(
                operation, input, input_length);
#elif defined(MBEDTLS_PSA_BUILTIN_XOF)
        mbedtls_test_driver_xof_hooks.driver_status =
            mbedtls_psa_xof_update(operation, input, input_length);
#else
        (void) operation;
        (void) input;
        (void) input_length;
        mbedtls_test_driver_xof_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_xof_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_xof_output(
    mbedtls_transparent_test_driver_xof_operation_t *operation,
    uint8_t *output, size_t output_length)
{
    mbedtls_test_driver_xof_hooks.hits++;

    if (mbedtls_test_driver_xof_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_xof_hooks.driver_status =
            mbedtls_test_driver_xof_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_XOF)
        mbedtls_test_driver_xof_hooks.driver_status =
            libtestdriver1_mbedtls_psa_xof_output(
                operation, output, output_length);
#elif defined(MBEDTLS_PSA_BUILTIN_XOF)
        mbedtls_test_driver_xof_hooks.driver_status =
            mbedtls_psa_xof_output(operation, output, output_length);
#else
        (void) operation;
        (void) output;
        (void) output_length;
        mbedtls_test_driver_xof_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_xof_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_xof_abort(
    mbedtls_transparent_test_driver_xof_operation_t *operation)
{
    mbedtls_test_driver_xof_hooks.hits++;

    if (mbedtls_test_driver_xof_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_xof_hooks.driver_status =
            mbedtls_test_driver_xof_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_XOF)
        mbedtls_test_driver_xof_hooks.driver_status =
            libtestdriver1_mbedtls_psa_xof_abort(operation);
#elif defined(MBEDTLS_PSA_BUILTIN_XOF)
        mbedtls_test_driver_xof_hooks.driver_status =
            mbedtls_psa_xof_abort(operation);
#else
        (void) operation;
        mbedtls_test_driver_xof_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_xof_hooks.driver_status;
}

#endif /* PSA_CRYPTO_DRIVER_TEST && defined(PSA_ALG_CATEGORY_XOF) */
