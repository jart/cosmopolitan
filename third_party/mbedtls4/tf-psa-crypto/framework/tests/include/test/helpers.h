/**
 * \file helpers.h
 *
 * \brief   This file contains the prototypes of helper functions for the
 *          purpose of testing.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include "build_info.h"

#if defined(__SANITIZE_ADDRESS__) /* gcc -fsanitize=address */
#  define MBEDTLS_TEST_HAVE_ASAN
#endif
#if defined(__SANITIZE_THREAD__) /* gcc -fsanitize-thread */
#  define MBEDTLS_TEST_HAVE_TSAN
#endif

#if defined(__has_feature)
#  if __has_feature(address_sanitizer) /* clang -fsanitize=address */
#    define MBEDTLS_TEST_HAVE_ASAN
#  endif
#  if __has_feature(memory_sanitizer) /* clang -fsanitize=memory */
#    define MBEDTLS_TEST_HAVE_MSAN
#  endif
#  if __has_feature(thread_sanitizer) /* clang -fsanitize=thread */
#    define MBEDTLS_TEST_HAVE_TSAN
#  endif
#endif

#include "test/threading_helpers.h"

#if defined(MBEDTLS_TEST_MUTEX_USAGE)
#include "mbedtls/threading.h"
#endif

#include "mbedtls/platform.h"

#include <stddef.h>
#include <stdint.h>

#if defined(MBEDTLS_BIGNUM_C)

#if !defined(MBEDTLS_VERSION_MAJOR) || MBEDTLS_VERSION_MAJOR >= 4
#include "mbedtls/private/bignum.h"
#else
#include "mbedtls/bignum.h"
#endif

#endif

/** The type of test case arguments that contain binary data. */
typedef struct data_tag {
    uint8_t *x;
    uint32_t    len;
} data_t;

typedef enum {
    MBEDTLS_TEST_RESULT_SUCCESS = 0,
    MBEDTLS_TEST_RESULT_FAILED,
    MBEDTLS_TEST_RESULT_SKIPPED
} mbedtls_test_result_t;

#define MBEDTLS_TEST_LINE_LENGTH 76

typedef struct {
    mbedtls_test_result_t result;
    const char *test;
    const char *filename;
    int line_no;
    unsigned long step;
    char line1[MBEDTLS_TEST_LINE_LENGTH];
    char line2[MBEDTLS_TEST_LINE_LENGTH];
#if defined(MBEDTLS_TEST_MUTEX_USAGE)
    const char *mutex_usage_error;
#endif
#if defined(MBEDTLS_BIGNUM_C)
    unsigned case_uses_negative_0;
#endif
}
mbedtls_test_info_t;

/**
 * \brief           Get the current test result status
 *
 * \return          The current test result status
 */
mbedtls_test_result_t mbedtls_test_get_result(void);

/**
 * \brief           Get the current test name/description
 *
 * \return          The current test name/description
 */
const char *mbedtls_test_get_test(void);

/**
 * \brief           Get the current test filename
 *
 * \return          The current test filename
 */
const char *mbedtls_get_test_filename(void);

/**
 * \brief           Get the current test file line number (for failure / skip)
 *
 * \return          The current test file line number (for failure / skip)
 */
int mbedtls_test_get_line_no(void);

/**
 * \brief           Increment the current test step.
 *
 * \note            It is not recommended for multiple threads to call this
 *                  function concurrently - whilst it is entirely thread safe,
 *                  the order of calls to this function can obviously not be
 *                  ensured, so unexpected results may occur.
 */
void mbedtls_test_increment_step(void);

/**
 * \brief           Get the current test step
 *
 * \return          The current test step
 */
unsigned long mbedtls_test_get_step(void);

/**
 * \brief           Get the current test line buffer 1
 *
 * \param line      Buffer of minimum size \c MBEDTLS_TEST_LINE_LENGTH,
 *                  which will have line buffer 1 copied to it.
 */
void mbedtls_test_get_line1(char *line);

/**
 * \brief           Get the current test line buffer 2
 *
 * \param line      Buffer of minimum size \c MBEDTLS_TEST_LINE_LENGTH,
 *                  which will have line buffer 1 copied to it.
 */
void mbedtls_test_get_line2(char *line);

/**
 * \brief           Get a copy of the test result information.
 *
 * \param[out] out  On output, contains a copy of the current test info.
 */
void mbedtls_test_info_save(mbedtls_test_info_t *out);

/**
 * \brief           Overwrite the test result information.
 *                  This is intended for some unusual scenarios.
 *                  You probably shouldn't use this in a test function.
 *
 * \param[in] replacement
 *                  The test info to use instead of the current one.
 *                  The function copies the data, so the pointer does
 *                  not need to be valid after this function returns.
 */
void mbedtls_test_info_overwrite(const mbedtls_test_info_t *replacement);

#if defined(MBEDTLS_TEST_MUTEX_USAGE)
/**
 * \brief           Get the current mutex usage error message
 *
 * \return          The current mutex error message (may be NULL if no error)
 */
const char *mbedtls_test_get_mutex_usage_error(void);

