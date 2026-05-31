/**
 * \file macros.h
 *
 * \brief   This file contains generic macros for the purpose of testing.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef TEST_MACROS_H
#define TEST_MACROS_H

#include "build_info.h"

#include <stdlib.h>

#include "mbedtls/platform.h"

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
#include "mbedtls/memory_buffer_alloc.h"
#endif

#if defined(TF_PSA_CRYPTO_VERSION_NUMBER)
#include "tf_psa_crypto_common.h"
#else
#include "common.h"
#endif

/**
 * \brief   This macro tests the expression passed to it as a test step or
 *          individual test in a test case.
 *
 *          It allows a library function to return a value and return an error
 *          code that can be tested.
 *
 *          Failing the test means:
 *          - Mark this test case as failed.
 *          - Print a message identifying the failure.
 *          - Jump to the \c exit label.
 *
 *          This macro expands to an instruction, not an expression.
 *          It may jump to the \c exit label.
 *
 * \param   TEST    The test expression to be tested.
 */
#define TEST_ASSERT(TEST)                                 \
    do {                                                    \
        if (!(TEST))                                       \
        {                                                    \
            mbedtls_test_fail( #TEST, __LINE__, __FILE__);   \
            goto exit;                                        \
        }                                                    \
    } while (0)

/** \brief Evaluate an integer expression. If the value is 0 (i.e. false),
 *         mark the test case as failed and display errno.
 *
 * This is intended for functions that follow the Unix API convention of
 * returning a particular value (often -1) and setting errno on failure,
 * e.g. `TEST_ASSERT_ERRNO(open(...) != -1)`.
 */
#define TEST_ASSERT_ERRNO(expr)                                         \
    do {                                                                \
        if (!(expr)) {                                                  \
            mbedtls_test_fail_errno(#expr, __LINE__, __FILE__);         \
            goto exit;                                                  \
        }                                                               \
    } while (0)

/** This macro asserts fails the test with given output message.
 *
 * \param   MESSAGE The message to be outputed on assertion
 */
#define TEST_FAIL(MESSAGE)                           \
    do {                                                  \
        mbedtls_test_fail(MESSAGE, __LINE__, __FILE__);   \
        goto exit;                                        \
    } while (0)

/** Evaluate two integer expressions and fail the test case if they have
 * different values.
 *
 * The two expressions should have the same signedness, otherwise the
 * comparison is not meaningful if the signed value is negative.
 *
 * \param expr1     An integral-typed expression to evaluate.
 * \param expr2     Another integral-typed expression to evaluate.
 */
#define TEST_EQUAL(expr1, expr2)                                      \
    do {                                                                \
        if (!mbedtls_test_equal( #expr1 " == " #expr2, __LINE__, __FILE__, \
                                 (unsigned long long) (expr1), (unsigned long long) (expr2)))                      \
        goto exit;                                                  \
    } while (0)

/** Evaluate two unsigned integer expressions and fail the test case
 * if they are not in increasing order (left <= right).
 *
 * \param expr1     An integral-typed expression to evaluate.
 * \param expr2     Another integral-typed expression to evaluate.
 */
#define TEST_LE_U(expr1, expr2)                                       \
    do {                                                                \
        if (!mbedtls_test_le_u( #expr1 " <= " #expr2, __LINE__, __FILE__, \
                                expr1, expr2))                      \
        goto exit;                                                  \
    } while (0)

/** Evaluate two signed integer expressions and fail the test case
 * if they are not in increasing order (left <= right).
 *
 * \param expr1     An integral-typed expression to evaluate.
 * \param expr2     Another integral-typed expression to evaluate.
 */
#define TEST_LE_S(expr1, expr2)                                       \
    do {                                                                \
        if (!mbedtls_test_le_s( #expr1 " <= " #expr2, __LINE__, __FILE__, \
                                expr1, expr2))                      \
        goto exit;                                                  \
    } while (0)

/** Allocate memory dynamically and fail the test case if this fails.
 * The allocated memory will be filled with zeros.
 *
 * You must set \p pointer to \c NULL before calling this macro and
 * put `mbedtls_free(pointer)` in the test's cleanup code.
 *
 * If \p item_count is zero, the resulting \p pointer will be \c NULL.
 * This is usually what we want in tests since API functions are
 * supposed to accept null pointers when a buffer size is zero.
 *
 * This macro expands to an instruction, not an expression.
 * It may jump to the \c exit label.
 *
 * \param pointer    An lvalue where the address of the allocated buffer
 *                   will be stored.
 *                   This expression may be evaluated multiple times.
 * \param item_count Number of elements to allocate.
 *                   This expression may be evaluated multiple times.
 *
 */
#define TEST_CALLOC(pointer, item_count)                    \
    do {                                                    \
        TEST_ASSERT((pointer) == NULL);                     \
        if ((item_count) != 0) {                            \
            (pointer) = mbedtls_calloc((item_count),        \
                                       sizeof(*(pointer))); \
            TEST_ASSERT((pointer) != NULL);                 \
        }                                                   \
    } while (0)

/** Allocate memory dynamically and fail the test case if this fails.
 * The allocated memory will be filled with zeros.
 *
 * You must set \p pointer to \c NULL before calling this macro and
 * put `mbedtls_free(pointer)` in the test's cleanup code.
 *
 * If \p item_count is zero, the resulting \p pointer will not be \c NULL.
 *
 * This macro expands to an instruction, not an expression.
 * It may jump to the \c exit label.
 *
 * \param pointer    An lvalue where the address of the allocated buffer
 *                   will be stored.
 *                   This expression may be evaluated multiple times.
 * \param item_count Number of elements to allocate.
 *                   This expression may be evaluated multiple times.
 *
 * Note: if passing size 0, mbedtls_calloc may return NULL. In this case,
 * we reattempt to allocate with the smallest possible buffer to assure a
 * non-NULL pointer.
 */
#define TEST_CALLOC_NONNULL(pointer, item_count)            \
    do {                                                    \
        TEST_ASSERT((pointer) == NULL);                     \
        (pointer) = mbedtls_calloc((item_count),            \
                                   sizeof(*(pointer)));     \
        if (((pointer) == NULL) && ((item_count) == 0)) {   \
            (pointer) = mbedtls_calloc(1, 1);               \
        }                                                   \
        TEST_ASSERT((pointer) != NULL);                     \
    } while (0)

/* For backwards compatibility */
#define ASSERT_ALLOC(pointer, item_count) TEST_CALLOC(pointer, item_count)

/** Allocate memory dynamically. If the allocation fails, skip the test case.
 *
 * This macro behaves like #TEST_CALLOC, except that if the allocation
 * fails, it marks the test as skipped rather than failed.
 */
#define TEST_CALLOC_OR_SKIP(pointer, item_count)            \
    do {                                                    \
        TEST_ASSERT((pointer) == NULL);                     \
        if ((item_count) != 0) {                            \
            (pointer) = mbedtls_calloc((item_count),        \
                                       sizeof(*(pointer))); \
            TEST_ASSUME((pointer) != NULL);                 \
        }                                                   \
    } while (0)

/* For backwards compatibility */
#define ASSERT_ALLOC_WEAK(pointer, item_count) TEST_CALLOC_OR_SKIP(pointer, item_count)

/** Compare two buffers and fail the test case if they differ.
 *
 * This macro expands to an instruction, not an expression.
 * It may jump to the \c exit label.
 *
 * \param p1        Pointer to the start of the first buffer.
 * \param size1     Size of the first buffer in bytes.
 *                  This expression may be evaluated multiple times.
 * \param p2        Pointer to the start of the second buffer.
 * \param size2     Size of the second buffer in bytes.
 *                  This expression may be evaluated multiple times.
 */
#define TEST_MEMORY_COMPARE(p1, size1, p2, size2)              \
    do {                                                       \
        TEST_EQUAL((size1), (size2));                          \
        if ((size1) != 0) {                                    \
            TEST_ASSERT(memcmp((p1), (p2), (size1)) == 0);     \
        }                                                      \
    } while (0)

/* For backwards compatibility */
#define ASSERT_COMPARE(p1, size1, p2, size2) TEST_MEMORY_COMPARE(p1, size1, p2, size2)

/**
 * \brief   This macro tests the expression passed to it and skips the
 *          running test if it doesn't evaluate to 'true'.
 *
 * \param   TEST    The test expression to be tested.
 */
#define TEST_ASSUME(TEST)                                 \
    do {                                                    \
        if (!(TEST))                                      \
        {                                                   \
            mbedtls_test_skip( #TEST, __LINE__, __FILE__); \
            goto exit;                                      \
        }                                                   \
    } while (0)

#define TEST_HELPER_ASSERT(a) if (!(a))                          \
    {                                                                   \
        mbedtls_fprintf(stderr, "Assertion Failed at %s:%d - %s\n",    \
                        __FILE__, __LINE__, #a);              \
        mbedtls_exit(1);                                              \
    }

/** Return the smaller of two values.
 *
 * \param x         An integer-valued expression without side effects.
 * \param y         An integer-valued expression without side effects.
 *
 * \return The smaller of \p x and \p y.
 */
#define MIN(x, y) ((x) < (y) ? (x) : (y))

/** Return the larger of two values.
 *
 * \param x         An integer-valued expression without side effects.
 * \param y         An integer-valued expression without side effects.
 *
 * \return The larger of \p x and \p y.
 */
#define MAX(x, y) ((x) > (y) ? (x) : (y))

#endif /* TEST_MACROS_H */
