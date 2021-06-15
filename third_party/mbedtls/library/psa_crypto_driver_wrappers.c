/*
 *  Functions to delegate cryptographic operations to an available
 *  and appropriate accelerator.
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

#include "psa_crypto_core.h"
#include "psa_crypto_driver_wrappers.h"
#include "mbedtls/platform.h"

#if defined(MBEDTLS_PSA_CRYPTO_DRIVERS)

/* Include test driver definition when running tests */
#if defined(PSA_CRYPTO_DRIVER_TEST)
#ifndef PSA_CRYPTO_DRIVER_PRESENT
#define PSA_CRYPTO_DRIVER_PRESENT
#endif
#ifndef PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#define PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT
#endif
#include "test/drivers/test_driver.h"
#endif /* PSA_CRYPTO_DRIVER_TEST */

/* Repeat above block for each JSON-declared driver during autogeneration */

/* Auto-generated values depending on which drivers are registered. ID 0 is
 * reserved for unallocated operations. */
#if defined(PSA_CRYPTO_DRIVER_TEST)
#define PSA_CRYPTO_TRANSPARENT_TEST_DRIVER_ID (1)
#define PSA_CRYPTO_OPAQUE_TEST_DRIVER_ID (2)
#endif /* PSA_CRYPTO_DRIVER_TEST */
#endif /* MBEDTLS_PSA_CRYPTO_DRIVERS */

/* Support the 'old' SE interface when asked to */
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
/* PSA_CRYPTO_DRIVER_PRESENT is defined when either a new-style or old-style
 * SE driver is present, to avoid unused argument errors at compile time. */
#ifndef PSA_CRYPTO_DRIVER_PRESENT
#define PSA_CRYPTO_DRIVER_PRESENT
#endif
#include "psa_crypto_se.h"
#endif

/* Start delegation functions */
psa_status_t psa_driver_wrapper_sign_hash(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg, const uint8_t *hash, size_t hash_length,
    uint8_t *signature, size_t signature_size, size_t *signature_length )
{
    /* Try dynamically-registered SE interface first */
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    const psa_drv_se_t *drv;
    psa_drv_se_context_t *drv_context;

    if( psa_get_se_driver( attributes->core.lifetime, &drv, &drv_context ) )
    {
        if( drv->asymmetric == NULL ||
            drv->asymmetric->p_sign == NULL )
        {
            /* Key is defined in SE, but we have no way to exercise it */
            return( PSA_ERROR_NOT_SUPPORTED );
        }
        return( drv->asymmetric->p_sign(
                    drv_context, *( (psa_key_slot_number_t *)key_buffer ),
                    alg, hash, hash_length,
                    signature, signature_size, signature_length ) );
    }
#endif /* PSA_CRYPTO_SE_C */

    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_location_t location =
        PSA_KEY_LIFETIME_GET_LOCATION( attributes->core.lifetime );

    switch( location )
    {
        case PSA_KEY_LOCATION_LOCAL_STORAGE:
            /* Key is stored in the slot in export representation, so
             * cycle through all known transparent accelerators */
#if defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
#if defined(PSA_CRYPTO_DRIVER_TEST)
            status = test_transparent_signature_sign_hash( attributes,
                                                           key_buffer,
                                                           key_buffer_size,
                                                           alg,
                                                           hash,
                                                           hash_length,
                                                           signature,
                                                           signature_size,
                                                           signature_length );
            /* Declared with fallback == true */
            if( status != PSA_ERROR_NOT_SUPPORTED )
                return( status );
#endif /* PSA_CRYPTO_DRIVER_TEST */
#endif /* PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT */
            /* Fell through, meaning no accelerator supports this operation */
            return( psa_sign_hash_internal( attributes,
                                            key_buffer,
                                            key_buffer_size,
                                            alg,
                                            hash,
                                            hash_length,
                                            signature,
                                            signature_size,
                                            signature_length ) );

        /* Add cases for opaque driver here */
#if defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_TEST_DRIVER_LIFETIME:
            return( test_opaque_signature_sign_hash( attributes,
                                                     key_buffer,
                                                     key_buffer_size,
                                                     alg,
                                                     hash,
                                                     hash_length,
                                                     signature,
                                                     signature_size,
                                                     signature_length ) );
#endif /* PSA_CRYPTO_DRIVER_TEST */
#endif /* PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT */
        default:
            /* Key is declared with a lifetime not known to us */
            (void)status;
            return( PSA_ERROR_INVALID_ARGUMENT );
    }
}

