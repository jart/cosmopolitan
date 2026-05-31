/** \file metatest.c
 *
 *  \brief Test features of the test framework.
 *
 * When you run this program, it runs a single "meta-test". A meta-test
 * performs an operation which should be caught as a failure by our
 * test framework. The meta-test passes if this program calls `exit` with
 * a nonzero status, or aborts, or is terminated by a signal, or if the
 * framework running the program considers the run an error (this happens
 * with Valgrind for a memory leak). The non-success of the meta-test
 * program means that the test failure has been caught correctly.
 *
 * Some failures are purely functional: the logic of the code causes the
 * test result to be set to FAIL. Other failures come from extra
 * instrumentation which is not present in a normal build; for example,
 * Asan or Valgrind to detect memory leaks. This is reflected by the
 * "platform" associated with each meta-test.
 *
 * Use the companion script `framework/scripts/run-metatests.sh` to run all
 * the meta-tests for a given platform and validate that they trigger a
 * detected failure as expected.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

/* On Mingw-w64, force the use of a C99-compliant printf() and friends.
 * This is necessary on older versions of Mingw and/or Windows runtimes
 * where snprintf does not always zero-terminate the buffer, and does
 * not support formats such as "%zu" for size_t and "%lld" for long long.
 */
#if !defined(__USE_MINGW_ANSI_STDIO)
#define __USE_MINGW_ANSI_STDIO 1
#endif

#include <mbedtls/debug.h>
#include <mbedtls/platform.h>
#include <mbedtls/platform_util.h>
#include "test/helpers.h"
#include "test/threading_helpers.h"
#include "test/macros.h"
#include "test/memory.h"

#include <stdio.h>
#include <string.h>

#if defined(MBEDTLS_THREADING_C)
#include "threading_internal.h"
#endif


/* This is an external variable, so the compiler doesn't know that we're never
 * changing its value.
 */
volatile int false_but_the_compiler_does_not_know = 0;

/* Hide calls to calloc/free from static checkers such as
 * `gcc-12 -Wuse-after-free`, to avoid compile-time complaints about
 * code where we do mean to cause a runtime error. */
void * (* volatile calloc_but_the_compiler_does_not_know)(size_t, size_t) = mbedtls_calloc;
void(*volatile free_but_the_compiler_does_not_know)(void *) = mbedtls_free;

/* Set n bytes at the address p to all-bits-zero, in such a way that
 * the compiler should not know that p is all-bits-zero. */
static void set_to_zero_but_the_compiler_does_not_know(volatile void *p, size_t n)
{
    memset((void *) p, false_but_the_compiler_does_not_know, n);
}

/* Simulate an access to the given object, to avoid compiler optimizations
 * in code that prepares or consumes the object. */
static void do_nothing_with_object(void *p)
{
    (void) p;
}
void(*volatile do_nothing_with_object_but_the_compiler_does_not_know)(void *) =
    do_nothing_with_object;


/****************************************************************/
/* Test framework features */
/****************************************************************/

static void meta_test_fail(const char *name)
{
    (void) name;
    mbedtls_test_fail("Forced test failure", __LINE__, __FILE__);
}

static void meta_test_not_equal(const char *name)
{
    int left = 20;
    int right = 10;

    (void) name;

    TEST_EQUAL(left, right);
exit:
    ;
}

static void meta_test_not_le_s(const char *name)
{
    int left = 20;
    int right = 10;

    (void) name;

    TEST_LE_S(left, right);
exit:
    ;
}

static void meta_test_not_le_u(const char *name)
{
    size_t left = 20;
    size_t right = 10;

    (void) name;

    TEST_LE_U(left, right);
exit:
    ;
}

/****************************************************************/
/* Platform features */
/****************************************************************/

static void null_pointer_dereference(const char *name)
{
    (void) name;
    volatile char *volatile p;
    set_to_zero_but_the_compiler_does_not_know(&p, sizeof(p));
    /* Undefined behavior (read from null data pointer) */
    mbedtls_printf("%p -> %u\n", (void *) p, (unsigned) *p);
}

static void null_pointer_call(const char *name)
{
    (void) name;
    unsigned(*volatile p)(void);
    set_to_zero_but_the_compiler_does_not_know(&p, sizeof(p));
    /* Undefined behavior (execute null function pointer) */
    /* The pointer representation may be truncated, but we don't care:
     * the only point of printing it is to have some use of the pointer
     * to dissuade the compiler from optimizing it away. */
    mbedtls_printf("%lx() -> %u\n", (unsigned long) (uintptr_t) p, p());
}


