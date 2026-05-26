/** Code to exercise a PSA key object, i.e. validate that it seems well-formed
 * and can do what it is supposed to do.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef PSA_EXERCISE_KEY_H
#define PSA_EXERCISE_KEY_H

#include "build_info.h"
#include "test/helpers.h"
#include "test/psa_crypto_helpers.h"

#include <psa/crypto.h>

#if defined(MBEDTLS_PK_C)
#include <mbedtls/pk.h>
#if defined(MBEDTLS_PK_HAVE_PRIVATE_HEADER)
#include <mbedtls/private/pk_private.h>
#endif /* MBEDTLS_PK_HAVE_PRIVATE_HEADER */
#endif /* MBEDTLS_PK_C */

/** \def KNOWN_SUPPORTED_HASH_ALG
 *
 * A hash algorithm that is known to be supported.
 *
 * This is used in some smoke tests.
 */
#if defined(PSA_WANT_ALG_SHA_256)
#define KNOWN_SUPPORTED_HASH_ALG PSA_ALG_SHA_256
#elif defined(PSA_WANT_ALG_SHA_384)
#define KNOWN_SUPPORTED_HASH_ALG PSA_ALG_SHA_384
#elif defined(PSA_WANT_ALG_SHA_512)
#define KNOWN_SUPPORTED_HASH_ALG PSA_ALG_SHA_512
#elif defined(PSA_WANT_ALG_SHA3_256)
#define KNOWN_SUPPORTED_HASH_ALG PSA_ALG_SHA3_256
#elif defined(PSA_WANT_ALG_SHA_1)
#define KNOWN_SUPPORTED_HASH_ALG PSA_ALG_SHA_1
#elif defined(PSA_WANT_ALG_MD5)
#define KNOWN_SUPPORTED_HASH_ALG PSA_ALG_MD5
/* PSA_WANT_ALG_RIPEMD160 omitted. This is necessary for the sake of
 * exercise_signature_key() because Mbed TLS doesn't support RIPEMD160
 * in RSA PKCS#1v1.5 signatures. A RIPEMD160-only configuration would be
 * implausible anyway. */
#else
#undef KNOWN_SUPPORTED_HASH_ALG
#endif

/** \def KNOWN_SUPPORTED_BLOCK_CIPHER
 *
 * A block cipher that is known to be supported.
 *
 * For simplicity's sake, stick to block ciphers with 16-byte blocks.
 */
#if defined(PSA_WANT_KEY_TYPE_AES)
#define KNOWN_SUPPORTED_BLOCK_CIPHER PSA_KEY_TYPE_AES
#elif defined(PSA_WANT_KEY_TYPE_ARIA)
#define KNOWN_SUPPORTED_BLOCK_CIPHER PSA_KEY_TYPE_ARIA
#elif defined(PSA_WANT_KEY_TYPE_CAMELLIA)
#define KNOWN_SUPPORTED_BLOCK_CIPHER PSA_KEY_TYPE_CAMELLIA
#else
#undef KNOWN_SUPPORTED_BLOCK_CIPHER
#endif

/** \def KNOWN_SUPPORTED_MAC_ALG
 *
 * A MAC mode that is known to be supported.
 *
 * It must either be HMAC with #KNOWN_SUPPORTED_HASH_ALG or
 * a block cipher-based MAC with #KNOWN_SUPPORTED_BLOCK_CIPHER.
 *
 * This is used in some smoke tests.
 */
#if defined(KNOWN_SUPPORTED_HASH_ALG) && defined(PSA_WANT_ALG_HMAC)
#define KNOWN_SUPPORTED_MAC_ALG (PSA_ALG_HMAC(KNOWN_SUPPORTED_HASH_ALG))
#define KNOWN_SUPPORTED_MAC_KEY_TYPE PSA_KEY_TYPE_HMAC
#elif defined(KNOWN_SUPPORTED_BLOCK_CIPHER) && defined(MBEDTLS_CMAC_C)
#define KNOWN_SUPPORTED_MAC_ALG PSA_ALG_CMAC
#define KNOWN_SUPPORTED_MAC_KEY_TYPE KNOWN_SUPPORTED_BLOCK_CIPHER
#else
#undef KNOWN_SUPPORTED_MAC_ALG
#undef KNOWN_SUPPORTED_MAC_KEY_TYPE
#endif

/** \def KNOWN_SUPPORTED_BLOCK_CIPHER_ALG
 *
 * A cipher algorithm and key type that are known to be supported.
 *
 * This is used in some smoke tests.
 */