psa_status_t psa_driver_wrapper_verify_hash(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg, const uint8_t *hash, size_t hash_length,
    const uint8_t *signature, size_t signature_length )
{
    /* Try dynamically-registered SE interface first */
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    const psa_drv_se_t *drv;
    psa_drv_se_context_t *drv_context;

    if( psa_get_se_driver( attributes->core.lifetime, &drv, &drv_context ) )
    {
        if( drv->asymmetric == NULL ||
            drv->asymmetric->p_verify == NULL )
        {
            /* Key is defined in SE, but we have no way to exercise it */
            return( PSA_ERROR_NOT_SUPPORTED );
        }
        return( drv->asymmetric->p_verify(
                    drv_context, *( (psa_key_slot_number_t *)key_buffer ),
                    alg, hash, hash_length,
                    signature, signature_length ) );
    }
#endif /* PSA_CRYPTO_SE_C */

    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_location_t location =
        PSA_KEY_LIFETIME_GET_LOCATION( attributes->core.lifetime );

    switch( location )
    {
        case PSA_KEY_LOCATION_LOCAL_STORAGE:
            /* Key is stored in the slot in export representation, so
             * cycle through all known transparent accelerators */
#if defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
#if defined(PSA_CRYPTO_DRIVER_TEST)
            status = test_transparent_signature_verify_hash( attributes,
                                                             key_buffer,
                                                             key_buffer_size,
                                                             alg,
                                                             hash,
                                                             hash_length,
                                                             signature,
                                                             signature_length );
            /* Declared with fallback == true */
            if( status != PSA_ERROR_NOT_SUPPORTED )
                return( status );
#endif /* PSA_CRYPTO_DRIVER_TEST */
#endif /* PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT */

            return( psa_verify_hash_internal( attributes,
                                              key_buffer,
                                              key_buffer_size,
                                              alg,
                                              hash,
                                              hash_length,
                                              signature,
                                              signature_length ) );

        /* Add cases for opaque driver here */
#if defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_TEST_DRIVER_LIFETIME:
            return( test_opaque_signature_verify_hash( attributes,
                                                       key_buffer,
                                                       key_buffer_size,
                                                       alg,
                                                       hash,
                                                       hash_length,
                                                       signature,
                                                       signature_length ) );
#endif /* PSA_CRYPTO_DRIVER_TEST */
#endif /* PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT */
        default:
            /* Key is declared with a lifetime not known to us */
            (void)status;
            return( PSA_ERROR_INVALID_ARGUMENT );
    }
}

/** Get the key buffer size for the key material of a generated key in the
 *  case of an opaque driver without storage.
 *
 * \param[in] attributes  The key attributes.
 * \param[out] key_buffer_size  Minimum buffer size to contain the key material
 *
 * \retval #PSA_SUCCESS
 *         The minimum size for a buffer to contain the key material has been
 *         returned successfully.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         The size in bits of the key is not valid.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         The type and/or the size in bits of the key or the combination of
 *         the two is not supported.
 */
psa_status_t psa_driver_wrapper_get_key_buffer_size(
    const psa_key_attributes_t *attributes,
    size_t *key_buffer_size )
{
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION( attributes->core.lifetime );
    psa_key_type_t key_type = attributes->core.type;
    size_t key_bits = attributes->core.bits;

    *key_buffer_size = 0;
    switch( location )
    {
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_TEST_DRIVER_LIFETIME:
#ifdef TEST_DRIVER_KEY_CONTEXT_SIZE_FUNCTION
            *key_buffer_size = test_size_function( key_type, key_bits );
            return( PSA_SUCCESS );
#else /* TEST_DRIVER_KEY_CONTEXT_SIZE_FUNCTION */
            if( PSA_KEY_TYPE_IS_KEY_PAIR( key_type ) )
            {
                int public_key_overhead =
                    ( ( TEST_DRIVER_KEY_CONTEXT_STORE_PUBLIC_KEY == 1 ) ?
                      PSA_EXPORT_KEY_OUTPUT_SIZE( key_type, key_bits ) : 0 );
                *key_buffer_size = TEST_DRIVER_KEY_CONTEXT_BASE_SIZE
                                 + TEST_DRIVER_KEY_CONTEXT_PUBLIC_KEY_SIZE
                                 + public_key_overhead;
            }
            else if( PSA_KEY_TYPE_IS_PUBLIC_KEY( key_type ) )
            {
                *key_buffer_size = TEST_DRIVER_KEY_CONTEXT_BASE_SIZE
                                 + TEST_DRIVER_KEY_CONTEXT_PUBLIC_KEY_SIZE;
            }
            else if ( !PSA_KEY_TYPE_IS_KEY_PAIR( key_type ) &&
                      !PSA_KEY_TYPE_IS_PUBLIC_KEY ( key_type ) )
            {
                *key_buffer_size = TEST_DRIVER_KEY_CONTEXT_BASE_SIZE
                                 + TEST_DRIVER_KEY_CONTEXT_SYMMETRIC_FACTOR
                                 * ( ( key_bits + 7 ) / 8 );
            }
            else
            {
                return( PSA_ERROR_NOT_SUPPORTED );
            }
            return( PSA_SUCCESS );
#endif /* TEST_DRIVER_KEY_CONTEXT_SIZE_FUNCTION */
#endif /* PSA_CRYPTO_DRIVER_TEST */

        default:
            (void)key_type;
            (void)key_bits;
            return( PSA_ERROR_NOT_SUPPORTED );
    }
}

