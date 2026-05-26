/*
 * Insecure but standalone implementation of mbedtls_psa_external_get_random().
 * Only for use in tests!
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef FAKE_EXTERNAL_RNG_FOR_TEST_H
#define FAKE_EXTERNAL_RNG_FOR_TEST_H

#include "build_info.h"

#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
/** Enable the insecure implementation of mbedtls_psa_external_get_random().
 *
 * The insecure implementation of mbedtls_psa_external_get_random() is
 * disabled by default.
 *
 * When MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG is enabled and the test
 * helpers are linked into a program, you must enable this before running any
 * code that uses the PSA subsystem to generate random data (including internal
 * random generation for purposes such as blinding when the random generation
 * is routed through PSA).
 *
 * You can enable and disable it at any time, regardless of the state
 * of the PSA subsystem. You may disable it temporarily to simulate a
 * depleted entropy source.
 */
void mbedtls_test_enable_insecure_external_rng(void);

/** Disable the insecure implementation of mbedtls_psa_external_get_random().
 *
 * See mbedtls_test_enable_insecure_external_rng().
 */
void mbedtls_test_disable_insecure_external_rng(void);
#endif /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */

#if defined(MBEDTLS_PSA_DRIVER_GET_ENTROPY)

#include <mbedtls/platform.h>

/* In the following there are some helper functions which allow tests to
 * modify the behavior of the mbedtls_platform_get_entropy() implementation
 * provided for test purposes.
 * The following features can be controlled:
 * - force a return value;
 * - force the amount of bytes returned on each call;
 * - force amount of entroy returned on each call;
 * - get the number of times the callback has been called.
 */

/* Disable all forced values */
void mbedtls_test_platform_get_entropy_reset(void);

/* Force a failure on mbedtls_platform_get_entropy() as follows
 * - val = 1 --> returns MBEDTLS_ERR_ENTROPY_SOURCE_FAILED.
 * - val = 0 --> works normally (other forced values apply if set).
 */
void mbedtls_test_platform_get_entropy_set_force_failure(int val);

/* If `val < SIZE_MAX` then forcedly limit the amount of data returned from
 * mbedtls_platform_get_entropy() to the provided `val` value.
 */
void mbedtls_test_platform_get_entropy_set_output_len(size_t val);

/* If `val < SIZE_MAX` then forcedly limit the amount of returned entropy from
 * mbedtls_platform_get_entropy() to the provided `val` value.
 */
void mbedtls_test_platform_get_entropy_set_entropy_content(size_t val);

/* Return the number of times mbedtls_platform_get_entropy() was called. */
size_t mbedtls_test_platform_get_entropy_get_call_count(void);

#endif /* MBEDTLS_PSA_DRIVER_GET_ENTROPY */

#endif /* FAKE_EXTERNAL_RNG_FOR_TEST_H */
