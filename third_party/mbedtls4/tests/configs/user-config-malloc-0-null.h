/* crypto_config.h modifier that forces calloc(0) to return NULL.
 * Used for testing.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <stdlib.h>

#ifndef MBEDTLS_PLATFORM_STD_CALLOC
static inline void *custom_calloc(size_t nmemb, size_t size)
{
    if (nmemb == 0 || size == 0) {
        return NULL;
    }
    return calloc(nmemb, size);
}

#define MBEDTLS_PLATFORM_MEMORY
#define MBEDTLS_PLATFORM_STD_CALLOC custom_calloc
#endif
