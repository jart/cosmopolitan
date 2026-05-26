/*
 * Helper functions for tests that use the PSA Crypto API.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef PSA_CRYPTO_HELPERS_H
#define PSA_CRYPTO_HELPERS_H

#include "build_info.h"
#include "test/helpers.h"

#if (MBEDTLS_VERSION_MAJOR < 4 && defined(MBEDTLS_PSA_CRYPTO_C)) || \
    (MBEDTLS_VERSION_MAJOR >= 4 && defined(MBEDTLS_PSA_CRYPTO_CLIENT))
#include "test/psa_helpers.h"
#endif

#include <psa/crypto.h>

#if !defined(MBEDTLS_VERSION_MAJOR) || MBEDTLS_VERSION_MAJOR >= 4
#include <mbedtls/private/ctr_drbg.h>
#else
#include <mbedtls/ctr_drbg.h>
#endif

#if defined(MBEDTLS_PSA_CRYPTO_C)
/** Initialize the PSA Crypto subsystem. */
#define PSA_INIT() PSA_ASSERT(psa_crypto_init())

/** Shut down the PSA Crypto subsystem and destroy persistent keys.
 * Expect a clean shutdown, with no slots in use.
 *
 * If some key slots are still in use, record the test case as failed,
 * but continue executing. This macro is suitable (and primarily intended)
 * for use in the cleanup section of test functions.
 *
 * \note Persistent keys must be recorded with #TEST_USES_KEY_ID before
 *       creating them.
 */
#define PSA_DONE()                                                      \
    do                                                                  \
    {                                                                   \
        mbedtls_test_fail_if_psa_leaking(__LINE__, __FILE__);           \
        mbedtls_test_psa_purge_key_storage();                           \
        mbedtls_psa_crypto_free();                                      \
    }                                                                   \
    while (0)
#elif MBEDTLS_VERSION_MAJOR >= 4 && defined(MBEDTLS_PSA_CRYPTO_CLIENT)
#define PSA_INIT() PSA_ASSERT(psa_crypto_init())
#define PSA_DONE() mbedtls_psa_crypto_free();
#else  /* MBEDTLS_PSA_CRYPTO_CLIENT && !MBEDTLS_PSA_CRYPTO_C */
#define PSA_INIT() ((void) 0)
#define PSA_DONE() ((void) 0)
#endif /* MBEDTLS_PSA_CRYPTO_C */

#if (MBEDTLS_VERSION_MAJOR < 4 && defined(MBEDTLS_PSA_CRYPTO_C)) || \
    (MBEDTLS_VERSION_MAJOR >= 4 && defined(MBEDTLS_PSA_CRYPTO_CLIENT))

#if defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)

/* Internal function for #TEST_USES_KEY_ID. Return 1 on success, 0 on failure. */
int mbedtls_test_uses_key_id(mbedtls_svc_key_id_t key_id);

/** Destroy persistent keys recorded with #TEST_USES_KEY_ID.
 */
void mbedtls_test_psa_purge_key_storage(void);

/** Purge the in-memory cache of persistent keys recorded with
 * #TEST_USES_KEY_ID.
 *
 * Call this function before calling PSA_DONE() if it's ok for
 * persistent keys to still exist at this point.
 */
void mbedtls_test_psa_purge_key_cache(void);

/** \def TEST_USES_KEY_ID
 *
 * Call this macro in a test function before potentially creating a
 * persistent key. Test functions that use this mechanism must call
 * mbedtls_test_psa_purge_key_storage() in their cleanup code.
 *
 * This macro records a persistent key identifier as potentially used in the
 * current test case. Recorded key identifiers will be cleaned up at the end
 * of the test case, even on failure.
 *
 * This macro has no effect on volatile keys. Therefore, it is safe to call
 * this macro in a test function that creates either volatile or persistent
 * keys depending on the test data.
 *
 * This macro currently has no effect on special identifiers
 * used to store implementation-specific files.
 *
 * Calling this macro multiple times on the same key identifier in the same
 * test case has no effect.
 *
 * This macro can fail the test case if there isn't enough memory to
 * record the key id.
 *
 * \param key_id    The PSA key identifier to record.
 */
#define TEST_USES_KEY_ID(key_id)                      \
    TEST_ASSERT(mbedtls_test_uses_key_id(key_id))

#else /* MBEDTLS_PSA_CRYPTO_STORAGE_C */