#if defined(KNOWN_SUPPORTED_BLOCK_CIPHER) && defined(PSA_WANT_ALG_CTR)
#define KNOWN_SUPPORTED_BLOCK_CIPHER_ALG PSA_ALG_CTR
#elif defined(KNOWN_SUPPORTED_BLOCK_CIPHER) && defined(PSA_WANT_ALG_CBC_NO_PADDING)
#define KNOWN_SUPPORTED_BLOCK_CIPHER_ALG PSA_ALG_CBC_NO_PADDING
#elif defined(KNOWN_SUPPORTED_BLOCK_CIPHER) && defined(PSA_WANT_ALG_CFB)
#define KNOWN_SUPPORTED_BLOCK_CIPHER_ALG PSA_ALG_CFB
#elif defined(KNOWN_SUPPORTED_BLOCK_CIPHER) && defined(PSA_WANT_ALG_OFB)
#define KNOWN_SUPPORTED_BLOCK_CIPHER_ALG PSA_ALG_OFB
#else
#undef KNOWN_SUPPORTED_BLOCK_CIPHER_ALG
#endif
#if defined(KNOWN_SUPPORTED_BLOCK_CIPHER_ALG)
#define KNOWN_SUPPORTED_CIPHER_ALG KNOWN_SUPPORTED_BLOCK_CIPHER_ALG
#define KNOWN_SUPPORTED_CIPHER_KEY_TYPE KNOWN_SUPPORTED_BLOCK_CIPHER
#else
#undef KNOWN_SUPPORTED_CIPHER_ALG
#undef KNOWN_SUPPORTED_CIPHER_KEY_TYPE
#endif

/** Convenience function to set up a key derivation.
 *
 * In case of failure, mark the current test case as failed.
 *
 * The inputs \p input1 and \p input2 are, in order:
 * - HKDF: salt, info.
 * - TKS 1.2 PRF, TLS 1.2 PSK-to-MS: seed, label.
 * - PBKDF2: input cost, salt.
 *
 * \param operation         The operation object to use.
 *                          It must be in the initialized state.
 * \param key               The key to use.
 * \param alg               The algorithm to use.
 * \param input1            The first input to pass.
 * \param input1_length     The length of \p input1 in bytes.
 * \param input2            The first input to pass.
 * \param input2_length     The length of \p input2 in bytes.
 * \param capacity          The capacity to set.
 * \param key_destroyable   If set to 1, a failure due to the key not existing
 *                          or the key being destroyed mid-operation will only
 *                          be reported if the error code is unexpected.
 *
 * \return                  \c 1 on success, \c 0 on failure.
 */
int mbedtls_test_psa_setup_key_derivation_wrap(
    psa_key_derivation_operation_t *operation,
    mbedtls_svc_key_id_t key,
    psa_algorithm_t alg,
    const unsigned char *input1, size_t input1_length,
    const unsigned char *input2, size_t input2_length,
    size_t capacity, int key_destroyable);

/** Perform a key agreement using the given key pair against its public key
 * (not combined with a key derivation).
 *
 * The result is discarded. Thus this function can be used for smoke-testing
 * a key, and to validate input validation, but not to validate results.
 *
 * Depending on the library version, there can be multiple interfaces for key
 * agreement. This test function performs the ones that are available amongst:
 * - psa_raw_key_agreement()
 * - psa_key_agreement()
 * - psa_key_agreement_iop_setup() and psa_key_agreement_iop_complete()
 *
 * Mark the current test case as failed in the following cases:
 * - Operational errors such as failure to allocate memory for an intermediate
 *   buffer.
 * - Results are not consistent between the methods that are performed:
 *   different statuses, or inconsistent metadata, or different shared secret.
 *
 * \param alg               A key agreement algorithm compatible with \p key.
 * \param key               A key that allows key agreement with \p alg.
 * \param key_destroyable   If set to 1, a failure due to the key not existing
 *                          or the key being destroyed mid-operation will only
 *                          be reported if the error code is unexpected.
 *
 * \return                  The status from psa_raw_key_agreement().
 */
psa_status_t mbedtls_test_psa_raw_key_agreement_with_self(
    psa_algorithm_t alg,
    mbedtls_svc_key_id_t key, int key_destroyable);

/** Perform a key agreement using the given key pair against its public key
 * using psa_key_derivation_raw_key().
 *
 * The result is discarded. The purpose of this function is to smoke-test a key.
 *
 * In case of failure, mark the current test case as failed.
 *
 * \param operation         An operation that has been set up for a key
 *                          agreement algorithm that is compatible with
 *                          \p key.
 * \param key               A key pair object that is suitable for a key
 *                          agreement with \p operation.
 * \param key_destroyable   If set to 1, a failure due to the key not existing
 *                          or the key being destroyed mid-operation will only
 *                          be reported if the error code is unexpected.
 *
 * \return                  \c 1 on success, \c 0 on failure.
 */
