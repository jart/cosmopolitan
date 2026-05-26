/** Support for path tracking in optionally safe bignum functions
 *
 * The functions are called when an optionally safe path is taken and logs it with a single
 * variable. This variable is at any time in one of three states:
 *      - MBEDTLS_MPI_IS_TEST: No optionally safe path has been taken since the last reset
 *      - MBEDTLS_MPI_IS_SECRET: Only safe paths were teken since the last reset
 *      - MBEDTLS_MPI_IS_PUBLIC: At least one unsafe path has been taken since the last reset
 *
 * Use a simple global variable to track execution path. Making it work with multithreading
 * isn't worth the effort as multithreaded tests add little to no value here.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef BIGNUM_CODEPATH_CHECK_H
#define BIGNUM_CODEPATH_CHECK_H

#include "build_info.h"

#include "bignum_core.h"

#if defined(MBEDTLS_TEST_HOOKS) && !defined(MBEDTLS_THREADING_C)

extern int mbedtls_codepath_check;

/**
 * \brief         Setup the codepath test hooks used by optionally safe bignum functions to signal
 *                the path taken.
 */
void mbedtls_codepath_test_hooks_setup(void);

/**
 * \brief         Teardown the codepath test hooks used by optionally safe bignum functions to
 *                signal the path taken.
 */
void mbedtls_codepath_test_hooks_teardown(void);

/**
 * \brief         Reset the state of the codepath to the initial state.
 */
static inline void mbedtls_codepath_reset(void)
{
    mbedtls_codepath_check = MBEDTLS_MPI_IS_TEST;
}

/** Check the codepath taken and fail if it doesn't match.
 *
 * When a function returns with an error, it can do so before reaching any interesting codepath. The
 * same can happen if a parameter to the function is zero. In these cases we need to allow
 * the codepath tracking variable to still have its initial "not set" value.
 *
 * This macro expands to an instruction, not an expression.
 * It may jump to the \c exit label.
 *
 * \param path      The expected codepath.
 *                  This expression may be evaluated multiple times.
 * \param ret       The expected return value.
 * \param E         The MPI parameter that can cause shortcuts.
 */
#define ASSERT_BIGNUM_CODEPATH(path, ret, E)                            \
    do {                                                                \
        if ((ret) != 0 || (E).n == 0) {                                 \
            TEST_ASSERT(mbedtls_codepath_check == (path) ||             \
                        mbedtls_codepath_check == MBEDTLS_MPI_IS_TEST); \
        } else {                                                        \
            TEST_EQUAL(mbedtls_codepath_check, (path));                 \
        }                                                               \
    } while (0)

/** Check the codepath taken and fail if it doesn't match.
 *
 * When a function returns with an error, it can do so before reaching any interesting codepath. In
 * this case we need to allow the codepath tracking variable to still have its
 * initial "not set" value.
 *
 * This macro expands to an instruction, not an expression.
 * It may jump to the \c exit label.
 *
 * \param path      The expected codepath.
 *                  This expression may be evaluated multiple times.
 * \param ret       The expected return value.
 */
#define ASSERT_RSA_CODEPATH(path, ret)                                  \
    do {                                                                \
        if ((ret) != 0) {                                               \
            TEST_ASSERT(mbedtls_codepath_check == (path) ||             \
                        mbedtls_codepath_check == MBEDTLS_MPI_IS_TEST); \
        } else {                                                        \
            TEST_EQUAL(mbedtls_codepath_check, (path));                 \
        }                                                               \
    } while (0)
#endif /* MBEDTLS_TEST_HOOKS && !MBEDTLS_THREADING_C */

#endif /* BIGNUM_CODEPATH_CHECK_H */