#define TEST_USES_KEY_ID(key_id) ((void) (key_id))
#define mbedtls_test_psa_purge_key_storage() ((void) 0)
#define mbedtls_test_psa_purge_key_cache() ((void) 0)

#endif /* MBEDTLS_PSA_CRYPTO_STORAGE_C */

/** Check for things that have not been cleaned up properly in the
 * PSA subsystem.
 *
 * \return NULL if nothing has leaked.
 * \return A string literal explaining what has not been cleaned up
 *         if applicable.
 */
const char *mbedtls_test_helper_is_psa_leaking(void);

/** Check that no PSA Crypto key slots are in use.
 *
 * If any slots are in use, mark the current test as failed and jump to
 * the exit label. This is equivalent to
 * `TEST_ASSERT( ! mbedtls_test_helper_is_psa_leaking( ) )`
 * but with a more informative message.
 */
#define ASSERT_PSA_PRISTINE()                                           \
    do                                                                  \
    {                                                                   \
        if (mbedtls_test_fail_if_psa_leaking(__LINE__, __FILE__))       \
        goto exit;                                                      \
    }                                                                   \
    while (0)

/** Shut down the PSA Crypto subsystem, allowing persistent keys to survive.
 * Expect a clean shutdown, with no slots in use.
 *
 * If some key slots are still in use, record the test case as failed and
 * jump to the `exit` label.
 */
#define PSA_SESSION_DONE()                                             \
    do                                                                  \
    {                                                                   \
        mbedtls_test_psa_purge_key_cache();                            \
        ASSERT_PSA_PRISTINE();                                         \
        mbedtls_psa_crypto_free();                                     \
    }                                                                   \
    while (0)


/** Initializer that doesn't set the embedded union to zero.
 *
 * Use this to validate that our code correctly handles platforms where
 * `{0}` does not initialize a union to all-bits-zero, only the first member.
 * Such behavior is uncommon, but compliant (see discussion in
 * https://github.com/Mbed-TLS/mbedtls/issues/9814).
 * You can portably simulate that behavior by using the `xxx_init_short()`
 * initializer function instead of `{0}` or an official initializer
 * `xxx_init()` or `XXX_INIT`.
 */
psa_hash_operation_t psa_hash_operation_init_short(void);
psa_mac_operation_t psa_mac_operation_init_short(void);
psa_cipher_operation_t psa_cipher_operation_init_short(void);
psa_aead_operation_t psa_aead_operation_init_short(void);
psa_key_derivation_operation_t psa_key_derivation_operation_init_short(void);
psa_pake_operation_t psa_pake_operation_init_short(void);
psa_sign_hash_interruptible_operation_t psa_sign_hash_interruptible_operation_init_short(void);
psa_verify_hash_interruptible_operation_t psa_verify_hash_interruptible_operation_init_short(void);
#if defined(PSA_KEY_AGREEMENT_IOP_INIT)
psa_key_agreement_iop_t psa_key_agreement_iop_init_short(void);
#endif
#if defined(PSA_GENERATE_KEY_IOP_INIT)
psa_generate_key_iop_t psa_generate_key_iop_init_short(void);
#endif
#if defined(PSA_EXPORT_PUBLIC_KEY_IOP_INIT)
psa_export_public_key_iop_t psa_export_public_key_iop_init_short(void);
#endif



#if defined(RECORD_PSA_STATUS_COVERAGE_LOG)
psa_status_t mbedtls_test_record_status(psa_status_t status,
                                        const char *func,
                                        const char *file, int line,
                                        const char *expr);

/** Return value logging wrapper macro.
 *
 * Evaluate \p expr. Write a line recording its value to the log file
 * #STATUS_LOG_FILE_NAME and return the value. The line is a colon-separated
 * list of fields:
 * ```
 * value of expr:string:__FILE__:__LINE__:expr
 * ```
 *
 * The test code does not call this macro explicitly because that would
 * be very invasive. Instead, we instrument the source code by defining
 * a bunch of wrapper macros like
 * ```
 * #define psa_crypto_init() RECORD_STATUS("psa_crypto_init", psa_crypto_init())
 * ```
 * These macro definitions must be present in `instrument_record_status.h`
 * when building the test suites.
 *
 * \param string    A string, normally a function name.
 * \param expr      An expression to evaluate, normally a call of the function
 *                  whose name is in \p string. This expression must return
 *                  a value of type #psa_status_t.
 * \return          The value of \p expr.
 */
