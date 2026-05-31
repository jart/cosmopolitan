/*
 * Mbed TLS configuration for PSA test driver libraries. It includes:
 * . the minimum set of modules needed by the PSA core.
 * . the Mbed TLS configuration options that may need to be additionally
 *   enabled for the purpose of a specific test.
 * . the PSA configuration file for the Mbed TLS library and its test drivers.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#endif /* MBEDTLS_CONFIG_H */
