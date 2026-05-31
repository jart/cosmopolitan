/**
 * \file test_common.h
 *
 * \brief Common things for all Mbed TLS and TF-PSA-Crypto test code.
 *
 * Include this header first in all test C files.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef TEST_TEST_COMMON_H
#define TEST_TEST_COMMON_H

#if !defined(_POSIX_C_SOURCE)
/* For standards-compliant access to
 * clock_gettime(), gmtime_r(), ...
 */
#define _POSIX_C_SOURCE 200112L
#endif

/* With GNU libc, define all the things, even when compiling with -pedantic. */
#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

/* On Mingw-w64, force the use of a C99-compliant printf() and friends.
 * This is necessary on older versions of Mingw and/or Windows runtimes
 * where snprintf does not always zero-terminate the buffer, and does
 * not support formats such as "%zu" for size_t and "%lld" for long long.
 */
#if !defined(__USE_MINGW_ANSI_STDIO)
#define __USE_MINGW_ANSI_STDIO 1
#endif

#if defined(__IAR_SYSTEMS_ICC__)
/* With IAR, enable support for ::FILE functions in stdio.h.
 */
#define _DLIB_FILE_DESCRIPTOR 1
#endif

/* Make sure we have the library configuration, and anything else that
 * is deemed necessary in test headers. */
#include <test/build_info.h>

/* Give test code access to internal macros of the library. */
#if defined(TF_PSA_CRYPTO_VERSION_NUMBER)
#include "tf_psa_crypto_common.h"
#else
#include "common.h"
#endif

#endif /* TEST_TEST_COMMON_H */