#define RECORD_STATUS(string, expr)                                   \
    mbedtls_test_record_status((expr), string, __FILE__, __LINE__, #expr)

#include "instrument_record_status.h"

#endif /* defined(RECORD_PSA_STATUS_COVERAGE_LOG) */

/** Return extended key usage policies.
 *
 * Do a key policy permission extension on key usage policies always involves
 * permissions of other usage policies
 * (like PSA_KEY_USAGE_SIGN_HASH involves PSA_KEY_USAGE_SIGN_MESSAGE).
 */
psa_key_usage_t mbedtls_test_update_key_usage_flags(psa_key_usage_t usage_flags);

/** Check that no PSA Crypto key slots are in use.
 *
 * If any slots are in use, mark the current test as failed.
 *
 * \return 0 if the key store is empty, 1 otherwise.
 */
int mbedtls_test_fail_if_psa_leaking(int line_no, const char *filename);



#if defined(MBEDTLS_PSA_INJECT_ENTROPY)
/* The #MBEDTLS_PSA_INJECT_ENTROPY feature requires two extra platform
 * functions, which must be configured as #MBEDTLS_PLATFORM_NV_SEED_READ_MACRO
 * and #MBEDTLS_PLATFORM_NV_SEED_WRITE_MACRO. The job of these functions
 * is to read and write from the entropy seed file, which is located
 * in the PSA ITS file whose uid is #PSA_CRYPTO_ITS_RANDOM_SEED_UID.
 * (These could have been provided as library functions, but for historical
 * reasons, they weren't, and so each integrator has to provide a copy
 * of these functions.)
 *
 * Provide implementations of these functions for testing. */
int mbedtls_test_inject_entropy_seed_read(unsigned char *buf, size_t len);
int mbedtls_test_inject_entropy_seed_write(unsigned char *buf, size_t len);


/** Make sure that the injected entropy is present.
 *
 * When MBEDTLS_PSA_INJECT_ENTROPY is enabled, psa_crypto_init()
 * will fail if the PSA entropy seed is not present.
 * This function must be called at least once in a test suite or other
 * program before any call to psa_crypto_init().
 * It does not need to be called in each test case.
 *
 * The test framework calls this function before running any test case.
 *
 * The few tests that might remove the entropy file must call this function
 * in their cleanup.
 */
int mbedtls_test_inject_entropy_restore(void);
#endif /* MBEDTLS_PSA_INJECT_ENTROPY */

/** Parse binary string and convert it to a long integer
 */
uint64_t mbedtls_test_parse_binary_string(data_t *bin_string);

/** Skip a test case if the given key is a 192 bits AES key and the AES
 *  implementation is at least partially provided by an accelerator or
 *  alternative implementation.
 *
 *  Call this macro in a test case when a cryptographic operation that may
 *  involve an AES operation returns a #PSA_ERROR_NOT_SUPPORTED error code.
 *  The macro call will skip and not fail the test case in case the operation
 *  involves a 192 bits AES key and the AES implementation is at least
 *  partially provided by an accelerator or alternative implementation.
 *
 *  Hardware AES implementations not supporting 192 bits keys commonly exist.
 *  Consequently, PSA test cases aim at not failing when an AES operation with
 *  a 192 bits key performed by an alternative AES implementation returns
 *  with the #PSA_ERROR_NOT_SUPPORTED error code. The purpose of this macro
 *  is to facilitate this and make the test case code more readable.
 *
 *  \param key_type  Key type
 *  \param key_bits  Key length in number of bits.
 */
#if defined(MBEDTLS_AES_ALT) || \
    defined(MBEDTLS_AES_SETKEY_ENC_ALT) || \
    defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_AES)
#define MBEDTLS_TEST_HAVE_ACCEL_AES 1
#else
#define MBEDTLS_TEST_HAVE_ACCEL_AES 0
#endif

#define MBEDTLS_TEST_PSA_SKIP_IF_ALT_AES_192(key_type, key_bits)        \
    do                                                                    \
    {                                                                     \
        if ((MBEDTLS_TEST_HAVE_ACCEL_AES) &&                              \
            ((key_type) == PSA_KEY_TYPE_AES) &&                       \
            (key_bits == 192))                                         \
        {                                                                 \
            mbedtls_test_skip("AES-192 not supported", __LINE__, __FILE__);     \
            goto exit;                                                    \
        }                                                                 \
    }                                                                     \
    while (0)

