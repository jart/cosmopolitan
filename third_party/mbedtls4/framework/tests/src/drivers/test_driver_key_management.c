/*
 * Test driver for generating and verifying keys.
 * Currently only supports generating and verifying ECC keys.
 */
/*  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include <test/helpers.h>

#if defined(PSA_CRYPTO_DRIVER_TEST)
#include "psa/crypto.h"
#include "psa_crypto_core.h"
#include "psa_crypto_ecp.h"
#include "psa_crypto_rsa.h"
#include "psa_crypto_ffdh.h"

#if !defined(MBEDTLS_VERSION_MAJOR) || MBEDTLS_VERSION_MAJOR >= 4
#include "mbedtls/private/ecp.h"
#else
#include "mbedtls/ecp.h"
#endif

#include "test/drivers/key_management.h"
#include "test/drivers/test_driver.h"

#include "test/random.h"

#if defined(MBEDTLS_TEST_LIBTESTDRIVER1)
#include LIBTESTDRIVER1_PSA_DRIVER_INTERNAL_HEADER(psa_crypto_ecp.h)
#include LIBTESTDRIVER1_PSA_DRIVER_INTERNAL_HEADER(psa_crypto_rsa.h)
#include LIBTESTDRIVER1_PSA_DRIVER_INTERNAL_HEADER(psa_crypto_ffdh.h)
#endif /* MBEDTLS_TEST_LIBTESTDRIVER1 */

#include <string.h>

mbedtls_test_driver_key_management_hooks_t
    mbedtls_test_driver_key_management_hooks = MBEDTLS_TEST_DRIVER_KEY_MANAGEMENT_INIT;

const uint8_t mbedtls_test_driver_aes_key[16] =
{ 0x36, 0x77, 0x39, 0x7A, 0x24, 0x43, 0x26, 0x46,
  0x29, 0x4A, 0x40, 0x4E, 0x63, 0x52, 0x66, 0x55 };
const uint8_t mbedtls_test_driver_ecdsa_key[32] =
{ 0xdc, 0x7d, 0x9d, 0x26, 0xd6, 0x7a, 0x4f, 0x63,
  0x2c, 0x34, 0xc2, 0xdc, 0x0b, 0x69, 0x86, 0x18,
  0x38, 0x82, 0xc2, 0x06, 0xdf, 0x04, 0xcd, 0xb7,
  0xd6, 0x9a, 0xab, 0xe2, 0x8b, 0xe4, 0xf8, 0x1a };
const uint8_t mbedtls_test_driver_ecdsa_pubkey[65] =
{ 0x04,
  0x85, 0xf6, 0x4d, 0x89, 0xf0, 0x0b, 0xe6, 0x6c,
  0x88, 0xdd, 0x93, 0x7e, 0xfd, 0x6d, 0x7c, 0x44,
  0x56, 0x48, 0xdc, 0xb7, 0x01, 0x15, 0x0b, 0x8a,
  0x95, 0x09, 0x29, 0x58, 0x50, 0xf4, 0x1c, 0x19,
  0x31, 0xe5, 0x71, 0xfb, 0x8f, 0x8c, 0x78, 0x31,
  0x7a, 0x20, 0xb3, 0x80, 0xe8, 0x66, 0x58, 0x4b,
  0xbc, 0x25, 0x16, 0xc3, 0xd2, 0x70, 0x2d, 0x79,
  0x2f, 0x13, 0x1a, 0x92, 0x20, 0x95, 0xfd, 0x6c };

psa_status_t mbedtls_test_transparent_init(void)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

#if !defined(TF_PSA_CRYPTO_TEST_LIBTESTDRIVER1) && \
    defined(MBEDTLS_TEST_LIBTESTDRIVER1)
    status = libtestdriver1_psa_crypto_init();
    if (status != PSA_SUCCESS) {
        return status;
    }
#endif

    (void) status;
    return PSA_SUCCESS;
}