/****************************************************************/
/* Memory */
/****************************************************************/

static void read_after_free(const char *name)
{
    (void) name;
    volatile char *p = calloc_but_the_compiler_does_not_know(1, 1);
    *p = 'a';
    free_but_the_compiler_does_not_know((void *) p);
    /* Undefined behavior (read after free) */
    mbedtls_printf("%u\n", (unsigned) *p);
}

static void double_free(const char *name)
{
    (void) name;
    volatile char *p = calloc_but_the_compiler_does_not_know(1, 1);
    *p = 'a';
    free_but_the_compiler_does_not_know((void *) p);
    /* Undefined behavior (double free) */
    free_but_the_compiler_does_not_know((void *) p);
}

static void read_uninitialized_stack(const char *name)
{
    (void) name;
    char buf[1];
    if (false_but_the_compiler_does_not_know) {
        buf[0] = '!';
    }
    char *volatile p = buf;
    if (*p != 0) {
        /* Unspecified result (read from uninitialized memory) */
        mbedtls_printf("%u\n", (unsigned) *p);
    }
}

static void memory_leak(const char *name)
{
    (void) name;
    volatile char *p = calloc_but_the_compiler_does_not_know(1, 1);
    mbedtls_printf("%u\n", (unsigned) *p);
    /* Leak of a heap object */
}

/* name = "test_memory_poison_%(start)_%(offset)_%(count)_%(direction)"
 * Poison a region starting at start from an 8-byte aligned origin,
 * encompassing count bytes. Access the region at offset from the start.
 * %(start), %(offset) and %(count) are decimal integers.
 * %(direction) is either the character 'r' for read or 'w' for write.
 */
static void test_memory_poison(const char *name)
{
    size_t start = 0, offset = 0, count = 0;
    char direction = 'r';
    if (sscanf(name,
               "%*[^0-9]%zu%*[^0-9]%zu%*[^0-9]%zu_%c",
               &start, &offset, &count, &direction) != 4) {
        mbedtls_fprintf(stderr, "%s: Bad name format: %s\n", __func__, name);
        return;
    }

    union {
        long long ll;
        unsigned char buf[32];
    } aligned;
    memset(aligned.buf, 'a', sizeof(aligned.buf));

    if (start > sizeof(aligned.buf)) {
        mbedtls_fprintf(stderr,
                        "%s: start=%zu > size=%zu",
                        __func__, start, sizeof(aligned.buf));
        return;
    }
    if (start + count > sizeof(aligned.buf)) {
        mbedtls_fprintf(stderr,
                        "%s: start+count=%zu > size=%zu",
                        __func__, start + count, sizeof(aligned.buf));
        return;
    }
    if (offset >= count) {
        mbedtls_fprintf(stderr,
                        "%s: offset=%zu >= count=%zu",
                        __func__, offset, count);
        return;
    }

    MBEDTLS_TEST_MEMORY_POISON(aligned.buf + start, count);

    if (direction == 'w') {
        aligned.buf[start + offset] = 'b';
        do_nothing_with_object_but_the_compiler_does_not_know(aligned.buf);
    } else {
        do_nothing_with_object_but_the_compiler_does_not_know(aligned.buf);
        mbedtls_printf("%u\n", (unsigned) aligned.buf[start + offset]);
    }
}


/****************************************************************/
/* Threading */
/****************************************************************/

static void mutex_lock_not_initialized(const char *name)
{
    (void) name;
#if defined(MBEDTLS_THREADING_C)
    mbedtls_threading_mutex_t mutex;
    memset(&mutex, 0, sizeof(mutex));
    /* This mutex usage error is detected by our test framework's mutex usage
     * verification framework. See framework/tests/src/threading_helpers.c. Other
     * threading implementations (e.g. pthread without our instrumentation)
     * might consider this normal usage. */
    TEST_ASSERT(mbedtls_mutex_lock(&mutex) == 0);
exit:
    ;
#endif
}

static void mutex_unlock_not_initialized(const char *name)
{
    (void) name;
#if defined(MBEDTLS_THREADING_C)
    mbedtls_threading_mutex_t mutex;
    memset(&mutex, 0, sizeof(mutex));
    /* This mutex usage error is detected by our test framework's mutex usage
     * verification framework. See framework/tests/src/threading_helpers.c. Other
     * threading implementations (e.g. pthread without our instrumentation)
     * might consider this normal usage. */
    TEST_ASSERT(mbedtls_mutex_unlock(&mutex) == 0);
exit:
    ;
#endif
}

