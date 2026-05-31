/*
 * Test driver for PAKE entry points.
 */
/*  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <test/helpers.h>

#if defined(PSA_CRYPTO_DRIVER_TEST)
#include "psa_crypto_pake.h"

#include "test/drivers/pake.h"
#include "string.h"

#if defined(MBEDTLS_TEST_LIBTESTDRIVER1)
#include LIBTESTDRIVER1_PSA_DRIVER_INTERNAL_HEADER(psa_crypto_pake.h)
#endif

mbedtls_test_driver_pake_hooks_t mbedtls_test_driver_pake_hooks =
    MBEDTLS_TEST_DRIVER_PAKE_INIT;


psa_status_t mbedtls_test_transparent_pake_setup(
    mbedtls_transparent_test_driver_pake_operation_t *operation,
    const psa_crypto_driver_pake_inputs_t *inputs)
{
    mbedtls_test_driver_pake_hooks.hits.total++;
    mbedtls_test_driver_pake_hooks.hits.setup++;

    if (mbedtls_test_driver_pake_hooks.forced_setup_status != PSA_SUCCESS) {
        mbedtls_test_driver_pake_hooks.driver_status =
            mbedtls_test_driver_pake_hooks.forced_setup_status;
    } else if (!MBEDTLS_TEST_OBJECT_IS_ALL_ZERO(operation)) {
        mbedtls_test_driver_pake_hooks.driver_status =
            PSA_ERROR_TEST_DETECTED_BAD_INITIALIZATION;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_PAKE)
        mbedtls_test_driver_pake_hooks.driver_status =
            libtestdriver1_mbedtls_psa_pake_setup(
                operation, (const libtestdriver1_psa_crypto_driver_pake_inputs_t *) inputs);
#elif defined(MBEDTLS_PSA_BUILTIN_PAKE)
        mbedtls_test_driver_pake_hooks.driver_status =
            mbedtls_psa_pake_setup(
                operation, inputs);
#else
        (void) operation;
        (void) inputs;
        mbedtls_test_driver_pake_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_pake_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_pake_output(
    mbedtls_transparent_test_driver_pake_operation_t *operation,
    psa_crypto_driver_pake_step_t step,
    uint8_t *output,
    size_t output_size,
    size_t *output_length)
{
    mbedtls_test_driver_pake_hooks.hits.total++;
    mbedtls_test_driver_pake_hooks.hits.output++;

    if (mbedtls_test_driver_pake_hooks.forced_output != NULL) {
        if (output_size < mbedtls_test_driver_pake_hooks.forced_output_length) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }

        memcpy(output,
               mbedtls_test_driver_pake_hooks.forced_output,
               mbedtls_test_driver_pake_hooks.forced_output_length);
        *output_length = mbedtls_test_driver_pake_hooks.forced_output_length;

        return mbedtls_test_driver_pake_hooks.forced_status;
    }

    if (mbedtls_test_driver_pake_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_pake_hooks.driver_status =
            mbedtls_test_driver_pake_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_PAKE)
        mbedtls_test_driver_pake_hooks.driver_status =
            libtestdriver1_mbedtls_psa_pake_output(
                operation, (libtestdriver1_psa_crypto_driver_pake_step_t) step,
                output, output_size, output_length);
#elif defined(MBEDTLS_PSA_BUILTIN_PAKE)
        mbedtls_test_driver_pake_hooks.driver_status =
            mbedtls_psa_pake_output(
                operation, step, output, output_size, output_length);
#else
        (void) operation;
        (void) step;
        (void) output;
        (void) output_size;
        (void) output_length;
        mbedtls_test_driver_pake_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_pake_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_pake_input(
    mbedtls_transparent_test_driver_pake_operation_t *operation,
    psa_crypto_driver_pake_step_t step,
    const uint8_t *input,
    size_t input_length)
{
    mbedtls_test_driver_pake_hooks.hits.total++;
    mbedtls_test_driver_pake_hooks.hits.input++;

    if (mbedtls_test_driver_pake_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_pake_hooks.driver_status =
            mbedtls_test_driver_pake_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_PAKE)
        mbedtls_test_driver_pake_hooks.driver_status =
            libtestdriver1_mbedtls_psa_pake_input(
                operation, (libtestdriver1_psa_crypto_driver_pake_step_t) step,
                input, input_length);
#elif defined(MBEDTLS_PSA_BUILTIN_PAKE)
        mbedtls_test_driver_pake_hooks.driver_status =
            mbedtls_psa_pake_input(
                operation, step, input, input_length);
#else
        (void) operation;
        (void) step;
        (void) input;
        (void) input_length;
        mbedtls_test_driver_pake_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_pake_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_pake_get_implicit_key(
    mbedtls_transparent_test_driver_pake_operation_t *operation,
    uint8_t *output, size_t output_size, size_t *output_length)
{
    mbedtls_test_driver_pake_hooks.hits.total++;
    mbedtls_test_driver_pake_hooks.hits.implicit_key++;

    if (mbedtls_test_driver_pake_hooks.forced_status != PSA_SUCCESS) {
        mbedtls_test_driver_pake_hooks.driver_status =
            mbedtls_test_driver_pake_hooks.forced_status;
    } else {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_PAKE)
        mbedtls_test_driver_pake_hooks.driver_status =
            libtestdriver1_mbedtls_psa_pake_get_implicit_key(
                operation,  output, output_size, output_length);
#elif defined(MBEDTLS_PSA_BUILTIN_PAKE)
        mbedtls_test_driver_pake_hooks.driver_status =
            mbedtls_psa_pake_get_implicit_key(
                operation, output, output_size, output_length);
#else
        (void) operation;
        (void) output;
        (void) output_size;
        (void) output_length;
        mbedtls_test_driver_pake_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif
    }

    return mbedtls_test_driver_pake_hooks.driver_status;
}

psa_status_t mbedtls_test_transparent_pake_abort(
    mbedtls_transparent_test_driver_pake_operation_t *operation)
{
    mbedtls_test_driver_pake_hooks.hits.total++;
    mbedtls_test_driver_pake_hooks.hits.abort++;

#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
    defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_PAKE)
    mbedtls_test_driver_pake_hooks.driver_status =
        libtestdriver1_mbedtls_psa_pake_abort(
            operation);
#elif defined(MBEDTLS_PSA_BUILTIN_PAKE)
    mbedtls_test_driver_pake_hooks.driver_status =
        mbedtls_psa_pake_abort(
            operation);
#else
    (void) operation;
    mbedtls_test_driver_pake_hooks.driver_status = PSA_ERROR_NOT_SUPPORTED;
#endif


    if (mbedtls_test_driver_pake_hooks.forced_status != PSA_SUCCESS &&
        mbedtls_test_driver_pake_hooks.driver_status == PSA_SUCCESS) {
        mbedtls_test_driver_pake_hooks.driver_status =
            mbedtls_test_driver_pake_hooks.forced_status;
    }


    return mbedtls_test_driver_pake_hooks.driver_status;
}

#endif /* PSA_CRYPTO_DRIVER_TEST */
