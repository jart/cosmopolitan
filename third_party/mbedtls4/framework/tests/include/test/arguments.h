/**
 * \file arguments.h
 *
 * \brief Manipulation of test arguments.
 *
 * Much of the code is in host_test.function, to be migrated here later.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef TEST_ARGUMENTS_H
#define TEST_ARGUMENTS_H

#include "build_info.h"
#include <stdint.h>
#include <stdlib.h>

typedef union {
    size_t len;
    intmax_t sint;
} mbedtls_test_argument_t;

#endif /* TEST_ARGUMENTS_H */
