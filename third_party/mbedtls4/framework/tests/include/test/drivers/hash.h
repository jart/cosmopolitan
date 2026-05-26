/*
 * Test driver for hash driver entry points.
 */
/*  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef PSA_CRYPTO_TEST_DRIVERS_HASH_H
#define PSA_CRYPTO_TEST_DRIVERS_HASH_H

#include "mbedtls/build_info.h"

#if defined(PSA_CRYPTO_DRIVER_TEST)
#include "test_driver_common.h"

#include <psa/crypto_driver_common.h>

typedef struct {
    /* If not PSA_SUCCESS, return this error code instead of processing the
     * function call. */
    psa_status_t forced_status;
    /* Count the amount of times hash driver entry points are called. */
    unsigned long hits;
    /* Status returned by the last hash driver entry point call. */
    psa_status_t driver_status;
} mbedtls_test_driver_hash_hooks_t;

#define MBEDTLS_TEST_DRIVER_HASH_INIT { 0, 0, 0 }
static inline mbedtls_test_driver_hash_hooks_t
mbedtls_test_driver_hash_hooks_init(void)
{
    const mbedtls_test_driver_hash_hooks_t v = MBEDTLS_TEST_DRIVER_HASH_INIT;
    return v;
}

extern mbedtls_test_driver_hash_hooks_t mbedtls_test_driver_hash_hooks;

psa_status_t mbedtls_test_transparent_hash_compute(
    psa_algorithm_t alg,
    const uint8_t *input, size_t input_length,
    uint8_t *hash, size_t hash_size, size_t *hash_length);

psa_status_t mbedtls_test_transparent_hash_setup(
    mbedtls_transparent_test_driver_hash_operation_t *operation,
    psa_algorithm_t alg);

psa_status_t mbedtls_test_transparent_hash_clone(
    const mbedtls_transparent_test_driver_hash_operation_t *source_operation,
    mbedtls_transparent_test_driver_hash_operation_t *target_operation);

psa_status_t mbedtls_test_transparent_hash_update(
    mbedtls_transparent_test_driver_hash_operation_t *operation,
    const uint8_t *input,
    size_t input_length);

psa_status_t mbedtls_test_transparent_hash_finish(
    mbedtls_transparent_test_driver_hash_operation_t *operation,
    uint8_t *hash,
    size_t hash_size,
    size_t *hash_length);

psa_status_t mbedtls_test_transparent_hash_abort(
    mbedtls_transparent_test_driver_hash_operation_t *operation);

#endif /* PSA_CRYPTO_DRIVER_TEST */
#endif /* PSA_CRYPTO_TEST_DRIVERS_HASH_H */
