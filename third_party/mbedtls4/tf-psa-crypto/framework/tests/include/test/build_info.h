/**
 * \file test/build_info.h
 *
 * \brief Common things for all Mbed TLS and TF-PSA-Crypto test headers.
 *
 * Include this header first in all headers in `include/test/`.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef TEST_BUILD_INFO_H
#define TEST_BUILD_INFO_H

#include <mbedtls/build_info.h>

/* Most fields of publicly available structs are private and are wrapped with
 * MBEDTLS_PRIVATE macro. This define allows tests to access the private fields
 * directly (without using the MBEDTLS_PRIVATE wrapper). */
#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#endif /* TEST_BUILD_INFO_H */