void mbedtls_test_transparent_free(void)
{
#if !defined(TF_PSA_CRYPTO_TEST_LIBTESTDRIVER1) && \
    defined(MBEDTLS_TEST_LIBTESTDRIVER1)
    libtestdriver1_mbedtls_psa_crypto_free();
#endif

    return;
}

psa_status_t mbedtls_test_opaque_init(void)
{
    return PSA_SUCCESS;
}

void mbedtls_test_opaque_free(void)
{
    return;
}

/*
 * This macro returns the base size for the key context when SE does not
 * support storage. It is the size of the metadata that gets added to the
 * wrapped key. In its test functionality the metadata is just some padded
 * prefixing to the key.
 */
#define TEST_DRIVER_KEY_CONTEXT_BASE_SIZE  \
    PSA_CRYPTO_TEST_DRIVER_OPAQUE_PAD_PREFIX_SIZE


size_t mbedtls_test_opaque_size_function(
    const psa_key_type_t key_type,
    const size_t key_bits)
{
    size_t key_buffer_size = 0;

    key_buffer_size = PSA_EXPORT_KEY_OUTPUT_SIZE(key_type, key_bits);
    if (key_buffer_size == 0) {
        return 0;
    }
    /* Include spacing for base size overhead over the key size
     * */
    key_buffer_size += TEST_DRIVER_KEY_CONTEXT_BASE_SIZE;
    return key_buffer_size;
}

static size_t mbedtls_test_opaque_get_base_size()
{
    return TEST_DRIVER_KEY_CONTEXT_BASE_SIZE;
}

/*
 * The wrap function mbedtls_test_opaque_wrap_key pads and wraps the
 * clear key. It expects the clear and wrap buffers to be passed in.
 * key_length is the size of the clear key to be wrapped.
 * wrapped_key_buffer_size is the size of the output buffer wrap_key.
 * The argument wrapped_key_buffer_length is filled with the wrapped
 * key_size on success.
 * */