/* Since TF-PSA-Crypto 1.0, mbedtls_mutex_free() on an all-bits-zero
 * mutex is defined to be a no-op. In earlier library versions,
 * it had undefined behavior. */
#if MBEDTLS_THREADING_INTERNAL_VERSION <= 0x04000000
static void mutex_free_not_initialized(const char *name)
{
    (void) name;
#if defined(MBEDTLS_THREADING_C)
    mbedtls_threading_mutex_t mutex;
    memset(&mutex, 0, sizeof(mutex));
    /* This mutex usage error is detected by our test framework's mutex usage
     * verification framework. See framework/tests/src/threading_helpers.c. Other
     * threading implementations (e.g. pthread without our instrumentation)
     * might consider this normal usage. */
    mbedtls_mutex_free(&mutex);
#endif
}
#endif

static void mutex_double_init(const char *name)
{
    (void) name;
#if defined(MBEDTLS_THREADING_C)
    mbedtls_threading_mutex_t mutex;
    mbedtls_mutex_init(&mutex);
    /* This mutex usage error is detected by our test framework's mutex usage
     * verification framework. See framework/tests/src/threading_helpers.c. Other
     * threading implementations (e.g. pthread without our instrumentation)
     * might consider this normal usage. */
    mbedtls_mutex_init(&mutex);
    mbedtls_mutex_free(&mutex);
#endif
}

/* Since TF-PSA-Crypto 1.0, mbedtls_mutex_free() on an all-bits-zero
 * mutex is defined to be a no-op. In earlier library versions,
 * it had undefined behavior. */
#if MBEDTLS_THREADING_INTERNAL_VERSION <= 0x04000000
static void mutex_double_free(const char *name)
{
    (void) name;
#if defined(MBEDTLS_THREADING_C)
    mbedtls_threading_mutex_t mutex;
    mbedtls_mutex_init(&mutex);
    mbedtls_mutex_free(&mutex);
    /* This mutex usage error is detected by our test framework's mutex usage
     * verification framework. See framework/tests/src/threading_helpers.c. Other
     * threading implementations (e.g. pthread without our instrumentation)
     * might consider this normal usage. */
    mbedtls_mutex_free(&mutex);
#endif
}
#endif

static void mutex_leak(const char *name)
{
    (void) name;
#if defined(MBEDTLS_THREADING_C)
    mbedtls_threading_mutex_t mutex;
    mbedtls_mutex_init(&mutex);
#endif
    /* This mutex usage error is detected by our test framework's mutex usage
     * verification framework. See framework/tests/src/threading_helpers.c. Other
     * threading implementations (e.g. pthread without our instrumentation)
     * might consider this normal usage. */
}


/****************************************************************/
/* Command line entry point */
/****************************************************************/

typedef struct {
    /** Command line argument that will trigger that metatest.
     *
     * Conventionally matches "[a-z0-9_]+". */
    const char *name;

    /** Platform under which that metatest is valid.
     *
     * - "any": should work anywhere.
     * - "asan": triggers ASan (Address Sanitizer).
     * - "msan": triggers MSan (Memory Sanitizer).
     * - "pthread": requires MBEDTLS_THREADING_PTHREAD and MBEDTLS_TEST_HOOKS,
     *   which enables MBEDTLS_TEST_MUTEX_USAGE internally in the test
     *   framework (see framework/tests/src/threading_helpers.c).
     */
    const char *platform;

    /** Function that performs the metatest.
     *
     * The function receives the name as an argument. This allows using the
     * same function to perform multiple variants of a test based on the name.
     *
     * When executed on a conforming platform, the function is expected to
     * either cause a test failure (mbedtls_test_fail()), or cause the
     * program to abort in some way (e.g. by causing a segfault or by
     * triggering a sanitizer).
     *
     * When executed on a non-conforming platform, the function may return
     * normally or may have unpredictable behavior.
     */
    void (*entry_point)(const char *name);
} metatest_t;

/* The list of available meta-tests. Remember to register new functions here!
 *
 * Note that we always compile all the functions, so that `metatest --list`
 * will always list all the available meta-tests.
 *
 * See the documentation of metatest_t::platform for the meaning of
 * platform values.
 */
