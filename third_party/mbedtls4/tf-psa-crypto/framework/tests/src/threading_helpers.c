/** Mutex usage verification framework. */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "test_common.h"
#include <test/helpers.h>
#include <test/threading_helpers.h>
#include <test/macros.h>

#include "threading_internal.h"

#if defined(MBEDTLS_THREADING_C)

#if defined(MBEDTLS_THREADING_PTHREAD)

static int threading_thread_create_pthread(mbedtls_test_thread_t *thread, void *(*thread_func)(
                                               void *), void *thread_data)
{
    if (thread == NULL || thread_func == NULL) {
        return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
    }

    if (pthread_create(&thread->thread, NULL, thread_func, thread_data)) {
        return MBEDTLS_ERR_THREADING_THREAD_ERROR;
    }

    return 0;
}

static int threading_thread_join_pthread(mbedtls_test_thread_t *thread)
{
    if (thread == NULL) {
        return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
    }

    if (pthread_join(thread->thread, NULL) != 0) {
        return MBEDTLS_ERR_THREADING_THREAD_ERROR;
    }

    return 0;
}

int (*mbedtls_test_thread_create)(mbedtls_test_thread_t *thread, void *(*thread_func)(void *),
                                  void *thread_data) = threading_thread_create_pthread;
int (*mbedtls_test_thread_join)(mbedtls_test_thread_t *thread) = threading_thread_join_pthread;

#endif /* MBEDTLS_THREADING_PTHREAD */

#if defined(MBEDTLS_THREADING_ALT)

static int threading_thread_create_fail(mbedtls_test_thread_t *thread,
                                        void *(*thread_func)(void *),
                                        void *thread_data)
{
    (void) thread;
    (void) thread_func;
    (void) thread_data;

    return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
}

static int threading_thread_join_fail(mbedtls_test_thread_t *thread)
{
    (void) thread;

    return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
}

int (*mbedtls_test_thread_create)(mbedtls_test_thread_t *thread, void *(*thread_func)(void *),
                                  void *thread_data) = threading_thread_create_fail;
int (*mbedtls_test_thread_join)(mbedtls_test_thread_t *thread) = threading_thread_join_fail;

#endif /* MBEDTLS_THREADING_ALT */

#if defined(MBEDTLS_TEST_MUTEX_USAGE)

#include "mbedtls/threading.h"

/** Mutex usage verification framework.
 *
 * The mutex usage verification code below aims to detect bad usage of
 * Mbed TLS's mutex abstraction layer at runtime. Note that this is solely
 * about the use of the mutex itself, not about checking whether the mutex
 * correctly protects whatever it is supposed to protect.
 *
 * The normal usage of a mutex is:
 * ```
 * digraph mutex_states {
 *   "UNINITIALIZED"; // the initial state
 *   "IDLE";
 *   "FREED";
 *   "LOCKED";
 *   "UNINITIALIZED" -> "IDLE" [label="init"];
 *   "FREED" -> "IDLE" [label="init"];
 *   "IDLE" -> "LOCKED" [label="lock"];
 *   "LOCKED" -> "IDLE" [label="unlock"];
 *   "IDLE" -> "FREED" [label="free"];
 * }
 * ```
 *
 * All bad transitions that can be unambiguously detected are reported.
 * An attempt to use an uninitialized mutex cannot be detected in general
 * since the memory content may happen to denote a valid state. For the same
 * reason, a double init cannot be detected.
 * All-bits-zero is the state of a freed mutex, which is distinct from an
 * initialized mutex, so attempting to use zero-initialized memory as a mutex
 * without calling the init function is detected.
 *
 * The framework attempts to detect missing calls to init and free by counting
 * calls to init and free. If there are more calls to init than free, this
 * means that a mutex is not being freed somewhere, which is a memory leak
 * on platforms where a mutex consumes resources other than the
 * mbedtls_threading_mutex_t object itself. If there are more calls to free
 * than init, this indicates a missing init, which is likely to be detected
 * by an attempt to lock the mutex as well. A limitation of this framework is
 * that it cannot detect scenarios where there is exactly the same number of
 * calls to init and free but the calls don't match. A bug like this is
 * unlikely to happen uniformly throughout the whole test suite though.
 *
 * If an error is detected, this framework will report what happened and the
 * test case will be marked as failed. Unfortunately, the error report cannot
 * indicate the exact location of the problematic call. To locate the error,
 * use a debugger and set a breakpoint on mbedtls_test_mutex_usage_error().
 */