static psa_status_t mbedtls_test_opaque_wrap_key(
    const uint8_t *key,
    size_t key_length,
    uint8_t *wrapped_key_buffer,
    size_t wrapped_key_buffer_size,
    size_t *wrapped_key_buffer_length)
{
    size_t opaque_key_base_size = mbedtls_test_opaque_get_base_size();
    uint64_t prefix = PSA_CRYPTO_TEST_DRIVER_OPAQUE_PAD_PREFIX;

    if (key_length + opaque_key_base_size > wrapped_key_buffer_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    /* Write in the opaque pad prefix */
    memcpy(wrapped_key_buffer, &prefix, opaque_key_base_size);
    wrapped_key_buffer += opaque_key_base_size;
    *wrapped_key_buffer_length = key_length + opaque_key_base_size;

    while (key_length--) {
        wrapped_key_buffer[key_length] = key[key_length] ^ 0xFF;
    }
    return PSA_SUCCESS;
}

/*
 * The unwrap function mbedtls_test_opaque_unwrap_key removes a pad prefix
 * and unwraps the wrapped key. It expects the clear and wrap buffers to be
 * passed in.
 * wrapped_key_length is the size of the wrapped key,
 * key_buffer_size is the size of the output buffer clear_key.
 * The argument key_buffer_length is filled with the unwrapped(clear)
 * key_size on success.
 * */
psa_status_t mbedtls_test_opaque_unwrap_key(
    const uint8_t *wrapped_key,
    size_t wrapped_key_length,
    uint8_t *key_buffer,
    size_t key_buffer_size,
    size_t *key_buffer_length)
{
    /* Remove the pad prefix from the wrapped key */
    size_t opaque_key_base_size = mbedtls_test_opaque_get_base_size();
    size_t clear_key_size;

    /* Check for underflow */
    if (wrapped_key_length < opaque_key_base_size) {
        return PSA_ERROR_DATA_CORRUPT;
    }
    clear_key_size = wrapped_key_length - opaque_key_base_size;

    wrapped_key += opaque_key_base_size;
    if (clear_key_size > key_buffer_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    *key_buffer_length = clear_key_size;
    while (clear_key_size--) {
        key_buffer[clear_key_size] = wrapped_key[clear_key_size] ^ 0xFF;
    }
    return PSA_SUCCESS;
}

psa_status_t mbedtls_test_transparent_generate_key(
    const psa_key_attributes_t *attributes,
    uint8_t *key, size_t key_size, size_t *key_length)
{
    ++mbedtls_test_driver_key_management_hooks.hits;
    ++mbedtls_test_driver_key_management_hooks.hits_generate_key;

    if (mbedtls_test_driver_key_management_hooks.forced_status != PSA_SUCCESS) {
        return mbedtls_test_driver_key_management_hooks.forced_status;
    }

    if (mbedtls_test_driver_key_management_hooks.forced_output != NULL) {
        if (mbedtls_test_driver_key_management_hooks.forced_output_length >
            key_size) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }
        memcpy(key, mbedtls_test_driver_key_management_hooks.forced_output,
               mbedtls_test_driver_key_management_hooks.forced_output_length);
        *key_length = mbedtls_test_driver_key_management_hooks.forced_output_length;
        return PSA_SUCCESS;
    }

    if (PSA_KEY_TYPE_IS_ECC(psa_get_key_type(attributes))
        && PSA_KEY_TYPE_IS_KEY_PAIR(psa_get_key_type(attributes))) {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR_GENERATE)
        return libtestdriver1_mbedtls_psa_ecp_generate_key(
            (const libtestdriver1_psa_key_attributes_t *) attributes,
            key, key_size, key_length);
#elif defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR_GENERATE)
        return mbedtls_psa_ecp_generate_key(
            attributes, key, key_size, key_length);
#endif
    } else if (psa_get_key_type(attributes) == PSA_KEY_TYPE_RSA_KEY_PAIR) {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR_GENERATE)
        return libtestdriver1_mbedtls_psa_rsa_generate_key(
            (const libtestdriver1_psa_key_attributes_t *) attributes,
            NULL, 0, /* We don't support custom e in the test driver yet */
            key, key_size, key_length);
#elif defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR_GENERATE)
        return mbedtls_psa_rsa_generate_key(
            attributes,
            NULL, 0, /* We don't support custom e in the test driver yet */
            key, key_size, key_length);
#endif
    } else if (PSA_KEY_TYPE_IS_DH(psa_get_key_type(attributes))
               && PSA_KEY_TYPE_IS_KEY_PAIR(psa_get_key_type(attributes))) {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_KEY_TYPE_DH_KEY_PAIR_GENERATE)
        return libtestdriver1_mbedtls_psa_ffdh_generate_key(
            (const libtestdriver1_psa_key_attributes_t *) attributes,
            key, key_size, key_length);
#elif defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_DH_KEY_PAIR)
        return mbedtls_psa_ffdh_generate_key(
            attributes, key, key_size, key_length);
#endif
    }

    (void) attributes;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t mbedtls_test_opaque_generate_key(
    const psa_key_attributes_t *attributes,
    uint8_t *key, size_t key_size, size_t *key_length)
{
    (void) attributes;
    (void) key;
    (void) key_size;
    (void) key_length;
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t mbedtls_test_transparent_import_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *data,
    size_t data_length,
    uint8_t *key_buffer,
    size_t key_buffer_size,
    size_t *key_buffer_length,
    size_t *bits)
{
    psa_key_type_t type = psa_get_key_type(attributes);

    ++mbedtls_test_driver_key_management_hooks.hits;
    mbedtls_test_driver_key_management_hooks.location = PSA_KEY_LOCATION_LOCAL_STORAGE;

    if (mbedtls_test_driver_key_management_hooks.forced_status != PSA_SUCCESS) {
        return mbedtls_test_driver_key_management_hooks.forced_status;
    }

    if (PSA_KEY_TYPE_IS_ECC(type)) {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        (defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR_IMPORT) || \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_PUBLIC_KEY))
        return libtestdriver1_mbedtls_psa_ecp_import_key(
            (const libtestdriver1_psa_key_attributes_t *) attributes,
            data, data_length,
            key_buffer, key_buffer_size,
            key_buffer_length, bits);
