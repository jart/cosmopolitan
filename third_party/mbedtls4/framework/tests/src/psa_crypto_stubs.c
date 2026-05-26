/** \file psa_crypto_stubs.c
 *
 * \brief Stub functions when MBEDTLS_PSA_CRYPTO_CLIENT is enabled but
 *        MBEDTLS_PSA_CRYPTO_C is disabled.
 */

/*
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "test_common.h"
#include <psa/crypto.h>

#if defined(MBEDTLS_PSA_CRYPTO_CLIENT) && !defined(MBEDTLS_PSA_CRYPTO_C)

psa_status_t psa_generate_random(uint8_t *output,
                                 size_t output_size)
{
    (void) output;
    (void) output_size;

    return PSA_ERROR_COMMUNICATION_FAILURE;
}

psa_status_t psa_export_key(mbedtls_svc_key_id_t key,
                            uint8_t *data,
                            size_t data_size,
                            size_t *data_length)
{
    (void) key;
    (void) data;
    (void) data_size;
    (void) data_length;
    return PSA_ERROR_COMMUNICATION_FAILURE;
}

psa_status_t psa_export_public_key(mbedtls_svc_key_id_t key,
                                   uint8_t *data,
                                   size_t data_size,
                                   size_t *data_length)
{
    (void) key;
    (void) data;
    (void) data_size;
    (void) data_length;
    return PSA_ERROR_COMMUNICATION_FAILURE;
}

psa_status_t psa_get_key_attributes(mbedtls_svc_key_id_t key,
                                    psa_key_attributes_t *attributes)
{
    (void) key;
    (void) attributes;
    return PSA_ERROR_COMMUNICATION_FAILURE;
}

psa_status_t psa_hash_abort(psa_hash_operation_t *operation)
{
    (void) operation;
    return PSA_ERROR_COMMUNICATION_FAILURE;
}

psa_status_t psa_import_key(const psa_key_attributes_t *attributes,
                            const uint8_t *data,
                            size_t data_length,
                            mbedtls_svc_key_id_t *key)
{
    (void) attributes;
    (void) data;
    (void) data_length;
    (void) key;
    return PSA_ERROR_COMMUNICATION_FAILURE;
}

int psa_can_do_hash(psa_algorithm_t hash_alg)
{
    (void) hash_alg;
    return 0;
}

psa_status_t psa_hash_clone(const psa_hash_operation_t *source_operation,
                            psa_hash_operation_t *target_operation)
{
    (void) source_operation;
    (void) target_operation;
    return PSA_ERROR_COMMUNICATION_FAILURE;
}

psa_status_t psa_hash_setup(psa_hash_operation_t *operation,
                            psa_algorithm_t alg)
{
    (void) operation;
    (void) alg;
    return PSA_ERROR_COMMUNICATION_FAILURE;
}

psa_status_t psa_hash_update(psa_hash_operation_t *operation,
                             const uint8_t *input_external,
                             size_t input_length)
{
    (void) operation;
    (void) input_external;
    (void) input_length;
    return PSA_ERROR_COMMUNICATION_FAILURE;
}

psa_status_t psa_hash_finish(psa_hash_operation_t *operation,
                             uint8_t *hash_external,
                             size_t hash_size,
                             size_t *hash_length)
{
    (void) operation;
    (void) hash_external;
    (void) hash_size;
    (void) hash_length;
    return PSA_ERROR_COMMUNICATION_FAILURE;
}

psa_status_t psa_hash_compute(psa_algorithm_t alg,
                              const uint8_t *input_external, size_t input_length,
                              uint8_t *hash_external, size_t hash_size,
                              size_t *hash_length)
{
    (void) alg;
    (void) input_external;
    (void) input_length;
    (void) hash_external;
    (void) hash_size;
    (void) hash_length;
    return PSA_ERROR_COMMUNICATION_FAILURE;
}

#endif /* MBEDTLS_PSA_CRYPTO_CLIENT && !MBEDTLS_PSA_CRYPTO_C */
