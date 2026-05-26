/** Support for path tracking in optionally safe bignum functions
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "test_common.h"
#include "test/bignum_codepath_check.h"
#include "bignum_core_invasive.h"

#if defined(MBEDTLS_TEST_HOOKS) && !defined(MBEDTLS_THREADING_C)
int mbedtls_codepath_check = MBEDTLS_MPI_IS_TEST;

static void mbedtls_codepath_take_safe(void)
{
    if (mbedtls_codepath_check == MBEDTLS_MPI_IS_TEST) {
        mbedtls_codepath_check = MBEDTLS_MPI_IS_SECRET;
    }
}

static void mbedtls_codepath_take_unsafe(void)
{
    mbedtls_codepath_check = MBEDTLS_MPI_IS_PUBLIC;
}

void mbedtls_codepath_test_hooks_setup(void)
{
    mbedtls_safe_codepath_hook = mbedtls_codepath_take_safe;
    mbedtls_unsafe_codepath_hook = mbedtls_codepath_take_unsafe;
}

void mbedtls_codepath_test_hooks_teardown(void)
{
    mbedtls_safe_codepath_hook = NULL;
    mbedtls_unsafe_codepath_hook = NULL;
}

#endif /* MBEDTLS_TEST_HOOKS && !MBEDTLS_THREADING_C */