#elif defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR_IMPORT) || \
        defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_PUBLIC_KEY)
        return mbedtls_psa_ecp_import_key(
            attributes,
            data, data_length,
            key_buffer, key_buffer_size,
            key_buffer_length, bits);
#endif
    } else if (PSA_KEY_TYPE_IS_RSA(type)) {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        (defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR_IMPORT) || \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY))
        return libtestdriver1_mbedtls_psa_rsa_import_key(
            (const libtestdriver1_psa_key_attributes_t *) attributes,
            data, data_length,
            key_buffer, key_buffer_size,
            key_buffer_length, bits);
#elif defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR_IMPORT) || \
        defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY)
        return mbedtls_psa_rsa_import_key(
            attributes,
            data, data_length,
            key_buffer, key_buffer_size,
            key_buffer_length, bits);
#endif
    } else if (PSA_KEY_TYPE_IS_DH(type)) {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        (defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_KEY_TYPE_DH_KEY_PAIR) || \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_KEY_TYPE_DH_PUBLIC_KEY))
        return libtestdriver1_mbedtls_psa_ffdh_import_key(
            (const libtestdriver1_psa_key_attributes_t *) attributes,
            data, data_length,
            key_buffer, key_buffer_size,
            key_buffer_length, bits);
#elif defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR) || \
        defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY)
        return mbedtls_psa_ffdh_import_key(
            attributes,
            data, data_length,
            key_buffer, key_buffer_size,
            key_buffer_length, bits);
#endif
    }
    (void) data;
    (void) data_length;
    (void) key_buffer;
    (void) key_buffer_size;
    (void) key_buffer_length;
    (void) bits;
    (void) type;

    return PSA_ERROR_NOT_SUPPORTED;
}


psa_status_t mbedtls_test_opaque_import_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *data,
    size_t data_length,
    uint8_t *key_buffer,
    size_t key_buffer_size,
    size_t *key_buffer_length,
    size_t *bits)
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_type_t type = psa_get_key_type(attributes);
    /* This buffer will be used as an intermediate placeholder for
     * the clear key till we wrap it */
    uint8_t *key_buffer_temp;

    ++mbedtls_test_driver_key_management_hooks.hits;
    mbedtls_test_driver_key_management_hooks.location = PSA_CRYPTO_TEST_DRIVER_LOCATION;

    if (mbedtls_test_driver_key_management_hooks.forced_status != PSA_SUCCESS) {
        return mbedtls_test_driver_key_management_hooks.forced_status;
    }

    key_buffer_temp = mbedtls_calloc(1, key_buffer_size);
    if (key_buffer_temp == NULL) {
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    }

    if (PSA_KEY_TYPE_IS_UNSTRUCTURED(type)) {
        *bits = PSA_BYTES_TO_BITS(data_length);

        status = psa_validate_unstructured_key_bit_size(type,
                                                        *bits);
        if (status != PSA_SUCCESS) {
            goto exit;
        }

        if (data_length > key_buffer_size) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }

        /* Copy the key material accounting for opaque key padding. */
        memcpy(key_buffer_temp, data, data_length);
        *key_buffer_length = data_length;
    } else if (PSA_KEY_TYPE_IS_ECC(type)) {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        (defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR) || \
        defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY))
        status = libtestdriver1_mbedtls_psa_ecp_import_key(
            (const libtestdriver1_psa_key_attributes_t *) attributes,
            data, data_length,
            key_buffer_temp, key_buffer_size,
            key_buffer_length, bits);
#elif defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR_IMPORT) || \
        defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_PUBLIC_KEY)
        status = mbedtls_psa_ecp_import_key(
            attributes,
            data, data_length,
            key_buffer_temp, key_buffer_size,
            key_buffer_length, bits);
