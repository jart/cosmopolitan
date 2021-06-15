/*
 *  Function signatures for functionality that can be provided by
 *  cryptographic accelerators.
 *  Warning: This file will be auto-generated in the future.
 */
/*  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef PSA_CRYPTO_DRIVER_WRAPPERS_H
#define PSA_CRYPTO_DRIVER_WRAPPERS_H

#include "psa/crypto.h"
#include "psa/crypto_driver_common.h"

/*
 * Signature functions
 */
psa_status_t psa_driver_wrapper_sign_hash(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg, const uint8_t *hash, size_t hash_length,
    uint8_t *signature, size_t signature_size, size_t *signature_length );

psa_status_t psa_driver_wrapper_verify_hash(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg, const uint8_t *hash, size_t hash_length,
    const uint8_t *signature, size_t signature_length );

/*
 * Key handling functions
 */

psa_status_t psa_driver_wrapper_import_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *data, size_t data_length,
    uint8_t *key_buffer, size_t key_buffer_size,
    size_t *key_buffer_length, size_t *bits );

psa_status_t psa_driver_wrapper_export_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    uint8_t *data, size_t data_size, size_t *data_length );

psa_status_t psa_driver_wrapper_export_public_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    uint8_t *data, size_t data_size, size_t *data_length );

psa_status_t psa_driver_wrapper_get_key_buffer_size(
    const psa_key_attributes_t *attributes,
    size_t *key_buffer_size );

psa_status_t psa_driver_wrapper_generate_key(
    const psa_key_attributes_t *attributes,
    uint8_t *key_buffer, size_t key_buffer_size, size_t *key_buffer_length );

/*
 * Cipher functions
 */
psa_status_t psa_driver_wrapper_cipher_encrypt(
    psa_key_slot_t *slot,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length );

psa_status_t psa_driver_wrapper_cipher_decrypt(
    psa_key_slot_t *slot,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length );

psa_status_t psa_driver_wrapper_cipher_encrypt_setup(
    psa_operation_driver_context_t *operation,
    psa_key_slot_t *slot,
    psa_algorithm_t alg );

psa_status_t psa_driver_wrapper_cipher_decrypt_setup(
    psa_operation_driver_context_t *operation,
    psa_key_slot_t *slot,
    psa_algorithm_t alg );

psa_status_t psa_driver_wrapper_cipher_generate_iv(
    psa_operation_driver_context_t *operation,
    uint8_t *iv,
    size_t iv_size,
    size_t *iv_length );

psa_status_t psa_driver_wrapper_cipher_set_iv(
    psa_operation_driver_context_t *operation,
    const uint8_t *iv,
    size_t iv_length );

psa_status_t psa_driver_wrapper_cipher_update(
    psa_operation_driver_context_t *operation,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length );

psa_status_t psa_driver_wrapper_cipher_finish(
    psa_operation_driver_context_t *operation,
    uint8_t *output,
    size_t output_size,
    size_t *output_length );

psa_status_t psa_driver_wrapper_cipher_abort(
    psa_operation_driver_context_t *operation );

#endif /* PSA_CRYPTO_DRIVER_WRAPPERS_H */

/* End of automatically generated file. */
