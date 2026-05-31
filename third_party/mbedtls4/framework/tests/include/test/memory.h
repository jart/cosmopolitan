/**
 * \file memory.h
 *
 * \brief   Helper macros and functions related to testing memory management.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef TEST_MEMORY_H
#define TEST_MEMORY_H

#include "build_info.h"

#include "mbedtls/platform.h"
#include "test/helpers.h"

/** \def MBEDTLS_TEST_MEMORY_CAN_POISON
 *
 * This macro is defined if the tests are compiled with a method to mark
 * memory as poisoned, which can be used to enforce some memory access
 * policies.
 *
 * Support for the C11 thread_local keyword is also required.
 *
 * Currently, only Asan (Address Sanitizer) is supported.
 */
#if defined(MBEDTLS_TEST_HAVE_ASAN) && \
    (__STDC_VERSION__ >= 201112L) && \
    !defined(PSA_CRYPTO_DRIVER_TEST)
#  define MBEDTLS_TEST_MEMORY_CAN_POISON
#endif

/** \def MBEDTLS_TEST_MEMORY_POISON(buf, size)
 *
 * Poison a memory area so that any attempt to read or write from it will
 * cause a runtime failure.
 *
 * Depending on the implementation, this may poison a few bytes beyond the
 * indicated region, but will never poison a separate object on the heap
 * or a separate object with more than the alignment of a long long.
 *
 * The behavior is undefined if any part of the memory area is invalid.
 *
 * This is a no-op in builds without a poisoning method.
 * See #MBEDTLS_TEST_MEMORY_CAN_POISON.
 *
 * \param buf   Pointer to the beginning of the memory area to poison.
 * \param size  Size of the memory area in bytes.
 */

/** \def MBEDTLS_TEST_MEMORY_UNPOISON(buf, size)
 *
 * Undo the effect of #MBEDTLS_TEST_MEMORY_POISON.
 *
 * The behavior is undefined if any part of the memory area is invalid,
 * or if the memory area contains a mixture of poisoned and unpoisoned parts.
 *
 * This is a no-op in builds without a poisoning method.
 * See #MBEDTLS_TEST_MEMORY_CAN_POISON.
 *
 * \param buf   Pointer to the beginning of the memory area to unpoison.
 * \param size  Size of the memory area in bytes.
 */

#if defined(MBEDTLS_TEST_MEMORY_CAN_POISON)

/** Thread-local variable used to enable memory poisoning. This is set and
 *  unset in the test wrappers so that calls to PSA functions from the library
 *  do not poison memory.
 */
extern _Thread_local unsigned int mbedtls_test_memory_poisoning_count;

/** Poison a memory area so that any attempt to read or write from it will
 * cause a runtime failure.
 *
 * The behavior is undefined if any part of the memory area is invalid.
 */
void mbedtls_test_memory_poison(const unsigned char *ptr, size_t size);
#define MBEDTLS_TEST_MEMORY_POISON(ptr, size)    \
    do { \
        mbedtls_test_memory_poisoning_count++; \
        mbedtls_test_memory_poison(ptr, size); \
    } while (0)

/** Undo the effect of mbedtls_test_memory_poison().
 *
 * This is a no-op if the given area is entirely valid, unpoisoned memory.
 *
 * The behavior is undefined if any part of the memory area is invalid,
 * or if the memory area contains a mixture of poisoned and unpoisoned parts.
 */
void mbedtls_test_memory_unpoison(const unsigned char *ptr, size_t size);
#define MBEDTLS_TEST_MEMORY_UNPOISON(ptr, size)    \
    do { \
        mbedtls_test_memory_unpoison(ptr, size); \
        if (mbedtls_test_memory_poisoning_count != 0) { \
            mbedtls_test_memory_poisoning_count--; \
        } \
    } while (0)

#else /* MBEDTLS_TEST_MEMORY_CAN_POISON */
#define MBEDTLS_TEST_MEMORY_POISON(ptr, size) ((void) (ptr), (void) (size))
#define MBEDTLS_TEST_MEMORY_UNPOISON(ptr, size) ((void) (ptr), (void) (size))
#endif /* MBEDTLS_TEST_MEMORY_CAN_POISON */

#endif /* TEST_MEMORY_H */