#else
        status = PSA_ERROR_NOT_SUPPORTED;
#endif
        if (status != PSA_SUCCESS) {
            goto exit;
        }
    } else if (PSA_KEY_TYPE_IS_RSA(type)) {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        (defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_RSA_KEY_PAIR) || \
        defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_RSA_PUBLIC_KEY))
        status = libtestdriver1_mbedtls_psa_rsa_import_key(
            (const libtestdriver1_psa_key_attributes_t *) attributes,
            data, data_length,
            key_buffer_temp, key_buffer_size,
            key_buffer_length, bits);
#elif defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR_IMPORT) || \
        defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY)
        status = mbedtls_psa_rsa_import_key(
            attributes,
            data, data_length,
            key_buffer_temp, key_buffer_size,
            key_buffer_length, bits);
#else
        status = PSA_ERROR_NOT_SUPPORTED;
#endif
        if (status != PSA_SUCCESS) {
            goto exit;
        }
    } else {
        status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    status = mbedtls_test_opaque_wrap_key(key_buffer_temp, *key_buffer_length,
                                          key_buffer, key_buffer_size, key_buffer_length);
exit:
    mbedtls_free(key_buffer_temp);
    return status;
}

psa_status_t mbedtls_test_opaque_export_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    uint8_t *data, size_t data_size, size_t *data_length)
{
    if (key_length == sizeof(psa_drv_slot_number_t)) {
        /* Assume this is a builtin key based on the key material length. */
        psa_drv_slot_number_t slot_number = *((psa_drv_slot_number_t *) key);

        switch (slot_number) {
            case PSA_CRYPTO_TEST_DRIVER_BUILTIN_ECDSA_KEY_SLOT:
                /* This is the ECDSA slot. Verify the key's attributes before
                 * returning the private key. */
                if (psa_get_key_type(attributes) !=
                    PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)) {
                    return PSA_ERROR_CORRUPTION_DETECTED;
                }
                if (psa_get_key_bits(attributes) != 256) {
                    return PSA_ERROR_CORRUPTION_DETECTED;
                }
                if (psa_get_key_algorithm(attributes) !=
                    PSA_ALG_ECDSA(PSA_ALG_ANY_HASH)) {
                    return PSA_ERROR_CORRUPTION_DETECTED;
                }
                if ((psa_get_key_usage_flags(attributes) &
                     PSA_KEY_USAGE_EXPORT) == 0) {
                    return PSA_ERROR_CORRUPTION_DETECTED;
                }

                if (data_size < sizeof(mbedtls_test_driver_ecdsa_key)) {
                    return PSA_ERROR_BUFFER_TOO_SMALL;
                }

                memcpy(data, mbedtls_test_driver_ecdsa_key,
                       sizeof(mbedtls_test_driver_ecdsa_key));
                *data_length = sizeof(mbedtls_test_driver_ecdsa_key);
                return PSA_SUCCESS;

            case PSA_CRYPTO_TEST_DRIVER_BUILTIN_AES_KEY_SLOT:
                /* This is the AES slot. Verify the key's attributes before
                 * returning the key. */
                if (psa_get_key_type(attributes) != PSA_KEY_TYPE_AES) {
                    return PSA_ERROR_CORRUPTION_DETECTED;
                }
                if (psa_get_key_bits(attributes) != 128) {
                    return PSA_ERROR_CORRUPTION_DETECTED;
                }
                if (psa_get_key_algorithm(attributes) != PSA_ALG_CTR) {
                    return PSA_ERROR_CORRUPTION_DETECTED;
                }
                if ((psa_get_key_usage_flags(attributes) &
                     PSA_KEY_USAGE_EXPORT) == 0) {
                    return PSA_ERROR_CORRUPTION_DETECTED;
                }

                if (data_size < sizeof(mbedtls_test_driver_aes_key)) {
                    return PSA_ERROR_BUFFER_TOO_SMALL;
                }

                memcpy(data, mbedtls_test_driver_aes_key,
                       sizeof(mbedtls_test_driver_aes_key));
                *data_length = sizeof(mbedtls_test_driver_aes_key);
                return PSA_SUCCESS;

            default:
                return PSA_ERROR_DOES_NOT_EXIST;
        }
    } else {
        /* This buffer will be used as an intermediate placeholder for
         * the opaque key till we unwrap the key into key_buffer */
        psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
        psa_key_type_t type = psa_get_key_type(attributes);

        if (PSA_KEY_TYPE_IS_UNSTRUCTURED(type) ||
            PSA_KEY_TYPE_IS_RSA(type)   ||
            PSA_KEY_TYPE_IS_ECC(type)) {
            status = mbedtls_test_opaque_unwrap_key(key, key_length,
                                                    data, data_size, data_length);
            return status;
        }
    }
    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t mbedtls_test_transparent_export_public_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    uint8_t *data, size_t data_size, size_t *data_length)
{
    ++mbedtls_test_driver_key_management_hooks.hits;
    ++mbedtls_test_driver_key_management_hooks.hits_export_public_key;

    if (mbedtls_test_driver_key_management_hooks.forced_status != PSA_SUCCESS) {
        return mbedtls_test_driver_key_management_hooks.forced_status;
    }

    if (mbedtls_test_driver_key_management_hooks.forced_output != NULL) {
        if (mbedtls_test_driver_key_management_hooks.forced_output_length >
            data_size) {
            return PSA_ERROR_BUFFER_TOO_SMALL;
        }
        memcpy(data, mbedtls_test_driver_key_management_hooks.forced_output,
               mbedtls_test_driver_key_management_hooks.forced_output_length);
        *data_length = mbedtls_test_driver_key_management_hooks.forced_output_length;
        return PSA_SUCCESS;
    }

    psa_key_type_t key_type = psa_get_key_type(attributes);

    if (PSA_KEY_TYPE_IS_ECC(key_type)) {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        (defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR_EXPORT) || \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_PUBLIC_KEY))
        return libtestdriver1_mbedtls_psa_ecp_export_public_key(
            (const libtestdriver1_psa_key_attributes_t *) attributes,
            key_buffer, key_buffer_size,
            data, data_size, data_length);