psa_status_t psa_driver_wrapper_generate_key(
    const psa_key_attributes_t *attributes,
    uint8_t *key_buffer, size_t key_buffer_size, size_t *key_buffer_length )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_location_t location =
        PSA_KEY_LIFETIME_GET_LOCATION(attributes->core.lifetime);

    /* Try dynamically-registered SE interface first */
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    const psa_drv_se_t *drv;
    psa_drv_se_context_t *drv_context;

    if( psa_get_se_driver( attributes->core.lifetime, &drv, &drv_context ) )
    {
        size_t pubkey_length = 0; /* We don't support this feature yet */
        if( drv->key_management == NULL ||
            drv->key_management->p_generate == NULL )
        {
            /* Key is defined as being in SE, but we have no way to generate it */
            return( PSA_ERROR_NOT_SUPPORTED );
        }
        return( drv->key_management->p_generate(
            drv_context,
            *( (psa_key_slot_number_t *)key_buffer ),
            attributes, NULL, 0, &pubkey_length ) );
    }
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */

    switch( location )
    {
        case PSA_KEY_LOCATION_LOCAL_STORAGE:
#if defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
            /* Transparent drivers are limited to generating asymmetric keys */
            if( PSA_KEY_TYPE_IS_ASYMMETRIC( attributes->core.type ) )
            {
            /* Cycle through all known transparent accelerators */
#if defined(PSA_CRYPTO_DRIVER_TEST)
                status = test_transparent_generate_key(
                    attributes, key_buffer, key_buffer_size,
                    key_buffer_length );
                /* Declared with fallback == true */
                if( status != PSA_ERROR_NOT_SUPPORTED )
                    break;
#endif /* PSA_CRYPTO_DRIVER_TEST */
            }
#endif /* PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT */

            /* Software fallback */
            status = psa_generate_key_internal(
                attributes, key_buffer, key_buffer_size, key_buffer_length );
            break;

        /* Add cases for opaque driver here */
#if defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_TEST_DRIVER_LIFETIME:
            status = test_opaque_generate_key(
                attributes, key_buffer, key_buffer_size, key_buffer_length );
            break;
#endif /* PSA_CRYPTO_DRIVER_TEST */
#endif /* PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT */

        default:
            /* Key is declared with a lifetime not known to us */
            status = PSA_ERROR_INVALID_ARGUMENT;
            break;
    }

    return( status );
}

psa_status_t psa_driver_wrapper_import_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *data,
    size_t data_length,
    uint8_t *key_buffer,
    size_t key_buffer_size,
    size_t *key_buffer_length,
    size_t *bits )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(
                                      psa_get_key_lifetime( attributes ) );

    /* Try dynamically-registered SE interface first */
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    const psa_drv_se_t *drv;
    psa_drv_se_context_t *drv_context;

    if( psa_get_se_driver( attributes->core.lifetime, &drv, &drv_context ) )
    {
        if( drv->key_management == NULL ||
            drv->key_management->p_import == NULL )
            return( PSA_ERROR_NOT_SUPPORTED );

        /* The driver should set the number of key bits, however in
         * case it doesn't, we initialize bits to an invalid value. */
        *bits = PSA_MAX_KEY_BITS + 1;
        status = drv->key_management->p_import(
            drv_context,
            *( (psa_key_slot_number_t *)key_buffer ),
            attributes, data, data_length, bits );

        if( status != PSA_SUCCESS )
            return( status );

        if( (*bits) > PSA_MAX_KEY_BITS )
            return( PSA_ERROR_NOT_SUPPORTED );

        return( PSA_SUCCESS );
    }
