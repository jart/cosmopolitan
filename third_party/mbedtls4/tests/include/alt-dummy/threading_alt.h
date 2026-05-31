/* threading_alt.h with dummy types for MBEDTLS_THREADING_ALT */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef THREADING_ALT_H
#define THREADING_ALT_H

typedef struct mbedtls_threading_mutex_t {
    int dummy;
} mbedtls_threading_mutex_t;

#endif /* threading_alt.h */