#elif defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR_EXPORT) || \
        defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_PUBLIC_KEY)
        return mbedtls_psa_ecp_export_public_key(
            attributes,
            key_buffer, key_buffer_size,
            data, data_size, data_length);
#endif
    } else if (PSA_KEY_TYPE_IS_RSA(key_type)) {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        (defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR_EXPORT) || \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY))
        return libtestdriver1_mbedtls_psa_rsa_export_public_key(
            (const libtestdriver1_psa_key_attributes_t *) attributes,
            key_buffer, key_buffer_size,
            data, data_size, data_length);
#elif defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR_EXPORT) || \
        defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY)
        return mbedtls_psa_rsa_export_public_key(
            attributes,
            key_buffer, key_buffer_size,
            data, data_size, data_length);
#endif
    } else if (PSA_KEY_TYPE_IS_DH(key_type)) {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
        (defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_KEY_TYPE_DH_KEY_PAIR) || \
        defined(LIBTESTDRIVER1_MBEDTLS_PSA_BUILTIN_KEY_TYPE_DH_PUBLIC_KEY))
        return libtestdriver1_mbedtls_psa_ffdh_export_public_key(
            (const libtestdriver1_psa_key_attributes_t *) attributes,
            key_buffer, key_buffer_size,
            data, data_size, data_length);
#elif defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_DH_KEY_PAIR) || \
        defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_DH_PUBLIC_KEY)
        return mbedtls_psa_ffdh_export_public_key(
            attributes,
            key_buffer, key_buffer_size,
            data, data_size, data_length);
#endif
    }

    (void) key_buffer;
    (void) key_buffer_size;
    (void) key_type;

    return PSA_ERROR_NOT_SUPPORTED;
}