/** Skip a test case if a GCM operation with a nonce length different from
 *  12 bytes fails and was performed by an accelerator or alternative
 *  implementation.
 *
 *  Call this macro in a test case when an AEAD cryptography operation that
 *  may involve the GCM mode returns with a #PSA_ERROR_NOT_SUPPORTED error
 *  code. The macro call will skip and not fail the test case in case the
 *  operation involves the GCM mode, a nonce with a length different from
 *  12 bytes and the GCM mode implementation is an alternative one.
 *
 *  Hardware GCM implementations not supporting nonce lengths different from
 *  12 bytes commonly exist, as supporting a non-12-byte nonce requires
 *  additional computations involving the GHASH function.
 *  Consequently, PSA test cases aim at not failing when an AEAD operation in
 *  GCM mode with a nonce length different from 12 bytes is performed by an
 *  alternative GCM implementation and returns with a #PSA_ERROR_NOT_SUPPORTED
 *  error code. The purpose of this macro is to facilitate this check and make
 *  the test case code more readable.
 *
 *  \param  alg             The AEAD algorithm.
 *  \param  nonce_length    The nonce length in number of bytes.
 */

#if defined(MBEDTLS_GCM_ALT) || \
    defined(MBEDTLS_PSA_ACCEL_ALG_GCM)
#define MBEDTLS_TEST_HAVE_ACCEL_GCM  1
#else
#define MBEDTLS_TEST_HAVE_ACCEL_GCM  0
#endif

#define MBEDTLS_TEST_PSA_SKIP_IF_ALT_GCM_NOT_12BYTES_NONCE(alg,           \
                                                           nonce_length) \
    do                                                                     \
    {                                                                      \
        if ((MBEDTLS_TEST_HAVE_ACCEL_GCM) &&                               \
            (PSA_ALG_AEAD_WITH_SHORTENED_TAG((alg), 0) ==            \
             PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, 0)) &&       \
            ((nonce_length) != 12))                                   \
        {                                                                  \
            mbedtls_test_skip("GCM with non-12-byte IV is not supported", __LINE__, __FILE__); \
            goto exit;                                                     \
        }                                                                  \
    }                                                                      \
    while (0)

#endif /* MBEDTLS_PSA_CRYPTO_CLIENT || MBEDTLS_PSA_CRYPTO_C */

#if MBEDTLS_VERSION_MAJOR >= 4
/* Legacy PSA_INIT() / PSA_DONE() variants from 3.6 */
#define USE_PSA_INIT()          PSA_INIT()
#define USE_PSA_DONE()          PSA_DONE()
#define MD_PSA_INIT()           PSA_INIT()
#define MD_PSA_DONE()           PSA_DONE()
#define BLOCK_CIPHER_PSA_INIT() PSA_INIT()
#define BLOCK_CIPHER_PSA_DONE() PSA_DONE()
#define MD_OR_USE_PSA_INIT()    PSA_INIT()
#define MD_OR_USE_PSA_DONE()    PSA_DONE()
#define AES_PSA_INIT()          PSA_INIT()
#define AES_PSA_DONE()          PSA_DONE()

#else /* MBEDTLS_VERSION_MAJOR < 4 */

/** \def USE_PSA_INIT
 *
 * Call this macro to initialize the PSA subsystem if #MBEDTLS_USE_PSA_CRYPTO
 * or #MBEDTLS_SSL_PROTO_TLS1_3 (In contrast to TLS 1.2 implementation, the
 * TLS 1.3 one uses PSA independently of the definition of
 * #MBEDTLS_USE_PSA_CRYPTO) is enabled and do nothing otherwise.
 *
 * If the initialization fails, mark the test case as failed and jump to the
 * \p exit label.
 */
/** \def USE_PSA_DONE
 *
 * Call this macro at the end of a test case if you called #USE_PSA_INIT.
 *
 * This is like #PSA_DONE except it does nothing under the same conditions as
 * #USE_PSA_INIT.
 */
