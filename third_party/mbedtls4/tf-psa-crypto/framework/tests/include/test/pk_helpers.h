/*
 * Helper functions for PK.
 * This is only for TF-PSA-Crypto 1.0 and above.
 */
/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#ifndef PK_HELPERS_H
#define PK_HELPERS_H

#include "build_info.h"

#if defined(MBEDTLS_PK_C)

#include <psa/crypto.h>
#include <mbedtls/pk.h>

/* 'pk_context_populate_method_t' is only used in 'mbedtls_pk_helpers_populate_context'
 * which takes a PSA key ID to populate the PK context. The idea is to use that
 * function after calling 'mbedtls_pk_helpers_make_psa_key_from_predefined' to
 * retrieve a PSA key ID. Adding support for parsing doesn't fit well with the
 * current prototype of 'mbedtls_pk_helpers_populate_context'.
 * What is needed to add parsing in the list below is a new function which acts
 * as a combination between 'mbedtls_pk_helpers_make_psa_key_from_predefined' and
 * 'mbedtls_pk_helpers_populate_context', i.e. it takes a key type, key bits and
 * population method as input and it returns a PK context.
 */
typedef enum {
    TEST_PK_WRAP_PSA,
    TEST_PK_COPY_FROM_PSA,
    TEST_PK_COPY_PUBLIC_FROM_PSA,
} pk_context_populate_method_t;

/**
 * Get predefined key pair/public key data for the requested key.
 *
 * If the specified key type or bit length does not exist in the list of known
 * predefined keys, an assertion failure will be generated.
 *
 * The output format is compatible with PSA API, so they key can be imported
 * with psa_import_key().
 *
 * \param key_type   PSA key type for the key being requested.
 * \param key_bits   Bit length for the PSA key being requested.
 * \param output     Pointer which on exit will point to the key material that has
 *                   been requested by "key_type" and "key_bits".
 * \param output_len Length of the key material being pointed to from "output".
 *
 * \return           0 on success; MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE if the required
 *                   key is not the in list of known ones.
 */
int mbedtls_pk_helpers_get_predefined_key_data(psa_key_type_t key_type, psa_key_bits_t key_bits,
                                               const uint8_t **output, size_t *output_len);

/**
 * Create a PSA key using prefined key data.
 *
 * Predefined key data is used to create the key and the choice of predefined
 * key material is based on the combination of "key_type" and "key_bits".
 *
 * \param key_type    Type of key to be created. For the time being only RSA and
 *                    EC key types are supported.
 * \param key_bits    Length of the key (in bits).
 * \param alg         Algorithm to be associated with the key.
 * \param alg2        Enrollment alogrithm to be associated with the key.
 * \param usage_flags Usage flags to be associated with the key.
 *
 * \return            On success the key ID of the created PSA key.
 *                    On failure 0 is returned and the test is marked as failed.
 */
mbedtls_svc_key_id_t mbedtls_pk_helpers_make_psa_key_from_predefined(psa_key_type_t key_type,
                                                                     psa_key_bits_t key_bits,
                                                                     psa_algorithm_t alg,
                                                                     psa_algorithm_t alg2,
                                                                     psa_key_usage_t usage_flags);

/**
 * Populate the given PK context using "key_id" and the desired "method".
 *
 * \param pk     The PK context to be populated; it must have been initialized.
 * \param key_id The PSA key ID to be used to populate the PK context.
 * \param method The desired method for populating the PK context. See
 *               "pk_context_populate_method_t" for available options.
 *
 * \return       0 on success.
 *               In case of failure -1 is returned and the test case is marked
 *               as failed.
 */
int mbedtls_pk_helpers_populate_context(mbedtls_pk_context *pk, mbedtls_svc_key_id_t key_id,
                                        pk_context_populate_method_t method);

#endif /* MBEDTLS_PK_C */

#endif /* PK_HELPERS_H */
