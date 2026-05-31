/** Helper functions for memory poisoning in tests.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */
#include "test_common.h"

#include <test/memory.h>
#include <test/psa_memory_poisoning_wrappers.h>

#include "psa_crypto_invasive.h"

#if defined(MBEDTLS_TEST_HOOKS)  && defined(MBEDTLS_PSA_CRYPTO_C) \
    && defined(MBEDTLS_TEST_MEMORY_CAN_POISON)

void mbedtls_poison_test_hooks_setup(void)
{
    psa_input_pre_copy_hook = mbedtls_test_memory_unpoison;
    psa_input_post_copy_hook = mbedtls_test_memory_poison;
    psa_output_pre_copy_hook = mbedtls_test_memory_unpoison;
    psa_output_post_copy_hook = mbedtls_test_memory_poison;
}

void mbedtls_poison_test_hooks_teardown(void)
{
    psa_input_pre_copy_hook = NULL;
    psa_input_post_copy_hook = NULL;
    psa_output_pre_copy_hook = NULL;
    psa_output_post_copy_hook = NULL;
}

#endif /* MBEDTLS_TEST_HOOKS && MBEDTLS_PSA_CRYPTO_C &&
          MBEDTLS_TEST_MEMORY_CAN_POISON */