#if defined(MBEDTLS_USE_PSA_CRYPTO)
#define USE_PSA_INIT() PSA_INIT()
#define USE_PSA_DONE() PSA_DONE()
#elif defined(MBEDTLS_SSL_PROTO_TLS1_3)
/* TLS 1.3 must work without having called psa_crypto_init(), for backward
 * compatibility with Mbed TLS <= 3.5 when connecting with a peer that
 * supports both TLS 1.2 and TLS 1.3. See mbedtls_ssl_tls13_crypto_init()
 * and https://github.com/Mbed-TLS/mbedtls/issues/9072 . */
#define USE_PSA_INIT() ((void) 0)
/* TLS 1.3 may have initialized the PSA subsystem. Shut it down cleanly,
 * otherwise Asan and Valgrind would notice a resource leak. */
#define USE_PSA_DONE() PSA_DONE()
#else /* MBEDTLS_USE_PSA_CRYPTO || MBEDTLS_SSL_PROTO_TLS1_3 */
/* Define empty macros so that we can use them in the preamble and teardown
 * of every test function that uses PSA conditionally based on
 * MBEDTLS_USE_PSA_CRYPTO. */
#define USE_PSA_INIT() ((void) 0)
#define USE_PSA_DONE() ((void) 0)
#endif /* !MBEDTLS_USE_PSA_CRYPTO && !MBEDTLS_SSL_PROTO_TLS1_3 */

/** \def MD_PSA_INIT
 *
 * Call this macro to initialize the PSA subsystem if MD uses a driver,
 * and do nothing otherwise.
 *
 * If the initialization fails, mark the test case as failed and jump to the
 * \p exit label.
 */
/** \def MD_PSA_DONE
 *
 * Call this macro at the end of a test case if you called #MD_PSA_INIT.
 *
 * This is like #PSA_DONE except it does nothing under the same conditions as
 * #MD_PSA_INIT.
 */
#if defined(MBEDTLS_MD_SOME_PSA)
#define MD_PSA_INIT()   PSA_INIT()
#define MD_PSA_DONE()   PSA_DONE()
#else /* MBEDTLS_MD_SOME_PSA */
#define MD_PSA_INIT() ((void) 0)
#define MD_PSA_DONE() ((void) 0)
#endif /* MBEDTLS_MD_SOME_PSA */

/** \def BLOCK_CIPHER_PSA_INIT
 *
 * Call this macro to initialize the PSA subsystem if BLOCK_CIPHER uses a driver,
 * and do nothing otherwise.
 *
 * If the initialization fails, mark the test case as failed and jump to the
 * \p exit label.
 */
/** \def BLOCK_CIPHER_PSA_DONE
 *
 * Call this macro at the end of a test case if you called #BLOCK_CIPHER_PSA_INIT.
 *
 * This is like #PSA_DONE except it does nothing under the same conditions as
 * #BLOCK_CIPHER_PSA_INIT.
 */
#if defined(MBEDTLS_BLOCK_CIPHER_SOME_PSA)
#define BLOCK_CIPHER_PSA_INIT()   PSA_INIT()
#define BLOCK_CIPHER_PSA_DONE()   PSA_DONE()
#else /* MBEDTLS_MD_SOME_PSA */
#define BLOCK_CIPHER_PSA_INIT() ((void) 0)
#define BLOCK_CIPHER_PSA_DONE() ((void) 0)
#endif /* MBEDTLS_MD_SOME_PSA */


/** \def MD_OR_USE_PSA_INIT
 *
 * Call this macro to initialize the PSA subsystem if MD uses a driver,
 * or if #MBEDTLS_USE_PSA_CRYPTO or #MBEDTLS_SSL_PROTO_TLS1_3 is enabled,
 * and do nothing otherwise.
 *
 * If the initialization fails, mark the test case as failed and jump to the
 * \p exit label.
 */
/** \def MD_OR_USE_PSA_DONE
 *
 * Call this macro at the end of a test case if you called #MD_OR_USE_PSA_INIT.
 *
 * This is like #PSA_DONE except it does nothing under the same conditions as
 * #MD_OR_USE_PSA_INIT.
 */
#if defined(MBEDTLS_MD_SOME_PSA)
#define MD_OR_USE_PSA_INIT()   PSA_INIT()
#define MD_OR_USE_PSA_DONE()   PSA_DONE()
#else
#define MD_OR_USE_PSA_INIT()   USE_PSA_INIT()
#define MD_OR_USE_PSA_DONE()   USE_PSA_DONE()
#endif