#endif /* PSA_CRYPTO_SE_C */

    switch( location )
    {
        case PSA_KEY_LOCATION_LOCAL_STORAGE:
            /* Key is stored in the slot in export representation, so
             * cycle through all known transparent accelerators */
#if defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
#if defined(PSA_CRYPTO_DRIVER_TEST)
            status = test_transparent_import_key( attributes,
                                                  data, data_length,
                                                  key_buffer, key_buffer_size,
                                                  key_buffer_length, bits );
            /* Declared with fallback == true */
            if( status != PSA_ERROR_NOT_SUPPORTED )
                return( status );
#endif /* PSA_CRYPTO_DRIVER_TEST */
#endif /* PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT */
            /* Fell through, meaning no accelerator supports this operation */
            return( psa_import_key_into_slot( attributes,
                                              data, data_length,
                                              key_buffer, key_buffer_size,
                                              key_buffer_length, bits ) );

        default:
            /* Importing a key with external storage in not yet supported.
             * Return in error indicating that the lifetime is not valid. */
            (void)status;
            return( PSA_ERROR_INVALID_ARGUMENT );
    }

}

psa_status_t psa_driver_wrapper_export_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    uint8_t *data, size_t data_size, size_t *data_length )

{
    psa_status_t status = PSA_ERROR_INVALID_ARGUMENT;
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(
                                      psa_get_key_lifetime( attributes ) );

    /* Try dynamically-registered SE interface first */
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    const psa_drv_se_t *drv;
    psa_drv_se_context_t *drv_context;

    if( psa_get_se_driver( attributes->core.lifetime, &drv, &drv_context ) )
    {
        if( ( drv->key_management == NULL   ) ||
            ( drv->key_management->p_export == NULL ) )
        {
            return( PSA_ERROR_NOT_SUPPORTED );
        }

        return( drv->key_management->p_export(
                     drv_context,
                     *( (psa_key_slot_number_t *)key_buffer ),
                     data, data_size, data_length ) );
    }
#endif /* PSA_CRYPTO_SE_C */

    switch( location )
    {
        case PSA_KEY_LOCATION_LOCAL_STORAGE:
            return( psa_export_key_internal( attributes,
                                             key_buffer,
                                             key_buffer_size,
                                             data,
                                             data_size,
                                             data_length ) );

        /* Add cases for opaque driver here */
#if defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_TEST_DRIVER_LIFETIME:
            return( test_opaque_export_key( attributes,
                                            key_buffer,
                                            key_buffer_size,
                                            data,
                                            data_size,
                                            data_length ) );
#endif /* PSA_CRYPTO_DRIVER_TEST */
#endif /* PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT */
        default:
            /* Key is declared with a lifetime not known to us */
            return( status );
    }
}

psa_status_t psa_driver_wrapper_export_public_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    uint8_t *data, size_t data_size, size_t *data_length )