psa_status_t mbedtls_test_psa_key_agreement_with_self(
    psa_key_derivation_operation_t *operation,
    mbedtls_svc_key_id_t key, int key_destroyable);

/** Perform sanity checks on the given key representation.
 *
 * If any of the checks fail, mark the current test case as failed.
 *
 * The checks depend on the key type.
 * - All types: check the export size against maximum-size macros.
 * - DES: parity bits.
 * - RSA: check the ASN.1 structure and the size and parity of the integers.
 * - ECC private or public key: exact representation length.
 * - Montgomery public key: first byte.
 *
 * \param type              The key type.
 * \param bits              The key size in bits.
 * \param exported          A buffer containing the key representation.
 * \param exported_length   The length of \p exported in bytes.
 *
 * \return                  \c 1 if all checks passed, \c 0 on failure.
 */
int mbedtls_test_psa_exported_key_sanity_check(
    psa_key_type_t type, size_t bits,
    const uint8_t *exported, size_t exported_length);

/** Do smoke tests on a key.
 *
 * Perform one of each operation indicated by \p alg (decrypt/encrypt,
 * sign/verify, or derivation) that is permitted according to \p usage.
 * \p usage and \p alg should correspond to the expected policy on the
 * key.
 *
 * Export the key if permitted by \p usage, and check that the output
 * looks sensible. If \p usage forbids export, check that
 * \p psa_export_key correctly rejects the attempt. If the key is
 * asymmetric, also check \p psa_export_public_key.
 *
 * If the key fails the tests, this function calls the test framework's
 * `mbedtls_test_fail` function and returns false. Otherwise this function
 * returns true. Therefore it should be used as follows:
 * ```
 * if( ! exercise_key( ... ) ) goto exit;
 * ```
 * To use this function for multi-threaded tests where the key
 * may be destroyed at any point: call this function with key_destroyable set
 * to 1, while another thread calls psa_destroy_key on the same key;
 * this will test whether destroying the key in use leads to any corruption.
 *
 * There cannot be a set of concurrent calls:
 * `mbedtls_test_psa_exercise_key(ki,...)` such that each ki is a unique
 * persistent key not loaded into any key slot, and i is greater than the
 * number of free key slots.
 * This is because such scenarios can lead to unsupported
 * `PSA_ERROR_INSUFFICIENT_MEMORY` return codes.
 *
 *
 * \param key               The key to exercise. It should be capable of performing
 *                          \p alg.
 * \param usage             The usage flags to assume.
 * \param alg               The algorithm to exercise.
 * \param key_destroyable   If set to 1, a failure due to the key not existing
 *                          or the key being destroyed mid-operation will only
 *                          be reported if the error code is unexpected.
 *
 * \retval 0 The key failed the smoke tests.
 * \retval 1 The key passed the smoke tests.
 */
int mbedtls_test_psa_exercise_key(mbedtls_svc_key_id_t key,
                                  psa_key_usage_t usage,
                                  psa_algorithm_t alg,
                                  int key_destroyable);

psa_key_usage_t mbedtls_test_psa_usage_to_exercise(psa_key_type_t type,
                                                   psa_algorithm_t alg);

/** Whether the specified algorithm can be exercised.
 *
 * \note This function is solely based on the algorithm and does not
 *       consider potential issues with the compatibility of a key.
 *       The idea is that you already have a key, so you know that the
 *       key type is supported, and you want to exercise the key but
 *       only if the algorithm given in its policy is enabled in the
 *       compile-time configuration.
 *
 * \note This function currently only supports signature algorithms
 *       (including wildcards).
 *       TODO: a more general mechanism, which should be automatically
 *       generated and possibly available as a library function?
 */
int mbedtls_test_can_exercise_psa_algorithm(psa_algorithm_t alg);

#if defined(MBEDTLS_PK_C)
/** PK-PSA key consistency test.
 *
 * This function tests that the pk context and the PSA key are
 * consistent. At a minimum:
 *
 * - The two objects must contain keys of the same type,
 *   or a key pair and a public key of the matching type.
 * - The two objects must have the same public key.
 *
 * \retval 0 The key failed the consistency tests.
 * \retval 1 The key passed the consistency tests.
 */
int mbedtls_test_key_consistency_psa_pk(mbedtls_svc_key_id_t psa_key,
                                        const mbedtls_pk_context *pk);
#endif /* MBEDTLS_PK_C */

#endif /* PSA_EXERCISE_KEY_H */
