/** \file fork_helpers.c
 *
 * \brief Helper functions for testing with subprocesses.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "test_common.h"
#include <test/helpers.h>
#include <test/macros.h>

#if defined(MBEDTLS_PLATFORM_IS_UNIXLIKE)

#include <test/fork_helpers.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/** Child exit code for mbedtls_test_fork_run_child().
 */
typedef enum {
    /** Reporting of the child output or the child test result through
     * the pipe succeeded.
     *
     * The content sent on the pipe has the following format:
     * - [1 byte] #mbedtls_test_result_t \c test_result
     * - Case \c test_result:
     *     - If \c test_result == #MBEDTLS_TEST_RESULT_SUCCESS:
     *       the output from the child body function.
     *     - Otherwise:
     *       the child failure (or skip) information, a direct write of
     *       a #mbedtls_test_result_t structure.
     */
    CHILD_EXIT_CODE_OK = 0,
    /** Something went wrong, e.g. a write error on the pipe. */
    CHILD_EXIT_CODE_REPORTING_FAILED = 122,
} child_exit_code_t;

static int env_contains_substring(const char *name, const char *substring)
{
    const char *value = getenv(name);
    if (value == NULL) {
        return 0;
    } else {
        return strstr(value, substring) != NULL;
    }
}

static int probably_running_under_valgrind(void)
{
    if (env_contains_substring("LD_PRELOAD", "/vgpreload_")) {
        return 1;
    }
    if (env_contains_substring("DYLD_INSERT_LIBRARIES", "/vgpreload_")) {
        return 1;
    }
    return 0;
}

#if defined(__GNUC__)
__attribute__((__noreturn__))
#endif
static void run_child(
    int write_fd,
    mbedtls_test_fork_child_callback_t *child_callback,
    void *param,
    unsigned char *buf, size_t size)
{
    /* If something goes wrong while trying to report what happened
     * in the child, exit with a nonzero status. */
    int child_exit_code = CHILD_EXIT_CODE_REPORTING_FAILED;
    /* We'll use stdio to write to the pipe, so we don't have to
     * manage EINTR and such. */
    FILE *file = fdopen(write_fd, "a");
    size_t length = 0;

    if (file == NULL) {
        /* There's no way we can report anything other than the exit code.
         * So we might as well quit without even running the child callback. */
        goto write_done;
    }

    child_callback(param, buf, size, &length);
    TEST_LE_U(length, size);

    /* Label called `exit`: this is where TEST_ASSERT() and friends jump to. */
exit:
    ; // label followed by a declaration is not portable C
    char result_char = mbedtls_test_get_result();
    if (fputc(result_char, file) == EOF) {
        goto write_done;
    }
    if (result_char == MBEDTLS_TEST_RESULT_SUCCESS) {
        if (fwrite(buf, length, 1, file) != 1) {
            goto write_done;
        }
    } else {
        mbedtls_test_info_t test_info;
        mbedtls_test_info_save(&test_info);
        if (fwrite(&test_info, sizeof(test_info), 1, file) != 1) {
            goto write_done;
        }
    }
    if (fflush(file) != 0) {
        goto write_done;
    }
    child_exit_code = CHILD_EXIT_CODE_OK;

    /* Label for `_exit()` call: this is where we jump to if the failure
     * reporting fails. */
write_done:
    /* We must call _exit(), not exit(), because the child must not run the
     * things that normally run at exit.
     *
     * - Do not flush any stdio buffers! Any unflushed buffers are inherited
     *   from our parent, and if we flushed them, we'd get duplicate output
     *   since the parent would also write the same buffer content.
     * - Do not run atexit hooks, e.g. leak detection code from sanitizers
     *   such as ASan. The child leaks any number of resources which are
     *   inherited from the parent but not used in the child. It's the
     *   parent's job to check for resource leaks.
     *   (We deliberately do not clean up in the child. One reason is that
     *   we try to minimize what happens in the child, because it's difficult
     *   to debug. Another reason is that we must not cause external effects
     *   such as destroying a PSA persistent key.)
     */
    if (probably_running_under_valgrind()) {
        /* Valgrind overloads _exit(), and this makes it do weird things,
         * including an lseek() call to rewind the pointer on the file
         * description for the `.datax` file, causing the same test cases
         * to run again (or parse errors, depending on the exact amount
         * of rewinding).
         *
         * Valgrind doesn't overload execve() and friends. So instead of
         * _exit(), execute a shell command that returns the same status.
         */
        char cmd[20];
        snprintf(cmd, sizeof(cmd), "exit %d", child_exit_code);
        execlp("sh", "sh", "-c", cmd, NULL);
    }
    _exit(child_exit_code);
}

