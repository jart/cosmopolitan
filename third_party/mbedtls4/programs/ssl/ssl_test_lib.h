/*
 *  Common code for SSL test programs
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef MBEDTLS_PROGRAMS_SSL_SSL_TEST_LIB_H
#define MBEDTLS_PROGRAMS_SSL_SSL_TEST_LIB_H

/* On Mingw-w64, force the use of a C99-compliant printf() and friends.
 * This is necessary on older versions of Mingw and/or Windows runtimes
 * where snprintf does not always zero-terminate the buffer, and does
 * not support formats such as "%zu" for size_t and "%lld" for long long.
 */
#if !defined(__USE_MINGW_ANSI_STDIO)
#define __USE_MINGW_ANSI_STDIO 1
#endif

/* Tell MSVC that we're ok with using classic C functions even
 * when an `_s` variant exist. For most functions, the improvements
 * of the `_s` variants are of limited usefulness and not worth
 * the portability headaches.
 */
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_DEPRECATE)
#define _CRT_SECURE_NO_DEPRECATE 1
#endif

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "mbedtls/private/pk_private.h"

#include "mbedtls/build_info.h"

#include "mbedtls/platform.h"
#include "mbedtls/md.h"

#undef HAVE_RNG
#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
#define HAVE_RNG
#elif defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_CTR_DRBG_C)
#define HAVE_RNG
#elif defined(MBEDTLS_ENTROPY_C) && defined(MBEDTLS_HMAC_DRBG_C) &&     \
    (defined(PSA_WANT_ALG_SHA_256) || defined(PSA_WANT_ALG_SHA_512))
#define HAVE_RNG
#endif

#if !defined(MBEDTLS_NET_C) ||                              \
    !defined(MBEDTLS_SSL_TLS_C)
#define MBEDTLS_SSL_TEST_IMPOSSIBLE                         \
    "MBEDTLS_NET_C and/or "                                 \
    "MBEDTLS_SSL_TLS_C not defined."
#elif !defined(HAVE_RNG)
#define MBEDTLS_SSL_TEST_IMPOSSIBLE                         \
    "No random generator is available.\n"
#else
#undef MBEDTLS_SSL_TEST_IMPOSSIBLE

#undef HAVE_RNG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/ssl_ciphersuites.h"
#include "mbedtls/x509.h"
#include "mbedtls/error.h"
#include "mbedtls/debug.h"
#include "mbedtls/timing.h"
#include "mbedtls/base64.h"
#include "test/certs.h"

#include "psa/crypto.h"
#include "mbedtls/psa_util.h"

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
#include "mbedtls/memory_buffer_alloc.h"
#endif

#include <test/helpers.h>

#include "query_config.h"

#define ALPN_LIST_SIZE    10
#define GROUP_LIST_SIZE   25
#define SIG_ALG_LIST_SIZE  5

typedef struct eap_tls_keys {
    unsigned char master_secret[48];
    unsigned char randbytes[64];
    mbedtls_tls_prf_types tls_prf_type;
} eap_tls_keys;

#if defined(MBEDTLS_SSL_DTLS_SRTP)

/* Supported SRTP mode needs a maximum of :
 * - 16 bytes for key (AES-128)
 * - 14 bytes SALT
 * One for sender, one for receiver context
 */
#define MBEDTLS_TLS_SRTP_MAX_KEY_MATERIAL_LENGTH    60

typedef struct dtls_srtp_keys {
    unsigned char master_secret[48];
    unsigned char randbytes[64];
    mbedtls_tls_prf_types tls_prf_type;
} dtls_srtp_keys;

#endif /* MBEDTLS_SSL_DTLS_SRTP */

typedef struct {
    mbedtls_ssl_context *ssl;
    mbedtls_net_context *net;
} io_ctx_t;

void my_debug(void *ctx, int level,
              const char *file, int line,
              const char *str);

#if defined(MBEDTLS_HAVE_TIME)
mbedtls_time_t dummy_constant_time(mbedtls_time_t *time);
#endif

/* Seed the random number generator.
 *
 * \param rng           The RNG context to use. It must have been initialized
 *                      with rng_init().
 * \param reproducible  If zero, seed the RNG from entropy.
 *                      If nonzero, use a fixed seed, so that the program
 *                      will produce the same sequence of random numbers
 *                      each time it is invoked.
 * \param pers          A null-terminated string. Different values for this
 *                      string cause the RNG to emit different output for
 *                      the same seed.
 *
 * return 0 on success, a negative value on error.
 */
int rng_seed(int reproducible, const char *pers);

/** Parse command-line option: key_opaque_algs
 *
 *
 * \param arg           String value of key_opaque_algs
 *                      Coma-separated pair of values among the following:
 *                      - "rsa-sign-pkcs1"
 *                      - "rsa-sign-pss"
 *                      - "ecdsa-sign"
 *                      - "ecdh"
 *                      - "none" (only acceptable for the second value).
 * \param alg1          Address of pointer to alg #1
 * \param alg2          Address of pointer to alg #2
 *
 * \return              \c 0 on success.
 * \return              \c 1 on parse failure.
 */
