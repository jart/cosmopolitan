/*
 * Test driver for xof driver entry points.
 */
/*  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef PSA_CRYPTO_TEST_DRIVERS_XOF_H
#define PSA_CRYPTO_TEST_DRIVERS_XOF_H

#include "mbedtls/build_info.h"

#if defined(PSA_CRYPTO_DRIVER_TEST)
#include "test_driver_common.h"

#include <psa/crypto_driver_common.h>

/* The type  mbedtls_transparent_test_driver_xof_operation_t is only
 * defined since XOF support was added in TF-PSA-Crypto 1.1.0. */
#if defined(PSA_ALG_CATEGORY_XOF)

typedef struct {
    /* If not PSA_SUCCESS, return this error code instead of processing the
     * function call. */
    psa_status_t forced_status;
    /* Count the amount of times xof driver entry points are called. */
    unsigned long hits;
    /* Status returned by the last xof driver entry point call. */
    psa_status_t driver_status;
} mbedtls_test_driver_xof_hooks_t;

#define MBEDTLS_TEST_DRIVER_XOF_INIT { 0, 0, 0 }
static inline mbedtls_test_driver_xof_hooks_t
mbedtls_test_driver_xof_hooks_init(void)
{
    const mbedtls_test_driver_xof_hooks_t v = MBEDTLS_TEST_DRIVER_XOF_INIT;
    return v;
}

extern mbedtls_test_driver_xof_hooks_t mbedtls_test_driver_xof_hooks;

psa_status_t mbedtls_test_transparent_xof_setup(
    mbedtls_transparent_test_driver_xof_operation_t *operation,
    psa_algorithm_t alg);

psa_status_t mbedtls_test_transparent_xof_set_context(
    mbedtls_transparent_test_driver_xof_operation_t *operation,
    const uint8_t *context, size_t context_length);

psa_status_t mbedtls_test_transparent_xof_update(
    mbedtls_transparent_test_driver_xof_operation_t *operation,
    const uint8_t *input, size_t input_length);

psa_status_t mbedtls_test_transparent_xof_output(
    mbedtls_transparent_test_driver_xof_operation_t *operation,
    uint8_t *output, size_t output_length);

psa_status_t mbedtls_test_transparent_xof_abort(
    mbedtls_transparent_test_driver_xof_operation_t *operation);

#endif /* PSA_ALG_CATEGORY_XOF */
#endif /* PSA_CRYPTO_DRIVER_TEST */
#endif /* PSA_CRYPTO_TEST_DRIVERS_XOF_H */