int mbedtls_test_fork_run_child(
    mbedtls_test_fork_child_callback_t *child_callback,
    void *param,
    unsigned char *child_output, size_t child_output_size,
    size_t *child_output_length)
{
    *child_output_length = 0;

    int ret = -1;
    pid_t pid = -1;
    int pipe_fd[2] = { -1, -1 };

    /* Set up a pipe. The child will write to pipe_fd[1], and the
     * parent will read from pipe_fd[0]. */
    TEST_ASSERT_ERRNO(pipe(pipe_fd) != -1);

    pid = fork();
    TEST_ASSERT_ERRNO(pid != -1);

    if (pid == 0) {
        /* The child code */
        close(pipe_fd[0]);
        run_child(pipe_fd[1], child_callback, param,
                  child_output, child_output_size);
        /* Unreachable */
    }
    /* Beyond this point, we're in the parent (original) process. */

    close(pipe_fd[1]);
    pipe_fd[1] = -1;

    unsigned char result_char;
    struct {
        mbedtls_test_info_t child_test_info;
        unsigned char excess;
    } reading_on_failure;
    /* Normally, the child should give us a 1-byte result, then either
     * the child body's output or a test info. */
    ssize_t n = read(pipe_fd[0], &result_char, 1);
    TEST_EQUAL(n, 1);

    /* Tentatively read what we were promised. Don't commit to anything
     * until we have the child's exit status. */
    size_t bytes_read = 0;
    if (result_char == MBEDTLS_TEST_RESULT_SUCCESS) {
        do {
            n = read(pipe_fd[0],
                     child_output + bytes_read,
                     child_output_size - bytes_read);
            if (n > 0) {
                bytes_read += n;
            }
        } while (n > 0 && bytes_read < child_output_size);
        TEST_ASSERT_ERRNO(n != -1);
    } else {
        do {
            n = read(pipe_fd[0],
                     (unsigned char *) &reading_on_failure + bytes_read,
                     sizeof(reading_on_failure) - bytes_read);
            if (n > 0) {
                bytes_read += n;
            }
        } while (n > 0 && bytes_read < sizeof(reading_on_failure));
        TEST_ASSERT_ERRNO(n != -1);
        /* Check that the child wrote the amount of data that what we expect. */
        TEST_EQUAL(bytes_read, sizeof(reading_on_failure.child_test_info));
    }

    /* Close the pipe. If we left it open, there could be a deadlock if the
     * child tried to write more than it should, while the parent is just
     * waiting for the child to exit. */
    close(pipe_fd[0]);
    pipe_fd[0] = -1;

    int wstatus;
    TEST_ASSERT_ERRNO(waitpid(pid, &wstatus, 0) == pid);
    if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == CHILD_EXIT_CODE_OK) {
        if (result_char == MBEDTLS_TEST_RESULT_SUCCESS) {
            *child_output_length = bytes_read;
            ret = 0;
        } else {
            mbedtls_test_info_overwrite(&reading_on_failure.child_test_info);
        }
    } else {
        /* Weird status, just report it. */
        TEST_EQUAL(wstatus, 0);
    }

exit:
    close(pipe_fd[0]);
    close(pipe_fd[1]);
    return ret;
}

#endif /* MBEDTLS_PLATFORM_IS_UNIXLIKE */