metatest_t metatests[] = {
    { "test_fail", "any", meta_test_fail },
    { "test_not_equal", "any", meta_test_not_equal },
    { "test_not_le_s", "any", meta_test_not_le_s },
    { "test_not_le_u", "any", meta_test_not_le_u },
    { "null_dereference", "any", null_pointer_dereference },
    { "null_call", "any", null_pointer_call },
    { "read_after_free", "asan", read_after_free },
    { "double_free", "asan", double_free },
    { "read_uninitialized_stack", "msan", read_uninitialized_stack },
    { "memory_leak", "asan", memory_leak },
    { "test_memory_poison_0_0_8_r", "poison", test_memory_poison },
    { "test_memory_poison_0_0_8_w", "poison", test_memory_poison },
    { "test_memory_poison_0_7_8_r", "poison", test_memory_poison },
    { "test_memory_poison_0_7_8_w", "poison", test_memory_poison },
    { "test_memory_poison_0_0_1_r", "poison", test_memory_poison },
    { "test_memory_poison_0_0_1_w", "poison", test_memory_poison },
    { "test_memory_poison_0_1_2_r", "poison", test_memory_poison },
    { "test_memory_poison_0_1_2_w", "poison", test_memory_poison },
    { "test_memory_poison_7_0_8_r", "poison", test_memory_poison },
    { "test_memory_poison_7_0_8_w", "poison", test_memory_poison },
    { "test_memory_poison_7_7_8_r", "poison", test_memory_poison },
    { "test_memory_poison_7_7_8_w", "poison", test_memory_poison },
    { "test_memory_poison_7_0_1_r", "poison", test_memory_poison },
    { "test_memory_poison_7_0_1_w", "poison", test_memory_poison },
    { "test_memory_poison_7_1_2_r", "poison", test_memory_poison },
    { "test_memory_poison_7_1_2_w", "poison", test_memory_poison },
    { "mutex_lock_not_initialized", "pthread", mutex_lock_not_initialized },
    { "mutex_unlock_not_initialized", "pthread", mutex_unlock_not_initialized },
#if MBEDTLS_THREADING_INTERNAL_VERSION <= 0x04000000
    { "mutex_free_not_initialized", "pthread", mutex_free_not_initialized },
#endif
    { "mutex_double_init", "pthread", mutex_double_init },
#if MBEDTLS_THREADING_INTERNAL_VERSION <= 0x04000000
    { "mutex_double_free", "pthread", mutex_double_free },
#endif
    { "mutex_leak", "pthread", mutex_leak },
    { NULL, NULL, NULL }
};

static void help(FILE *out, const char *argv0)
{
    mbedtls_fprintf(out, "Usage: %s list|TEST\n", argv0);
    mbedtls_fprintf(out, "Run a meta-test that should cause a test failure.\n");
    mbedtls_fprintf(out, "With 'list', list the available tests and their platform requirement.\n");
}

int main(int argc, char *argv[])
{
    const char *argv0 = argc > 0 ? argv[0] : "metatest";
    if (argc != 2) {
        help(stderr, argv0);
        mbedtls_exit(MBEDTLS_EXIT_FAILURE);
    }

    /* Support "-help", "--help", "--list", etc. */
    const char *command = argv[1];
    while (*command == '-') {
        ++command;
    }

    if (strcmp(argv[1], "help") == 0) {
        help(stdout, argv0);
        mbedtls_exit(MBEDTLS_EXIT_SUCCESS);
    }
    if (strcmp(argv[1], "list") == 0) {
        for (const metatest_t *p = metatests; p->name != NULL; p++) {
            mbedtls_printf("%s %s\n", p->name, p->platform);
        }
        mbedtls_exit(MBEDTLS_EXIT_SUCCESS);
    }

#if defined(MBEDTLS_TEST_MUTEX_USAGE)
    mbedtls_test_mutex_usage_init();
#endif

    for (const metatest_t *p = metatests; p->name != NULL; p++) {
        if (strcmp(argv[1], p->name) == 0) {
            mbedtls_printf("Running metatest %s...\n", argv[1]);
            p->entry_point(argv[1]);
#if defined(MBEDTLS_TEST_MUTEX_USAGE)
            mbedtls_test_mutex_usage_check();
#endif
            int result = (int) mbedtls_test_get_result();

            mbedtls_printf("Running metatest %s... done, result=%d\n",
                           argv[1], result);
            mbedtls_exit(result == MBEDTLS_TEST_RESULT_SUCCESS ?
                         MBEDTLS_EXIT_SUCCESS :
                         MBEDTLS_EXIT_FAILURE);
        }
    }

    mbedtls_fprintf(stderr, "%s: FATAL: No such metatest: %s\n",
                    argv0, command);
    mbedtls_exit(MBEDTLS_EXIT_FAILURE);
}