enum value_of_mutex_state_field {
    /* Potential values for the state field of mbedtls_threading_mutex_t.
     * Note that MUTEX_FREED must be 0 and MUTEX_IDLE must be 1 for
     * compatibility with threading_mutex_init_pthread() and
     * threading_mutex_free_pthread(). MUTEX_LOCKED could be any nonzero
     * value. */
    MUTEX_FREED = 0, //! < Set by mbedtls_test_wrap_mutex_free
    MUTEX_IDLE = 1, //! < Set by mbedtls_test_wrap_mutex_init and by mbedtls_test_wrap_mutex_unlock
    MUTEX_LOCKED = 2, //! < Set by mbedtls_test_wrap_mutex_lock
};

#if MBEDTLS_THREADING_INTERNAL_VERSION < 0x04000000
/* Historically, the mutex functions in the API were function pointers.
 * Since TF-PSA-Crypto 1.0.0 (paired with Mbed TLS 4.0.0), the API
 * functions have the historical names but the pointers have different
 * names. When building against Mbed TLS 3.6.x, define the pointer name
 * as aliases. */
#define mbedtls_mutex_init_ptr mbedtls_mutex_init
#define mbedtls_mutex_free_ptr mbedtls_mutex_free
#define mbedtls_mutex_lock_ptr mbedtls_mutex_lock
#define mbedtls_mutex_unlock_ptr mbedtls_mutex_unlock

typedef mbedtls_threading_mutex_t mbedtls_platform_mutex_t;
#define mutex_container(platform_mutex) (platform_mutex)

#else /* MBEDTLS_THREADING_INTERNAL_VERSION >= 0x04000000 */

/* Historically, the mutex platform functions received a pointer to the
 * mbedtls_threading_mutex_t object, and the pthread implementation of
 * that type had a state field. Now the platform functions receive a pointer
 * to the mbedtls_platform_mutex_t object which is a field of the
 * mbedtls_threading_mutex_t object. Get a pointer to the containing
 * object which holds the state field.
 *
 * This weird arrangement was done to minimize changes when switching the
 * mutex usage framework to the separated platform/API types. In the future
 * we should clean up how the usage framework fits into the library.
 */
static mbedtls_threading_mutex_t *mutex_container(
    mbedtls_platform_mutex_t *platform_mutex)
{
    unsigned char *field_address = (unsigned char *) platform_mutex;
    size_t offset = offsetof(mbedtls_threading_mutex_t, mutex);
    return (mbedtls_threading_mutex_t *) (field_address - offset);
}

#endif /* MBEDTLS_THREADING_INTERNAL_VERSION */

#if MBEDTLS_THREADING_INTERNAL_VERSION >= 0x04000001
typedef int mutex_init_return_t;
#else
typedef void mutex_init_return_t;
#endif

typedef struct {
    mutex_init_return_t (*init)(mbedtls_platform_mutex_t *);
    void (*free)(mbedtls_platform_mutex_t *);
    int (*lock)(mbedtls_platform_mutex_t *);
    int (*unlock)(mbedtls_platform_mutex_t *);
} mutex_functions_t;
static mutex_functions_t mutex_functions;

/**
 *  The mutex used to guard live_mutexes below and access to the status variable
 *  in every mbedtls_threading_mutex_t.
 *  Note that we are not reporting any errors when locking and unlocking this
 *  mutex. This is for a couple of reasons:
 *
 *  1. We have no real way of reporting any errors with this mutex - we cannot
 *  report it back to the caller, as the failure was not that of the mutex
 *  passed in. We could fail the test, but again this would indicate a problem
 *  with the test code that did not exist.
 *
 *  2. Any failure to lock is unlikely to be intermittent, and will thus not
 *  give false test results - the overall result would be to turn off the
 *  testing. This is not a situation that is likely to happen with normal
 *  testing and we still have TSan to fall back on should this happen.
 */