{
    psa_status_t status = PSA_ERROR_INVALID_ARGUMENT;
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(
                                      psa_get_key_lifetime( attributes ) );

    /* Try dynamically-registered SE interface first */
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    const psa_drv_se_t *drv;
    psa_drv_se_context_t *drv_context;

    if( psa_get_se_driver( attributes->core.lifetime, &drv, &drv_context ) )
    {
        if( ( drv->key_management == NULL ) ||
            ( drv->key_management->p_export_public == NULL ) )
        {
            return( PSA_ERROR_NOT_SUPPORTED );
        }

        return( drv->key_management->p_export_public(
                    drv_context,
                    *( (psa_key_slot_number_t *)key_buffer ),
                    data, data_size, data_length ) );
    }
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */

    switch( location )
    {
        case PSA_KEY_LOCATION_LOCAL_STORAGE:
            /* Key is stored in the slot in export representation, so
             * cycle through all known transparent accelerators */
#if defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
#if defined(PSA_CRYPTO_DRIVER_TEST)
            status = test_transparent_export_public_key( attributes,
                                                         key_buffer,
                                                         key_buffer_size,
                                                         data,
                                                         data_size,
                                                         data_length );
            /* Declared with fallback == true */
            if( status != PSA_ERROR_NOT_SUPPORTED )
                return( status );
#endif /* PSA_CRYPTO_DRIVER_TEST */
#endif /* PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT */
            /* Fell through, meaning no accelerator supports this operation */
            return( psa_export_public_key_internal( attributes,
                                                    key_buffer,
                                                    key_buffer_size,
                                                    data,
                                                    data_size,
                                                    data_length ) );

        /* Add cases for opaque driver here */
#if defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_TEST_DRIVER_LIFETIME:
            return( test_opaque_export_public_key( attributes,
                                                   key_buffer,
                                                   key_buffer_size,
                                                   data,
                                                   data_size,
                                                   data_length ) );
#endif /* PSA_CRYPTO_DRIVER_TEST */
#endif /* PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT */
        default:
            /* Key is declared with a lifetime not known to us */
            return( status );
    }
}

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
    size_t *output_length )
{
#if defined(PSA_CRYPTO_DRIVER_PRESENT) && defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
    psa_status_t status = PSA_ERROR_INVALID_ARGUMENT;
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(slot->attr.lifetime);
    psa_key_attributes_t attributes = {
      .core = slot->attr
    };

    switch( location )
    {
        case PSA_KEY_LOCATION_LOCAL_STORAGE:
            /* Key is stored in the slot in export representation, so
             * cycle through all known transparent accelerators */
#if defined(PSA_CRYPTO_DRIVER_TEST)
            status = test_transparent_cipher_encrypt( &attributes,
                                                      slot->key.data,
                                                      slot->key.bytes,
                                                      alg,
                                                      input,
                                                      input_length,
                                                      output,
                                                      output_size,
                                                      output_length );
            /* Declared with fallback == true */
            if( status != PSA_ERROR_NOT_SUPPORTED )
                return( status );
#endif /* PSA_CRYPTO_DRIVER_TEST */
            /* Fell through, meaning no accelerator supports this operation */
            return( PSA_ERROR_NOT_SUPPORTED );
        /* Add cases for opaque driver here */
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_TEST_DRIVER_LIFETIME:
            return( test_opaque_cipher_encrypt( &attributes,
                                                slot->key.data,
                                                slot->key.bytes,
                                                alg,
                                                input,
                                                input_length,
                                                output,
                                                output_size,
                                                output_length ) );
#endif /* PSA_CRYPTO_DRIVER_TEST */
        default:
            /* Key is declared with a lifetime not known to us */
            return( status );
    }
#else /* PSA_CRYPTO_DRIVER_PRESENT */
    (void) slot;
    (void) alg;
    (void) input;
    (void) input_length;
    (void) output;
    (void) output_size;
    (void) output_length;

    return( PSA_ERROR_NOT_SUPPORTED );
#endif /* PSA_CRYPTO_DRIVER_PRESENT */
}

psa_status_t psa_driver_wrapper_cipher_decrypt(
    psa_key_slot_t *slot,
    psa_algorithm_t alg,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length )
{
#if defined(PSA_CRYPTO_DRIVER_PRESENT) && defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
    psa_status_t status = PSA_ERROR_INVALID_ARGUMENT;
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(slot->attr.lifetime);
    psa_key_attributes_t attributes = {
      .core = slot->attr
    };

    switch( location )
    {
        case PSA_KEY_LOCATION_LOCAL_STORAGE:
            /* Key is stored in the slot in export representation, so
             * cycle through all known transparent accelerators */
#if defined(PSA_CRYPTO_DRIVER_TEST)
            status = test_transparent_cipher_decrypt( &attributes,
                                                      slot->key.data,
                                                      slot->key.bytes,
                                                      alg,
                                                      input,
                                                      input_length,
                                                      output,
                                                      output_size,
                                                      output_length );
            /* Declared with fallback == true */
            if( status != PSA_ERROR_NOT_SUPPORTED )
                return( status );
#endif /* PSA_CRYPTO_DRIVER_TEST */
            /* Fell through, meaning no accelerator supports this operation */
            return( PSA_ERROR_NOT_SUPPORTED );
        /* Add cases for opaque driver here */
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_TEST_DRIVER_LIFETIME:
            return( test_opaque_cipher_decrypt( &attributes,
                                                slot->key.data,
                                                slot->key.bytes,
                                                alg,
                                                input,
                                                input_length,
                                                output,
                                                output_size,
                                                output_length ) );
#endif /* PSA_CRYPTO_DRIVER_TEST */
        default:
            /* Key is declared with a lifetime not known to us */
            return( status );
    }
#else /* PSA_CRYPTO_DRIVER_PRESENT */
    (void) slot;
    (void) alg;
    (void) input;
    (void) input_length;
    (void) output;
    (void) output_size;
    (void) output_length;

    return( PSA_ERROR_NOT_SUPPORTED );
#endif /* PSA_CRYPTO_DRIVER_PRESENT */
}

