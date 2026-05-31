/* crypto_config.h modifier that defines mbedtls_platform_zeroize() to be
 * memset(), so that the compile can check arguments for us.
 * Used for testing.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <string.h>

/* Define _ALT so we don't get the built-in implementation. The test code will
 * also need to define MBEDTLS_TEST_DEFINES_ZEROIZE so we don't get the
 * declaration. */
#define MBEDTLS_PLATFORM_ZEROIZE_ALT

#define mbedtls_platform_zeroize(buf, len) memset(buf, 0, len)
