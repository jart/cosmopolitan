/*
 * Test driver for key agreement functions.
 */
/*  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef PSA_CRYPTO_TEST_DRIVERS_KEY_AGREEMENT_H
#define PSA_CRYPTO_TEST_DRIVERS_KEY_AGREEMENT_H

#include "mbedtls/build_info.h"

#if defined(PSA_CRYPTO_DRIVER_TEST)
#include "test_driver_common.h"

#include <psa/crypto_driver_common.h>

typedef struct {
    /* If non-null, on success, copy this to the output. */
    void *forced_output;
    size_t forced_output_length;
    /* If not PSA_SUCCESS, return this error code instead of processing the
     * function call. */
    psa_status_t forced_status;
    /* Count the amount of times one of the signature driver functions is called. */
    unsigned long hits;
} mbedtls_test_driver_key_agreement_hooks_t;

#define MBEDTLS_TEST_DRIVER_KEY_AGREEMENT_INIT { NULL, 0, PSA_SUCCESS, 0 }
static inline mbedtls_test_driver_key_agreement_hooks_t
mbedtls_test_driver_key_agreement_hooks_init(void)
{
    const mbedtls_test_driver_key_agreement_hooks_t
        v = MBEDTLS_TEST_DRIVER_KEY_AGREEMENT_INIT;
    return v;
}

extern mbedtls_test_driver_key_agreement_hooks_t
    mbedtls_test_driver_key_agreement_hooks;

psa_status_t mbedtls_test_transparent_key_agreement(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *peer_key,
    size_t peer_key_length,
    uint8_t *shared_secret,
    size_t shared_secret_size,
    size_t *shared_secret_length);

psa_status_t mbedtls_test_opaque_key_agreement(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    psa_algorithm_t alg,
    const uint8_t *peer_key,
    size_t peer_key_length,
    uint8_t *shared_secret,
    size_t shared_secret_size,
    size_t *shared_secret_length);

#endif /*PSA_CRYPTO_DRIVER_TEST */
#endif /* PSA_CRYPTO_TEST_DRIVERS_KEY_AGREEMENT_H */