psa_status_t psa_driver_wrapper_cipher_encrypt_setup(
    psa_operation_driver_context_t *operation,
    psa_key_slot_t *slot,
    psa_algorithm_t alg )
{
#if defined(PSA_CRYPTO_DRIVER_PRESENT) && defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
    psa_status_t status = PSA_ERROR_INVALID_ARGUMENT;
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(slot->attr.lifetime);
    psa_key_attributes_t attributes = {
      .core = slot->attr
    };

    switch( location )
    {
        case PSA_KEY_LOCATION_LOCAL_STORAGE:
            /* Key is stored in the slot in export representation, so
             * cycle through all known transparent accelerators */
#if defined(PSA_CRYPTO_DRIVER_TEST)
            operation->ctx = mbedtls_calloc( 1, sizeof(test_transparent_cipher_operation_t) );
            if( operation->ctx == NULL )
                return PSA_ERROR_INSUFFICIENT_MEMORY;

            status = test_transparent_cipher_encrypt_setup( operation->ctx,
                                                            &attributes,
                                                            slot->key.data,
                                                            slot->key.bytes,
                                                            alg );
            /* Declared with fallback == true */
            if( status == PSA_SUCCESS )
                operation->id = PSA_CRYPTO_TRANSPARENT_TEST_DRIVER_ID;
            else
            {
                mbedtls_platform_zeroize(
                    operation->ctx,
                    sizeof( test_transparent_cipher_operation_t ) );
                mbedtls_free( operation->ctx );
                operation->ctx = NULL;
            }

            return( status );
#endif /* PSA_CRYPTO_DRIVER_TEST */
            /* Fell through, meaning no accelerator supports this operation */
            return( PSA_ERROR_NOT_SUPPORTED );
        /* Add cases for opaque driver here */
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_TEST_DRIVER_LIFETIME:
            operation->ctx = mbedtls_calloc( 1, sizeof(test_opaque_cipher_operation_t) );
            if( operation->ctx == NULL )
                return( PSA_ERROR_INSUFFICIENT_MEMORY );

            status = test_opaque_cipher_encrypt_setup( operation->ctx,
                                                       &attributes,
                                                       slot->key.data,
                                                       slot->key.bytes,
                                                       alg );
            if( status == PSA_SUCCESS )
                operation->id = PSA_CRYPTO_OPAQUE_TEST_DRIVER_ID;
            else
            {
                mbedtls_platform_zeroize(
                    operation->ctx,
                    sizeof( test_opaque_cipher_operation_t ) );
                mbedtls_free( operation->ctx );
                operation->ctx = NULL;
            }

            return( status );
#endif /* PSA_CRYPTO_DRIVER_TEST */
        default:
            /* Key is declared with a lifetime not known to us */
            return( PSA_ERROR_NOT_SUPPORTED );
    }
#else /* PSA_CRYPTO_DRIVER_PRESENT */
    (void)slot;
    (void)alg;
    (void)operation;

    return( PSA_ERROR_NOT_SUPPORTED );
#endif /* PSA_CRYPTO_DRIVER_PRESENT */
}