int key_opaque_alg_parse(const char *arg, const char **alg1, const char **alg2);

/** Parse given opaque key algorithms to obtain psa algs and usage
 *  that will be passed to mbedtls_pk_wrap_as_opaque().
 *
 *
 * \param alg1          input string opaque key algorithm #1
 * \param alg2          input string opaque key algorithm #2
 * \param psa_alg1      output PSA algorithm #1
 * \param psa_alg2      output PSA algorithm #2
 * \param usage         output key usage
 * \param key_type      key type used to set default psa algorithm/usage
 *                      when alg1 in "none"
 *
 * \return              \c 0 on success.
 * \return              \c 1 on parse failure.
 */
int key_opaque_set_alg_usage(const char *alg1, const char *alg2,
                             psa_algorithm_t *psa_alg1,
                             psa_algorithm_t *psa_alg2,
                             psa_key_usage_t *usage,
                             mbedtls_pk_type_t key_type);

#if defined(MBEDTLS_PK_C)
/** Turn a non-opaque PK context into an opaque one with folowing steps:
 * - extract the key data and attributes from the PK context.
 * - import the key material into PSA.
 * - free the provided PK context and re-initilize it as an opaque PK context
 *   wrapping the PSA key imported in the above step.
 *
 * \param[in,out] pk    On input, the non-opaque PK context which contains the
 *                      key to be wrapped. On output, the re-initialized PK
 *                      context which represents the opaque version of the one
 *                      provided as input.
 * \param[in] psa_alg   The primary algorithm that will be associated to the
 *                      PSA key.
 * \param[in] psa_alg2  The enrollment algorithm that will be associated to the
 *                      PSA key.
 * \param[in] psa_usage The PSA key usage policy.
 * \param[out] key_id   The PSA key identifier of the imported key.
 *
 * \return              \c 0 on sucess.
 * \return              \c -1 on failure.
 */
int pk_wrap_as_opaque(mbedtls_pk_context *pk, psa_algorithm_t psa_alg, psa_algorithm_t psa_alg2,
                      psa_key_usage_t psa_usage, mbedtls_svc_key_id_t *key_id);
#endif /* MBEDTLS_PK_C */

#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
/* The test implementation of the PSA external RNG is insecure. When
 * MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG is enabled, before using any PSA crypto
 * function that makes use of an RNG, you must call
 * mbedtls_test_enable_insecure_external_rng(). */
#include <test/fake_external_rng_for_test.h>
#endif

#if defined(MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK)
int ca_callback(void *data, mbedtls_x509_crt const *child,
                mbedtls_x509_crt **candidates);
#endif /* MBEDTLS_X509_TRUSTED_CERTIFICATE_CALLBACK */

/*
 * Test recv/send functions that make sure each try returns
 * WANT_READ/WANT_WRITE at least once before succeeding
 */
int delayed_recv(void *ctx, unsigned char *buf, size_t len);
int delayed_send(void *ctx, const unsigned char *buf, size_t len);

/*
 * Wait for an event from the underlying transport or the timer
 * (Used in event-driven IO mode).
 */
int idle(mbedtls_net_context *fd,
#if defined(MBEDTLS_TIMING_C)
         mbedtls_timing_delay_context *timer,
#endif
         int idle_reason);

#if defined(MBEDTLS_TEST_HOOKS)
/** Initialize whatever test hooks are enabled by the compile-time
 * configuration and make sense for the TLS test programs. */
void test_hooks_init(void);

/** Check if any test hooks detected a problem.
 *
 * If a problem was detected, it's ok for the calling program to keep going,
 * but it should ultimately exit with an error status.
 *
 * \note When implementing a test hook that detects errors on its own
 *       (as opposed to e.g. leaving the error for a memory sanitizer to
 *       report), make sure to print a message to standard error either at
 *       the time the problem is detected or during the execution of this
 *       function. This function does not indicate what problem was detected,
 *       so printing a message is the only way to provide feedback in the
 *       logs of the calling program.
 *
 * \return Nonzero if a problem was detected.
 *         \c 0 if no problem was detected.
 */
int test_hooks_failure_detected(void);

/** Free any resources allocated for the sake of test hooks.
 *
 * Call this at the end of the program so that resource leak analyzers
 * don't complain.
 */
void test_hooks_free(void);

#endif /* !MBEDTLS_TEST_HOOKS */

/* Helper functions for FFDH groups. */
int parse_groups(const char *groups, uint16_t *group_list, size_t group_list_len);

#endif /* MBEDTLS_SSL_TEST_IMPOSSIBLE conditions: else */
#endif /* MBEDTLS_PROGRAMS_SSL_SSL_TEST_LIB_H */