/**
 * \brief           Set the current mutex usage error message
 *
 * \note            This will only set the mutex error message if one has not
 *                  already been set, or if we are clearing the message (msg is
 *                  NULL)
 *
 * \param msg       Error message to set (can be NULL to clear)
 */
void mbedtls_test_set_mutex_usage_error(const char *msg);
#endif

/**
 * \brief           Check whether the given buffer is all-bits-zero.
 *
 * \param[in] buf   Pointer to the buffer to check.
 * \param size      Buffer size in bytes.
 *
 * \retval 0        The given buffer has a nonzero byte.
 * \retval 1        The given buffer is all-bits-zero (this includes the case
 *                  of an empty buffer).
 */
int mbedtls_test_buffer_is_all_zero(const uint8_t *buf, size_t size);

/** Check whether the object at the given address is all-bits-zero.
 *
 * \param[in] ptr   A pointer to the object to check.
 *                  This macro parameter may be evaluated more than once.
 *
 * \retval 0        The given object has a nonzero byte.
 * \retval 1        The given object is all-bits-zero (this includes the case
 *                  of an empty buffer).
 */
#define MBEDTLS_TEST_OBJECT_IS_ALL_ZERO(ptr)    \
    (mbedtls_test_buffer_is_all_zero((const uint8_t *) (ptr), sizeof(*(ptr))))

#if defined(MBEDTLS_BIGNUM_C)

/**
 * \brief           Get whether the current test is a bignum test that uses
 *                  negative zero.
 *
 * \return          non zero if the current test uses bignum negative zero.
 */
unsigned mbedtls_test_get_case_uses_negative_0(void);

/**
 * \brief           Indicate that the current test uses bignum negative zero.
 *
 * \note            This function is called if the current test case had an
 *                  input parsed with mbedtls_test_read_mpi() that is a negative
 *                  0 (`"-"`, `"-0"`, `"-00"`, etc., constructing a result with
 *                  the sign bit set to -1 and the value being all-limbs-0,
 *                  which is not a valid representation in #mbedtls_mpi but is
 *                  tested for robustness). *
 */
void  mbedtls_test_increment_case_uses_negative_0(void);
#endif

int mbedtls_test_platform_setup(void);
void mbedtls_test_platform_teardown(void);

/**
 * \brief           Record the current test case as a failure.
 *
 *                  This function can be called directly however it is usually
 *                  called via macros such as TEST_ASSERT, TEST_EQUAL,
 *                  PSA_ASSERT, etc...
 *
 * \note            If the test case was already marked as failed, calling
 *                  `mbedtls_test_fail( )` again will not overwrite any
 *                  previous information about the failure.
 *
 * \param test      Description of the failure or assertion that failed. This
 *                  MUST be a string literal.
 * \param line_no   Line number where the failure originated.
 * \param filename  Filename where the failure originated.
 */
void mbedtls_test_fail(const char *test, int line_no, const char *filename);

/**
 * \brief           Record the current test case as a failure
 *                  and show the value of errno.
 *
 *                  This function is usually called via #TEST_ASSERT_ERRNO.
 *
 * \param test      Description of the failure or assertion that failed. This
 *                  MUST be a string literal.
 * \param line_no   Line number where the failure originated.
 * \param filename  Filename where the failure originated.
 */
void mbedtls_test_fail_errno(const char *test, int line_no, const char *filename);

/**
 * \brief           Record the current test case as skipped.
 *
 *                  This function can be called directly however it is usually
 *                  called via the TEST_ASSUME macro.
 *
 * \param test      Description of the assumption that caused the test case to
 *                  be skipped. This MUST be a string literal.
 * \param line_no   Line number where the test case was skipped.
 * \param filename  Filename where the test case was skipped.
 */
void mbedtls_test_skip(const char *test, int line_no, const char *filename);

/**
 * \brief           Set the test step number for failure reports.
 *
 *                  Call this function to display "step NNN" in addition to the
 *                  line number and file name if a test fails. Typically the
 *                  "step number" is the index of a for loop but it can be
 *                  whatever you want.
 *
 * \note            It is not recommended for multiple threads to call this
 *                  function concurrently - whilst it is entirely thread safe,
 *                  the order of calls to this function can obviously not be
 *                  ensured, so unexpected results may occur.
 *
 * \param step  The step number to report.
 */
void mbedtls_test_set_step(unsigned long step);

/**
 * \brief           Reset mbedtls_test_info to a ready/starting state.
 */
void mbedtls_test_info_reset(void);

#ifdef MBEDTLS_TEST_MUTEX_USAGE
/**
 * \brief           Get the test info data mutex.
 *
 * \note            This is designed only to be used by threading_helpers to
 *                  avoid a deadlock, not for general access to this mutex.
 *
 * \return          The test info data mutex.
 */
mbedtls_threading_mutex_t *mbedtls_test_get_info_mutex(void);

#endif /* MBEDTLS_TEST_MUTEX_USAGE */