mbedtls_platform_mutex_t mbedtls_test_mutex_mutex;

/**
 *  The total number of calls to mbedtls_mutex_init(), minus the total number
 *  of calls to mbedtls_mutex_free().
 *
 *  Do not read or write without holding mbedtls_test_mutex_mutex (above). Reset
 *  to 0 after each test case.
 */
static int live_mutexes;

static void mbedtls_test_mutex_usage_error(mbedtls_platform_mutex_t *mutex,
                                           const char *msg)
{
    (void) mutex;

    mbedtls_test_set_mutex_usage_error(msg);
    mbedtls_fprintf(stdout, "[mutex: %s] ", msg);
    /* Don't mark the test as failed yet. This way, if the test fails later
     * for a functional reason, the test framework will report the message
     * and location for this functional reason. If the test passes,
     * mbedtls_test_mutex_usage_check() will mark it as failed. */
}

static int mbedtls_test_mutex_can_test(mbedtls_platform_mutex_t *mutex)
{
    /* If we attempt to run tests on this mutex then we are going to run into a
     * couple of problems:
     * 1. If any test on this mutex fails, we are going to deadlock when
     * reporting that failure, as we already hold the mutex at that point.
     * 2. Given the 'global' position of the initialization and free of this
     * mutex, it will be shown as leaked on the first test run. */
    if (mutex_container(mutex) == mbedtls_test_get_info_mutex()) {
        return 0;
    }

    return 1;
}

static mutex_init_return_t mbedtls_test_wrap_mutex_init(mbedtls_platform_mutex_t *mutex)
{
#if MBEDTLS_THREADING_INTERNAL_VERSION >= 0x04000001
    int ret = mutex_functions.init(mutex);
    if (ret != 0) {
        return ret;
    }
#else
    mutex_functions.init(mutex);
#endif

    if (mbedtls_test_mutex_can_test(mutex)) {
        if (mutex_functions.lock(&mbedtls_test_mutex_mutex) == 0) {
            mutex_container(mutex)->state = MUTEX_IDLE;
            ++live_mutexes;

            mutex_functions.unlock(&mbedtls_test_mutex_mutex);
        }
    }

#if MBEDTLS_THREADING_INTERNAL_VERSION >= 0x04000001
    return 0;
#endif
}

static void mbedtls_test_wrap_mutex_free(mbedtls_platform_mutex_t *mutex)
{
    if (mbedtls_test_mutex_can_test(mutex)) {
        if (mutex_functions.lock(&mbedtls_test_mutex_mutex) == 0) {

            switch (mutex_container(mutex)->state) {
                case MUTEX_FREED:
                    mbedtls_test_mutex_usage_error(mutex, "free without init or double free");
                    break;
                case MUTEX_IDLE:
                    mutex_container(mutex)->state = MUTEX_FREED;
                    --live_mutexes;
                    break;
                case MUTEX_LOCKED:
                    mbedtls_test_mutex_usage_error(mutex, "free without unlock");
                    break;
                default:
                    mbedtls_test_mutex_usage_error(mutex, "corrupted state");
                    break;
            }

            mutex_functions.unlock(&mbedtls_test_mutex_mutex);
        }
    }

    mutex_functions.free(mutex);
}

static int mbedtls_test_wrap_mutex_lock(mbedtls_platform_mutex_t *mutex)
{
    /* Lock the passed in mutex first, so that the only way to change the state
     * is to hold the passed in and internal mutex - otherwise we create a race
     * condition. */
    int ret = mutex_functions.lock(mutex);

    if (mbedtls_test_mutex_can_test(mutex)) {
        if (mutex_functions.lock(&mbedtls_test_mutex_mutex) == 0) {
            switch (mutex_container(mutex)->state) {
                case MUTEX_FREED:
                    mbedtls_test_mutex_usage_error(mutex, "lock without init");
                    break;
                case MUTEX_IDLE:
                    if (ret == 0) {
                        mutex_container(mutex)->state = MUTEX_LOCKED;
                    }
                    break;
                case MUTEX_LOCKED:
                    mbedtls_test_mutex_usage_error(mutex, "double lock");
                    break;
                default:
                    mbedtls_test_mutex_usage_error(mutex, "corrupted state");
                    break;
            }

            mutex_functions.unlock(&mbedtls_test_mutex_mutex);
        }
    }

    return ret;
}

