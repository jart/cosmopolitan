/**
 * \file config-symmetric-only.h
 *
 * \brief Configuration without any asymmetric cryptography.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

/* Mbed TLS feature support */
#define MBEDTLS_ERROR_STRERROR_DUMMY
#define MBEDTLS_VERSION_FEATURES

#define MBEDTLS_TIMING_C //Only for benchmarking
#define MBEDTLS_VERSION_C
