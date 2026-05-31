/** \file fake_external_rng_for_test.c
 *
 * Helper functions to test external functions:
 * - mbedtls_psa_external_get_random()
 * - mbedtls_platform_get_entropy()
 *
 * These functions are provided only for test purposes and they should not be
 * used for production.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "test_common.h"

#include <test/fake_external_rng_for_test.h>

#if defined(MBEDTLS_PSA_DRIVER_GET_ENTROPY)
#include <psa/crypto_driver_random.h>
#endif

#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)

#include <test/random.h>
#include <psa/crypto.h>

static int test_insecure_external_rng_enabled = 0;

void mbedtls_test_enable_insecure_external_rng(void)
{
    test_insecure_external_rng_enabled = 1;
}

void mbedtls_test_disable_insecure_external_rng(void)
{
    test_insecure_external_rng_enabled = 0;
}

psa_status_t mbedtls_psa_external_get_random(
    mbedtls_psa_external_random_context_t *context,
    uint8_t *output, size_t output_size, size_t *output_length)
{
    (void) context;

    if (!test_insecure_external_rng_enabled) {
        return PSA_ERROR_INSUFFICIENT_ENTROPY;
    }

    /* This implementation is for test purposes only!
     * Use the libc non-cryptographic random generator. */
    mbedtls_test_rnd_std_rand(NULL, output, output_size);
    *output_length = output_size;
    return PSA_SUCCESS;
}

#endif /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */

#if defined(MBEDTLS_PSA_DRIVER_GET_ENTROPY)

#include <test/random.h>

#if !defined(MBEDTLS_VERSION_MAJOR) || MBEDTLS_VERSION_MAJOR >= 4
#include <mbedtls/private/entropy.h>
#else
#include <mbedtls/entropy.h>
#endif

static int platform_get_entropy_force_failure;
static size_t platform_get_entropy_forced_entropy_content = SIZE_MAX;
static size_t platform_get_entropy_forced_output_len = SIZE_MAX;
static size_t platform_get_entropy_call_count;

void mbedtls_test_platform_get_entropy_reset(void)
{
    platform_get_entropy_call_count = 0;
    platform_get_entropy_force_failure = 0;
    platform_get_entropy_forced_entropy_content = SIZE_MAX;
    platform_get_entropy_forced_output_len = SIZE_MAX;
}

void mbedtls_test_platform_get_entropy_set_force_failure(int val)
{
    platform_get_entropy_force_failure = (val != 0);
}

void mbedtls_test_platform_get_entropy_set_output_len(size_t val)
{
    platform_get_entropy_forced_output_len = val;
}

void mbedtls_test_platform_get_entropy_set_entropy_content(size_t val)
{
    platform_get_entropy_forced_entropy_content = val;
}

size_t mbedtls_test_platform_get_entropy_get_call_count(void)
{
    return platform_get_entropy_call_count;
}

static int fake_get_entropy(unsigned char *output, size_t output_size,
                            size_t *estimate_bits)
{
    platform_get_entropy_call_count++;

    /* Return a failure if we were requested to. */
    if (platform_get_entropy_force_failure != 0) {
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }

    /* Return less data than requested if we were requested to. */
    if (platform_get_entropy_forced_output_len < SIZE_MAX) {
        /* Prevent buffer overrun */
        if (platform_get_entropy_forced_output_len > output_size) {
            return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
        }
        output_size = platform_get_entropy_forced_output_len;
    }

    mbedtls_test_rnd_std_rand(NULL, output, output_size);

    if (estimate_bits != NULL) {
        if (platform_get_entropy_forced_entropy_content < SIZE_MAX) {
            *estimate_bits = platform_get_entropy_forced_entropy_content;
        } else {
            *estimate_bits = output_size * 8;
        }
    }

    return 0;
}

#endif /* MBEDTLS_PSA_DRIVER_GET_ENTROPY */

#if defined(MBEDTLS_PSA_DRIVER_GET_ENTROPY)
int mbedtls_platform_get_entropy(psa_driver_get_entropy_flags_t flags,
                                 size_t *estimate_bits,
                                 unsigned char *output, size_t output_size)
{
    /* We don't implement any flags yet. */
    if (flags != 0) {
        return PSA_ERROR_NOT_SUPPORTED;
    }

    int ret = fake_get_entropy(output, output_size, estimate_bits);
    return ret;
}
#endif /* MBEDTLS_PSA_DRIVER_GET_ENTROPY */