/**
 * \brief Record the current test case as a failure if two integers
 *                  have a different value.
 *
 *                  This function is usually called via the macro
 *                  #TEST_EQUAL.
 *
 * \param test      Description of the failure or assertion that failed. This
 *                  MUST be a string literal. This normally has the form
 *                  "EXPR1 == EXPR2" where EXPR1 has the value \p value1
 *                  and EXPR2 has the value \p value2.
 * \param line_no   Line number where the failure originated.
 * \param filename  Filename where the failure originated.
 * \param value1    The first value to compare.
 * \param value2    The second value to compare.
 *
 * \return          \c 1 if the values are equal, otherwise \c 0.
 */
int mbedtls_test_equal(const char *test, int line_no, const char *filename,
                       unsigned long long value1, unsigned long long value2);

/**
 * \brief           Record the current test case as a failure based
 *                  on comparing two unsigned integers.
 *
 *                  This function is usually called via the macro
 *                  #TEST_LE_U.
 *
 * \param test      Description of the failure or assertion that failed. This
 *                  MUST be a string literal. This normally has the form
 *                  "EXPR1 <= EXPR2" where EXPR1 has the value \p value1
 *                  and EXPR2 has the value \p value2.
 * \param line_no   Line number where the failure originated.
 * \param filename  Filename where the failure originated.
 * \param value1    The first value to compare.
 * \param value2    The second value to compare.
 *
 * \return          \c 1 if \p value1 <= \p value2, otherwise \c 0.
 */
int mbedtls_test_le_u(const char *test, int line_no, const char *filename,
                      unsigned long long value1, unsigned long long value2);

/**
 * \brief           Record the current test case as a failure based
 *                  on comparing two signed integers.
 *
 *                  This function is usually called via the macro
 *                  #TEST_LE_S.
 *
 * \param test      Description of the failure or assertion that failed. This
 *                  MUST be a string literal. This normally has the form
 *                  "EXPR1 <= EXPR2" where EXPR1 has the value \p value1
 *                  and EXPR2 has the value \p value2.
 * \param line_no   Line number where the failure originated.
 * \param filename  Filename where the failure originated.
 * \param value1    The first value to compare.
 * \param value2    The second value to compare.
 *
 * \return          \c 1 if \p value1 <= \p value2, otherwise \c 0.
 */
int mbedtls_test_le_s(const char *test, int line_no, const char *filename,
                      long long value1, long long value2);

/**
 * \brief          This function decodes the hexadecimal representation of
 *                 data.
 *
 * \note           The output buffer can be the same as the input buffer. For
 *                 any other overlapping of the input and output buffers, the
 *                 behavior is undefined.
 *
 * \param obuf     Output buffer.
 * \param obufmax  Size in number of bytes of \p obuf.
 * \param ibuf     Input buffer.
 * \param len      The number of unsigned char written in \p obuf. This must
 *                 not be \c NULL.
 *
 * \return         \c 0 on success.
 * \return         \c -1 if the output buffer is too small or the input string
 *                 is not a valid hexadecimal representation.
 */
int mbedtls_test_unhexify(unsigned char *obuf, size_t obufmax,
                          const char *ibuf, size_t *len);

void mbedtls_test_hexify(unsigned char *obuf,
                         const unsigned char *ibuf,
                         int len);

/**
 * \brief Convert hexadecimal digit to an integer.
 *
 * \param c        The digit to convert (`'0'` to `'9'`, `'A'` to `'F'` or
 *                 `'a'` to `'f'`).
 * \param[out] uc  On success, the value of the digit (0 to 15).
 *
 * \return         0 on success, -1 if \p c is not a hexadecimal digit.
 */
int mbedtls_test_ascii2uc(const char c, unsigned char *uc);

/**
 * Allocate and zeroize a buffer.
 *
 * If the size if zero, a pointer to a zeroized 1-byte buffer is returned.
 *
 * For convenience, dies if allocation fails.
 */
unsigned char *mbedtls_test_zero_alloc(size_t len);

/**
 * Allocate and fill a buffer from hex data.
 *
 * The buffer is sized exactly as needed. This allows to detect buffer
 * overruns (including overreads) when running the test suite under valgrind.
 *
 * If the size if zero, a pointer to a zeroized 1-byte buffer is returned.
 *
 * For convenience, dies if allocation fails.
 */
unsigned char *mbedtls_test_unhexify_alloc(const char *ibuf, size_t *olen);

int mbedtls_test_hexcmp(uint8_t *a, uint8_t *b,
                        uint32_t a_len, uint32_t b_len);

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
#include "test/fake_external_rng_for_test.h"
#endif

#if defined(MBEDTLS_TEST_HOOKS)
/**
 * \brief   Check that only a pure high-level error code is being combined with
 *          a pure low-level error code as otherwise the resultant error code
 *          would be corrupted.
 *
 * \note    Both high-level and low-level error codes cannot be greater than
 *          zero however can be zero. If one error code is zero then the
 *          other error code is returned even if both codes are zero.
 *
 * \note    If the check fails, fail the test currently being run.
 */
void mbedtls_test_err_add_check(int high, int low,
                                const char *file, int line);
#endif

#endif /* TEST_HELPERS_H */
