/*
 * Helper functions for tests that use any PSA API.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef PSA_HELPERS_H
#define PSA_HELPERS_H

#include "build_info.h"

#if defined(MBEDTLS_PSA_CRYPTO_SPM)
#include "spm/psa_defs.h"
#endif

/** Evaluate an expression and fail the test case if it returns an error.
 *
 * \param expr      The expression to evaluate. This is typically a call
 *                  to a \c psa_xxx function that returns a value of type
 *                  #psa_status_t.
 */
#define PSA_ASSERT(expr) TEST_EQUAL((expr), PSA_SUCCESS)

#endif /* PSA_HELPERS_H */