psa_status_t mbedtls_test_opaque_export_public_key(
    const psa_key_attributes_t *attributes,
    const uint8_t *key, size_t key_length,
    uint8_t *data, size_t data_size, size_t *data_length)
{
    if (key_length != sizeof(psa_drv_slot_number_t)) {
        psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
        psa_key_type_t key_type = psa_get_key_type(attributes);
        uint8_t *key_buffer_temp;

        key_buffer_temp = mbedtls_calloc(1, key_length);
        if (key_buffer_temp == NULL) {
            return PSA_ERROR_INSUFFICIENT_MEMORY;
        }

        if (PSA_KEY_TYPE_IS_ECC(key_type)) {
            status = mbedtls_test_opaque_unwrap_key(key, key_length,
                                                    key_buffer_temp, key_length, data_length);
            if (status == PSA_SUCCESS) {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
                (defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR) || \
                defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY))
                status = libtestdriver1_mbedtls_psa_ecp_export_public_key(
                    (const libtestdriver1_psa_key_attributes_t *) attributes,
                    key_buffer_temp, *data_length,
                    data, data_size, data_length);
#elif defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR_EXPORT) || \
                defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_PUBLIC_KEY)
                status = mbedtls_psa_ecp_export_public_key(
                    attributes,
                    key_buffer_temp, *data_length,
                    data, data_size, data_length);
#else
                status = PSA_ERROR_NOT_SUPPORTED;
#endif
            }
        } else if (PSA_KEY_TYPE_IS_RSA(key_type)) {
            status = mbedtls_test_opaque_unwrap_key(key, key_length,
                                                    key_buffer_temp, key_length, data_length);
            if (status == PSA_SUCCESS) {
#if defined(MBEDTLS_TEST_LIBTESTDRIVER1) && \
                (defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_RSA_KEY_PAIR) || \
                defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_RSA_PUBLIC_KEY))
                status = libtestdriver1_mbedtls_psa_rsa_export_public_key(
                    (const libtestdriver1_psa_key_attributes_t *) attributes,
                    key_buffer_temp, *data_length,
                    data, data_size, data_length);
#elif defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR_EXPORT) || \
                defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY)
                status = mbedtls_psa_rsa_export_public_key(
                    attributes,
                    key_buffer_temp, *data_length,
                    data, data_size, data_length);
#else
                status = PSA_ERROR_NOT_SUPPORTED;
#endif
            }
        } else {
            status = PSA_ERROR_NOT_SUPPORTED;
            (void) key;
            (void) key_type;
        }
        mbedtls_free(key_buffer_temp);
        return status;
    }

    /* Assume this is a builtin key based on the key material length. */
    psa_drv_slot_number_t slot_number = *((psa_drv_slot_number_t *) key);
    switch (slot_number) {
        case PSA_CRYPTO_TEST_DRIVER_BUILTIN_ECDSA_KEY_SLOT:
            /* This is the ECDSA slot. Verify the key's attributes before
             * returning the public key. */
            if (psa_get_key_type(attributes) !=
                PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1)) {
                return PSA_ERROR_CORRUPTION_DETECTED;
            }
            if (psa_get_key_bits(attributes) != 256) {
                return PSA_ERROR_CORRUPTION_DETECTED;
            }
            if (psa_get_key_algorithm(attributes) !=
                PSA_ALG_ECDSA(PSA_ALG_ANY_HASH)) {
                return PSA_ERROR_CORRUPTION_DETECTED;
            }

            if (data_size < sizeof(mbedtls_test_driver_ecdsa_pubkey)) {
                return PSA_ERROR_BUFFER_TOO_SMALL;
            }

            memcpy(data, mbedtls_test_driver_ecdsa_pubkey,
                   sizeof(mbedtls_test_driver_ecdsa_pubkey));
            *data_length = sizeof(mbedtls_test_driver_ecdsa_pubkey);
            return PSA_SUCCESS;

        default:
            return PSA_ERROR_DOES_NOT_EXIST;
    }
}

