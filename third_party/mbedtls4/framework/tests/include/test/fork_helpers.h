/** Helper functions for testing with subprocesses.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef TEST_FORK_HELPERS_H
#define TEST_FORK_HELPERS_H

#include "test/helpers.h"

/** Type of a function to run in a child process.
 *
 * The function can mark the test case as failed by calling
 * mbedtls_test_fail(). This information will be reported to the parent.
 *
 * \param param         Parameter passed to the callback.
 * \param[out] output   Buffer for data to pass to the parent.
 *                      This data is ignored if the test case is marked
 *                      as failed.
 * \param output_size   Size of \p output in bytes.
 * \param[out] output_length    Number of bytes written to \p output, to be
 *                              passed to the parent. The default is \c 0.
 */
typedef void mbedtls_test_fork_child_callback_t(
    void *param,
    unsigned char *output, size_t output_size, size_t *output_length);

/* Fork a child process and wait for it to collect some data.
 *
 * This is similar to backquotes or `$(...)` in a shell.
 *
 * This function blocks until the child exits.
 *
 * If the child marks the test as failed or skipped, the child's test
 * information (test result and failure location) is propagated to the
 * parent.
 *
 * \note Memory leak detection is disabled in the child.
 *
 * \param child_callback        Callback function to run in the child.
 * \param param                 Parameter to pass to the callback function.
 * \param[out] child_output     On success, data retrieved from the child.
 *                              Note that the data is only available if the
 *                              child did not mark the test case as failed
 *                              or skipped.
 * \param child_output_size     Size of \p child_output in bytes.
 * \param[out] child_output_length  On success, the number of bytes collected
 *                                  from the child in \c child_output.
 *
 * \return \c 0 on success.
 *         A nonzero value if the test case is marked as failed or skipped.
 */
int mbedtls_test_fork_run_child(
    mbedtls_test_fork_child_callback_t *child_callback,
    void *param,
    unsigned char *child_output, size_t child_output_size,
    size_t *child_output_length);

#endif /* TEST_FORK_HELPERS_H */