psa_status_t psa_driver_wrapper_cipher_decrypt_setup(
    psa_operation_driver_context_t *operation,
    psa_key_slot_t *slot,
    psa_algorithm_t alg )
{
#if defined(PSA_CRYPTO_DRIVER_PRESENT) && defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
    psa_status_t status = PSA_ERROR_INVALID_ARGUMENT;
    psa_key_location_t location = PSA_KEY_LIFETIME_GET_LOCATION(slot->attr.lifetime);
    psa_key_attributes_t attributes = {
      .core = slot->attr
    };

    switch( location )
    {
        case PSA_KEY_LOCATION_LOCAL_STORAGE:
            /* Key is stored in the slot in export representation, so
             * cycle through all known transparent accelerators */
#if defined(PSA_CRYPTO_DRIVER_TEST)
            operation->ctx = mbedtls_calloc( 1, sizeof(test_transparent_cipher_operation_t) );
            if( operation->ctx == NULL )
                return( PSA_ERROR_INSUFFICIENT_MEMORY );

            status = test_transparent_cipher_decrypt_setup( operation->ctx,
                                                            &attributes,
                                                            slot->key.data,
                                                            slot->key.bytes,
                                                            alg );
            /* Declared with fallback == true */
            if( status == PSA_SUCCESS )
                operation->id = PSA_CRYPTO_TRANSPARENT_TEST_DRIVER_ID;
            else
            {
                mbedtls_platform_zeroize(
                    operation->ctx,
                    sizeof( test_transparent_cipher_operation_t ) );
                mbedtls_free( operation->ctx );
                operation->ctx = NULL;
            }

            return( status );
#endif /* PSA_CRYPTO_DRIVER_TEST */
            /* Fell through, meaning no accelerator supports this operation */
            return( PSA_ERROR_NOT_SUPPORTED );
        /* Add cases for opaque driver here */
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_TEST_DRIVER_LIFETIME:
            operation->ctx = mbedtls_calloc( 1, sizeof(test_opaque_cipher_operation_t) );
            if( operation->ctx == NULL )
                return PSA_ERROR_INSUFFICIENT_MEMORY;

            status = test_opaque_cipher_decrypt_setup( operation->ctx,
                                                       &attributes,
                                                       slot->key.data,
                                                       slot->key.bytes,
                                                       alg );
            if( status == PSA_SUCCESS )
                operation->id = PSA_CRYPTO_OPAQUE_TEST_DRIVER_ID;
            else
            {
                mbedtls_platform_zeroize(
                    operation->ctx,
                    sizeof( test_opaque_cipher_operation_t ) );
                mbedtls_free( operation->ctx );
                operation->ctx = NULL;
            }

            return( status );
#endif /* PSA_CRYPTO_DRIVER_TEST */
        default:
            /* Key is declared with a lifetime not known to us */
            return( PSA_ERROR_NOT_SUPPORTED );
    }
#else /* PSA_CRYPTO_DRIVER_PRESENT */
    (void)slot;
    (void)alg;
    (void)operation;

    return( PSA_ERROR_NOT_SUPPORTED );
#endif /* PSA_CRYPTO_DRIVER_PRESENT */
}

psa_status_t psa_driver_wrapper_cipher_generate_iv(
    psa_operation_driver_context_t *operation,
    uint8_t *iv,
    size_t iv_size,
    size_t *iv_length )
{
#if defined(PSA_CRYPTO_DRIVER_PRESENT) && defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
    switch( operation->id )
    {
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_TRANSPARENT_TEST_DRIVER_ID:
            return( test_transparent_cipher_generate_iv( operation->ctx,
                                                         iv,
                                                         iv_size,
                                                         iv_length ) );
#endif /* PSA_CRYPTO_DRIVER_TEST */
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_OPAQUE_TEST_DRIVER_ID:
            return( test_opaque_cipher_generate_iv( operation->ctx,
                                                    iv,
                                                    iv_size,
                                                    iv_length ) );
#endif /* PSA_CRYPTO_DRIVER_TEST */
        default:
            /* Key is attached to a driver not known to us */
            return( PSA_ERROR_BAD_STATE );
    }
#else /* PSA_CRYPTO_DRIVER_PRESENT */
    (void) operation;
    (void) iv;
    (void) iv_size;
    (void) iv_length;

    return( PSA_ERROR_NOT_SUPPORTED );
#endif /* PSA_CRYPTO_DRIVER_PRESENT */
}

psa_status_t psa_driver_wrapper_cipher_set_iv(
    psa_operation_driver_context_t *operation,
    const uint8_t *iv,
    size_t iv_length )
{
#if defined(PSA_CRYPTO_DRIVER_PRESENT) && defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
    switch( operation->id )
    {
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_TRANSPARENT_TEST_DRIVER_ID:
            return( test_transparent_cipher_set_iv( operation->ctx,
                                                    iv,
                                                    iv_length ) );
#endif /* PSA_CRYPTO_DRIVER_TEST */
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_OPAQUE_TEST_DRIVER_ID:
            return( test_opaque_cipher_set_iv( operation->ctx,
                                               iv,
                                               iv_length ) );
#endif /* PSA_CRYPTO_DRIVER_TEST */
        default:
            /* Key is attached to a driver not known to us */
            return( PSA_ERROR_BAD_STATE );
    }
#else /* PSA_CRYPTO_DRIVER_PRESENT */
    (void) operation;
    (void) iv;
    (void) iv_length;

    return( PSA_ERROR_NOT_SUPPORTED );
#endif /* PSA_CRYPTO_DRIVER_PRESENT */
}