/** \def AES_PSA_INIT
 *
 * Call this macro to initialize the PSA subsystem if AES_C is not defined,
 * so that CTR_DRBG uses PSA implementation to get AES-ECB.
 *
 * If the initialization fails, mark the test case as failed and jump to the
 * \p exit label.
 */
/** \def AES_PSA_DONE
 *
 * Call this macro at the end of a test case if you called #AES_PSA_INIT.
 *
 * This is like #PSA_DONE except it does nothing under the same conditions as
 * #AES_PSA_INIT.
 */
#if defined(MBEDTLS_CTR_DRBG_USE_PSA_CRYPTO)
#define AES_PSA_INIT()   PSA_INIT()
#define AES_PSA_DONE()   PSA_DONE()
#else /* MBEDTLS_CTR_DRBG_USE_PSA_CRYPTO */
#define AES_PSA_INIT() ((void) 0)
#define AES_PSA_DONE() ((void) 0)
#endif /* MBEDTLS_CTR_DRBG_USE_PSA_CRYPTO */

#endif /* MBEDTLS_VERSION_MAJOR >= 4 */

#if !defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG) &&                        \
    defined(MBEDTLS_CTR_DRBG_C) &&                                      \
    defined(MBEDTLS_CTR_DRBG_USE_PSA_CRYPTO)
/* When AES_C is not defined and PSA does not have an external RNG,
 * then CTR_DRBG uses PSA to perform AES-ECB. In this scenario 1 key
 * slot is used internally from PSA to hold the AES key and it should
 * not be taken into account when evaluating remaining open slots. */
#define MBEDTLS_TEST_PSA_INTERNAL_KEYS_FOR_DRBG 1
#else
#define MBEDTLS_TEST_PSA_INTERNAL_KEYS_FOR_DRBG 0
#endif

/** The number of volatile keys that PSA crypto uses internally.
 *
 * We expect that many volatile keys to be in use after a successful
 * psa_crypto_init().
 */
#define MBEDTLS_TEST_PSA_INTERNAL_KEYS          \
    MBEDTLS_TEST_PSA_INTERNAL_KEYS_FOR_DRBG

/* A couple of helper macros to verify if MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE is
 * large enough to contain an RSA key pair of the given size. This is meant to be
 * used in test cases where MBEDTLS_PSA_STATIC_KEY_SLOTS is enabled. */
#if defined(MBEDTLS_PSA_CRYPTO_CLIENT)

#if (MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE >= PSA_KEY_EXPORT_RSA_KEY_PAIR_MAX_SIZE(4096))
#define MBEDTLS_TEST_STATIC_KEY_SLOTS_SUPPORT_RSA_4096
#endif

#if (MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE >= PSA_KEY_EXPORT_RSA_KEY_PAIR_MAX_SIZE(2048))
#define MBEDTLS_TEST_STATIC_KEY_SLOTS_SUPPORT_RSA_2048
#endif

#endif /* MBEDTLS_PSA_CRYPTO_CLIENT */

/* Helper macro to get the size of the each key slot buffer. */
#if defined(MBEDTLS_PSA_STATIC_KEY_SLOTS)
#define MBEDTLS_PSA_KEY_BUFFER_MAX_SIZE     MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE
#else
#define MBEDTLS_PSA_KEY_BUFFER_MAX_SIZE     SIZE_MAX
#endif

/* Helper macro for the PK module to check whether MBEDTLS_PSA_STATIC_KEY_SLOT_BUFFER_SIZE
 * is large enough to contain 4096-bit RSA key pairs. Of course this check is only
 * necessary if PK relies on PSA (i.e. MBEDTLS_USE_PSA_CRYPTO) to store and manage
 * the key. */
#if defined(MBEDTLS_USE_PSA_CRYPTO)

#if !defined(MBEDTLS_PSA_STATIC_KEY_SLOTS) || \
    defined(MBEDTLS_TEST_STATIC_KEY_SLOTS_SUPPORT_RSA_4096)
#define MBEDTLS_TEST_PK_ALLOW_RSA_KEY_PAIR_4096
#endif

#else /* MBEDTLS_USE_PSA_CRYPTO */

#define MBEDTLS_TEST_PK_ALLOW_RSA_KEY_PAIR_4096

#endif /* MBEDTLS_USE_PSA_CRYPTO */

#endif /* PSA_CRYPTO_HELPERS_H */