/* The opaque test driver exposes two built-in keys when builtin key support is
 * compiled in.
 * The key in slot #PSA_CRYPTO_TEST_DRIVER_BUILTIN_AES_KEY_SLOT is an AES-128
 * key which allows CTR mode.
 * The key in slot #PSA_CRYPTO_TEST_DRIVER_BUILTIN_ECDSA_KEY_SLOT is a secp256r1
 * private key which allows ECDSA sign & verify.
 * The key buffer format for these is the raw format of psa_drv_slot_number_t
 * (i.e. for an actual driver this would mean 'builtin_key_size' =
 * sizeof(psa_drv_slot_number_t)).
 */
psa_status_t mbedtls_test_opaque_get_builtin_key(
    psa_drv_slot_number_t slot_number,
    psa_key_attributes_t *attributes,
    uint8_t *key_buffer, size_t key_buffer_size, size_t *key_buffer_length)
{
    switch (slot_number) {
        case PSA_CRYPTO_TEST_DRIVER_BUILTIN_AES_KEY_SLOT:
            psa_set_key_type(attributes, PSA_KEY_TYPE_AES);
            psa_set_key_bits(attributes, 128);
            psa_set_key_usage_flags(
                attributes,
                PSA_KEY_USAGE_ENCRYPT |
                PSA_KEY_USAGE_DECRYPT |
                PSA_KEY_USAGE_EXPORT);
            psa_set_key_algorithm(attributes, PSA_ALG_CTR);

            if (key_buffer_size < sizeof(psa_drv_slot_number_t)) {
                return PSA_ERROR_BUFFER_TOO_SMALL;
            }

            *((psa_drv_slot_number_t *) key_buffer) =
                PSA_CRYPTO_TEST_DRIVER_BUILTIN_AES_KEY_SLOT;
            *key_buffer_length = sizeof(psa_drv_slot_number_t);
            return PSA_SUCCESS;
        case PSA_CRYPTO_TEST_DRIVER_BUILTIN_ECDSA_KEY_SLOT:
            psa_set_key_type(
                attributes,
                PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
            psa_set_key_bits(attributes, 256);
            psa_set_key_usage_flags(
                attributes,
                PSA_KEY_USAGE_SIGN_HASH |
                PSA_KEY_USAGE_VERIFY_HASH |
                PSA_KEY_USAGE_EXPORT);
            psa_set_key_algorithm(
                attributes, PSA_ALG_ECDSA(PSA_ALG_ANY_HASH));

            if (key_buffer_size < sizeof(psa_drv_slot_number_t)) {
                return PSA_ERROR_BUFFER_TOO_SMALL;
            }

            *((psa_drv_slot_number_t *) key_buffer) =
                PSA_CRYPTO_TEST_DRIVER_BUILTIN_ECDSA_KEY_SLOT;
            *key_buffer_length = sizeof(psa_drv_slot_number_t);
            return PSA_SUCCESS;
        default:
            return PSA_ERROR_DOES_NOT_EXIST;
    }
}

psa_status_t mbedtls_test_opaque_copy_key(
    psa_key_attributes_t *attributes,
    const uint8_t *source_key, size_t source_key_length,
    uint8_t *key_buffer, size_t key_buffer_size, size_t *key_buffer_length)
{
    /* This is a case where the opaque test driver emulates an SE without storage.
     * With that all key context is stored in the wrapped buffer.
     * So no additional house keeping is necessary to reference count the
     * copied keys. This could change when the opaque test driver is extended
     * to support SE with storage, or to emulate an SE without storage but
     * still holding some slot references */
    if (source_key_length > key_buffer_size) {
        return PSA_ERROR_BUFFER_TOO_SMALL;
    }

    memcpy(key_buffer, source_key, source_key_length);
    *key_buffer_length = source_key_length;
    (void) attributes;
    return PSA_SUCCESS;
}

#endif /* PSA_CRYPTO_DRIVER_TEST */