static int mbedtls_test_wrap_mutex_unlock(mbedtls_platform_mutex_t *mutex)
{
    /* Lock the internal mutex first and change state, so that the only way to
     * change the state is to hold the passed in and internal mutex - otherwise
     * we create a race condition. */
    if (mbedtls_test_mutex_can_test(mutex)) {
        if (mutex_functions.lock(&mbedtls_test_mutex_mutex) == 0) {
            switch (mutex_container(mutex)->state) {
                case MUTEX_FREED:
                    mbedtls_test_mutex_usage_error(mutex, "unlock without init");
                    break;
                case MUTEX_IDLE:
                    mbedtls_test_mutex_usage_error(mutex, "unlock without lock");
                    break;
                case MUTEX_LOCKED:
                    mutex_container(mutex)->state = MUTEX_IDLE;
                    break;
                default:
                    mbedtls_test_mutex_usage_error(mutex, "corrupted state");
                    break;
            }
            mutex_functions.unlock(&mbedtls_test_mutex_mutex);
        }
    }

    return mutex_functions.unlock(mutex);
}

void mbedtls_test_mutex_usage_init(void)
{
    mutex_functions.init = *mbedtls_mutex_init_ptr;
    mutex_functions.free = *mbedtls_mutex_free_ptr;
    mutex_functions.lock = *mbedtls_mutex_lock_ptr;
    mutex_functions.unlock = *mbedtls_mutex_unlock_ptr;
    mbedtls_mutex_init_ptr = &mbedtls_test_wrap_mutex_init;
    mbedtls_mutex_free_ptr = &mbedtls_test_wrap_mutex_free;
    mbedtls_mutex_lock_ptr = &mbedtls_test_wrap_mutex_lock;
    mbedtls_mutex_unlock_ptr = &mbedtls_test_wrap_mutex_unlock;

    mutex_functions.init(&mbedtls_test_mutex_mutex);
}

void mbedtls_test_mutex_usage_check(void)
{
    if (mutex_functions.lock(&mbedtls_test_mutex_mutex) == 0) {
        if (live_mutexes != 0) {
            /* A positive number (more init than free) means that a mutex resource
             * is leaking (on platforms where a mutex consumes more than the
             * mbedtls_threading_mutex_t object itself). The (hopefully) rare
             * case of a negative number means a missing init somewhere. */
            mbedtls_fprintf(stdout, "[mutex: %d leaked] ", live_mutexes);
            live_mutexes = 0;
            mbedtls_test_set_mutex_usage_error("missing free");
        }
        if (mbedtls_test_get_mutex_usage_error() != NULL &&
            mbedtls_test_get_result() != MBEDTLS_TEST_RESULT_FAILED) {
            /* Functionally, the test passed. But there was a mutex usage error,
             * so mark the test as failed after all. */
            mbedtls_test_fail("Mutex usage error", __LINE__, __FILE__);
        }
        mbedtls_test_set_mutex_usage_error(NULL);

        mutex_functions.unlock(&mbedtls_test_mutex_mutex);
    }
}

void mbedtls_test_mutex_usage_end(void)
{
    mbedtls_mutex_init_ptr = mutex_functions.init;
    mbedtls_mutex_free_ptr = mutex_functions.free;
    mbedtls_mutex_lock_ptr = mutex_functions.lock;
    mbedtls_mutex_unlock_ptr = mutex_functions.unlock;

    mutex_functions.free(&mbedtls_test_mutex_mutex);
}

#endif /* MBEDTLS_TEST_MUTEX_USAGE */

#endif /* MBEDTLS_THREADING_C */
