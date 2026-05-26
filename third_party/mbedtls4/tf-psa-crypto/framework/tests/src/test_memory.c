/**
 * \file memory.c
 *
 * \brief   Helper functions related to testing memory management.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "test_common.h"
#include <test/helpers.h>
#include <test/macros.h>
#include <test/memory.h>

#if defined(MBEDTLS_TEST_MEMORY_CAN_POISON)
#include <sanitizer/asan_interface.h>
#include <stdint.h>
#endif

#if defined(MBEDTLS_TEST_MEMORY_CAN_POISON)

_Thread_local unsigned int mbedtls_test_memory_poisoning_count = 0;

static void align_for_asan(const unsigned char **p_ptr, size_t *p_size)
{
    uintptr_t start = (uintptr_t) *p_ptr;
    uintptr_t end = start + (uintptr_t) *p_size;
    /* ASan can only poison regions with 8-byte alignment, and only poisons a
     * region if it's fully within the requested range. We want to poison the
     * whole requested region and don't mind a few extra bytes. Therefore,
     * align start down to an 8-byte boundary, and end up to an 8-byte
     * boundary. */
    start = start & ~(uintptr_t) 7;
    end = (end + 7) & ~(uintptr_t) 7;
    *p_ptr = (const unsigned char *) start;
    *p_size = end - start;
}

void mbedtls_test_memory_poison(const unsigned char *ptr, size_t size)
{
    if (mbedtls_test_memory_poisoning_count == 0) {
        return;
    }
    if (size == 0) {
        return;
    }
    align_for_asan(&ptr, &size);
    __asan_poison_memory_region(ptr, size);
}

void mbedtls_test_memory_unpoison(const unsigned char *ptr, size_t size)
{
    if (size == 0) {
        return;
    }
    align_for_asan(&ptr, &size);
    __asan_unpoison_memory_region(ptr, size);
}
#endif /* Memory poisoning */