psa_status_t psa_driver_wrapper_cipher_update(
    psa_operation_driver_context_t *operation,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length )
{
#if defined(PSA_CRYPTO_DRIVER_PRESENT) && defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
    switch( operation->id )
    {
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_TRANSPARENT_TEST_DRIVER_ID:
            return( test_transparent_cipher_update( operation->ctx,
                                                    input,
                                                    input_length,
                                                    output,
                                                    output_size,
                                                    output_length ) );
#endif /* PSA_CRYPTO_DRIVER_TEST */
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_OPAQUE_TEST_DRIVER_ID:
            return( test_opaque_cipher_update( operation->ctx,
                                               input,
                                               input_length,
                                               output,
                                               output_size,
                                               output_length ) );
#endif /* PSA_CRYPTO_DRIVER_TEST */
        default:
            /* Key is attached to a driver not known to us */
            return( PSA_ERROR_BAD_STATE );
    }
#else /* PSA_CRYPTO_DRIVER_PRESENT */
    (void) operation;
    (void) input;
    (void) input_length;
    (void) output;
    (void) output_length;
    (void) output_size;

    return( PSA_ERROR_NOT_SUPPORTED );
#endif /* PSA_CRYPTO_DRIVER_PRESENT */
}

psa_status_t psa_driver_wrapper_cipher_finish(
    psa_operation_driver_context_t *operation,
    uint8_t *output,
    size_t output_size,
    size_t *output_length )
{
#if defined(PSA_CRYPTO_DRIVER_PRESENT) && defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
    switch( operation->id )
    {
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_TRANSPARENT_TEST_DRIVER_ID:
            return( test_transparent_cipher_finish( operation->ctx,
                                                    output,
                                                    output_size,
                                                    output_length ) );
#endif /* PSA_CRYPTO_DRIVER_TEST */
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_OPAQUE_TEST_DRIVER_ID:
            return( test_opaque_cipher_finish( operation->ctx,
                                               output,
                                               output_size,
                                               output_length ) );
#endif /* PSA_CRYPTO_DRIVER_TEST */
        default:
            /* Key is attached to a driver not known to us */
            return( PSA_ERROR_BAD_STATE );
    }
#else /* PSA_CRYPTO_DRIVER_PRESENT */
    (void) operation;
    (void) output;
    (void) output_size;
    (void) output_length;

    return( PSA_ERROR_NOT_SUPPORTED );
#endif /* PSA_CRYPTO_DRIVER_PRESENT */
}

psa_status_t psa_driver_wrapper_cipher_abort(
    psa_operation_driver_context_t *operation )
{
#if defined(PSA_CRYPTO_DRIVER_PRESENT) && defined(PSA_CRYPTO_ACCELERATOR_DRIVER_PRESENT)
    psa_status_t status = PSA_ERROR_INVALID_ARGUMENT;

    /* The object has (apparently) been initialized but it is not in use. It's
     * ok to call abort on such an object, and there's nothing to do. */
    if( operation->ctx == NULL && operation->id == 0 )
        return( PSA_SUCCESS );

    switch( operation->id )
    {
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_TRANSPARENT_TEST_DRIVER_ID:
            status = test_transparent_cipher_abort( operation->ctx );
            mbedtls_platform_zeroize(
                operation->ctx,
                sizeof( test_transparent_cipher_operation_t ) );
            mbedtls_free( operation->ctx );
            operation->ctx = NULL;
            operation->id = 0;

            return( status );
#endif /* PSA_CRYPTO_DRIVER_TEST */
#if defined(PSA_CRYPTO_DRIVER_TEST)
        case PSA_CRYPTO_OPAQUE_TEST_DRIVER_ID:
            status = test_opaque_cipher_abort( operation->ctx );
            mbedtls_platform_zeroize(
                operation->ctx,
                sizeof( test_opaque_cipher_operation_t ) );
            mbedtls_free( operation->ctx );
            operation->ctx = NULL;
            operation->id = 0;

            return( status );
#endif /* PSA_CRYPTO_DRIVER_TEST */
        default:
            /* Operation is attached to a driver not known to us */
            return( PSA_ERROR_BAD_STATE );
    }
#else /* PSA_CRYPTO_DRIVER_PRESENT */
    (void)operation;

    return( PSA_ERROR_NOT_SUPPORTED );
#endif /* PSA_CRYPTO_DRIVER_PRESENT */
}

/* End of automatically generated file. */
