/*
 *  PSA crypto layer on top of Mbed TLS crypto
 */
/*
 *  Copyright The Mbed TLS Contributors
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

#include "common.h"

#if defined(MBEDTLS_PSA_CRYPTO_C)

#if defined(MBEDTLS_PSA_CRYPTO_CONFIG)
#include "check_crypto_config.h"
#endif

#include "psa_crypto_service_integration.h"
#include "psa/crypto.h"

#include "psa_crypto_core.h"
#include "psa_crypto_invasive.h"
#include "psa_crypto_driver_wrappers.h"
#include "psa_crypto_ecp.h"
#include "psa_crypto_rsa.h"
#include "psa_crypto_ecp.h"
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
#include "psa_crypto_se.h"
#endif
#include "psa_crypto_slot_management.h"
/* Include internal declarations that are useful for implementing persistently
 * stored keys. */
#include "psa_crypto_storage.h"

#include "psa_crypto_random_impl.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "mbedtls/platform.h"
#if !defined(MBEDTLS_PLATFORM_C)
#define mbedtls_calloc calloc
#define mbedtls_free   free
#endif

#include "mbedtls/aes.h"
#include "mbedtls/arc4.h"
#include "mbedtls/asn1.h"
#include "mbedtls/asn1write.h"
#include "mbedtls/bignum.h"
#include "mbedtls/blowfish.h"
#include "mbedtls/camellia.h"
#include "mbedtls/chacha20.h"
#include "mbedtls/chachapoly.h"
#include "mbedtls/cipher.h"
#include "mbedtls/ccm.h"
#include "mbedtls/cmac.h"
#include "mbedtls/des.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ecp.h"
#include "mbedtls/entropy.h"
#include "mbedtls/error.h"
#include "mbedtls/gcm.h"
#include "mbedtls/md2.h"
#include "mbedtls/md4.h"
#include "mbedtls/md5.h"
#include "mbedtls/md.h"
#include "mbedtls/md_internal.h"
#include "mbedtls/pk.h"
#include "mbedtls/pk_internal.h"
#include "mbedtls/platform_util.h"
#include "mbedtls/error.h"
#include "mbedtls/ripemd160.h"
#include "mbedtls/rsa.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"
#include "mbedtls/xtea.h"

#define ARRAY_LENGTH( array ) ( sizeof( array ) / sizeof( *( array ) ) )

/* constant-time buffer comparison */
static inline int safer_memcmp( const uint8_t *a, const uint8_t *b, size_t n )
{
    size_t i;
    unsigned char diff = 0;

    for( i = 0; i < n; i++ )
        diff |= a[i] ^ b[i];

    return( diff );
}



/****************************************************************/
/* Global data, support functions and library management */
/****************************************************************/

static int key_type_is_raw_bytes( psa_key_type_t type )
{
    return( PSA_KEY_TYPE_IS_UNSTRUCTURED( type ) );
}

/* Values for psa_global_data_t::rng_state */
#define RNG_NOT_INITIALIZED 0
#define RNG_INITIALIZED 1
#define RNG_SEEDED 2

typedef struct
{
    mbedtls_psa_random_context_t rng;
    unsigned initialized : 1;
    unsigned rng_state : 2;
} psa_global_data_t;

static psa_global_data_t global_data;

#if !defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
mbedtls_psa_drbg_context_t *const mbedtls_psa_random_state =
    &global_data.rng.drbg;
#endif

#define GUARD_MODULE_INITIALIZED        \
    if( global_data.initialized == 0 )  \
        return( PSA_ERROR_BAD_STATE );

psa_status_t mbedtls_to_psa_error( int ret )
{
    /* Mbed TLS error codes can combine a high-level error code and a
     * low-level error code. The low-level error usually reflects the
     * root cause better, so dispatch on that preferably. */
    int low_level_ret = - ( -ret & 0x007f );
    switch( low_level_ret != 0 ? low_level_ret : ret )
    {
        case 0:
            return( PSA_SUCCESS );

        case MBEDTLS_ERR_AES_INVALID_KEY_LENGTH:
        case MBEDTLS_ERR_AES_INVALID_INPUT_LENGTH:
        case MBEDTLS_ERR_AES_FEATURE_UNAVAILABLE:
            return( PSA_ERROR_NOT_SUPPORTED );
        case MBEDTLS_ERR_AES_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );

        case MBEDTLS_ERR_ARC4_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );

        case MBEDTLS_ERR_ASN1_OUT_OF_DATA:
        case MBEDTLS_ERR_ASN1_UNEXPECTED_TAG:
        case MBEDTLS_ERR_ASN1_INVALID_LENGTH:
        case MBEDTLS_ERR_ASN1_LENGTH_MISMATCH:
        case MBEDTLS_ERR_ASN1_INVALID_DATA:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_ASN1_ALLOC_FAILED:
            return( PSA_ERROR_INSUFFICIENT_MEMORY );
        case MBEDTLS_ERR_ASN1_BUF_TOO_SMALL:
            return( PSA_ERROR_BUFFER_TOO_SMALL );

#if defined(MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA)
        case MBEDTLS_ERR_BLOWFISH_BAD_INPUT_DATA:
#elif defined(MBEDTLS_ERR_BLOWFISH_INVALID_KEY_LENGTH)
        case MBEDTLS_ERR_BLOWFISH_INVALID_KEY_LENGTH:
#endif
        case MBEDTLS_ERR_BLOWFISH_INVALID_INPUT_LENGTH:
            return( PSA_ERROR_NOT_SUPPORTED );
        case MBEDTLS_ERR_BLOWFISH_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );

#if defined(MBEDTLS_ERR_CAMELLIA_BAD_INPUT_DATA)
        case MBEDTLS_ERR_CAMELLIA_BAD_INPUT_DATA:
#elif defined(MBEDTLS_ERR_CAMELLIA_INVALID_KEY_LENGTH)
        case MBEDTLS_ERR_CAMELLIA_INVALID_KEY_LENGTH:
#endif
        case MBEDTLS_ERR_CAMELLIA_INVALID_INPUT_LENGTH:
            return( PSA_ERROR_NOT_SUPPORTED );
        case MBEDTLS_ERR_CAMELLIA_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );

        case MBEDTLS_ERR_CCM_BAD_INPUT:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_CCM_AUTH_FAILED:
            return( PSA_ERROR_INVALID_SIGNATURE );
        case MBEDTLS_ERR_CCM_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );

        case MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA:
            return( PSA_ERROR_INVALID_ARGUMENT );

        case MBEDTLS_ERR_CHACHAPOLY_BAD_STATE:
            return( PSA_ERROR_BAD_STATE );
        case MBEDTLS_ERR_CHACHAPOLY_AUTH_FAILED:
            return( PSA_ERROR_INVALID_SIGNATURE );

        case MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE:
            return( PSA_ERROR_NOT_SUPPORTED );
        case MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_CIPHER_ALLOC_FAILED:
            return( PSA_ERROR_INSUFFICIENT_MEMORY );
        case MBEDTLS_ERR_CIPHER_INVALID_PADDING:
            return( PSA_ERROR_INVALID_PADDING );
        case MBEDTLS_ERR_CIPHER_FULL_BLOCK_EXPECTED:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_CIPHER_AUTH_FAILED:
            return( PSA_ERROR_INVALID_SIGNATURE );
        case MBEDTLS_ERR_CIPHER_INVALID_CONTEXT:
            return( PSA_ERROR_CORRUPTION_DETECTED );
        case MBEDTLS_ERR_CIPHER_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );

        case MBEDTLS_ERR_CMAC_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );

#if !( defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG) ||      \
       defined(MBEDTLS_PSA_HMAC_DRBG_MD_TYPE) )
        /* Only check CTR_DRBG error codes if underlying mbedtls_xxx
         * functions are passed a CTR_DRBG instance. */
        case MBEDTLS_ERR_CTR_DRBG_ENTROPY_SOURCE_FAILED:
            return( PSA_ERROR_INSUFFICIENT_ENTROPY );
        case MBEDTLS_ERR_CTR_DRBG_REQUEST_TOO_BIG:
        case MBEDTLS_ERR_CTR_DRBG_INPUT_TOO_BIG:
            return( PSA_ERROR_NOT_SUPPORTED );
        case MBEDTLS_ERR_CTR_DRBG_FILE_IO_ERROR:
            return( PSA_ERROR_INSUFFICIENT_ENTROPY );
#endif

        case MBEDTLS_ERR_DES_INVALID_INPUT_LENGTH:
            return( PSA_ERROR_NOT_SUPPORTED );
        case MBEDTLS_ERR_DES_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );

        case MBEDTLS_ERR_ENTROPY_NO_SOURCES_DEFINED:
        case MBEDTLS_ERR_ENTROPY_NO_STRONG_SOURCE:
        case MBEDTLS_ERR_ENTROPY_SOURCE_FAILED:
            return( PSA_ERROR_INSUFFICIENT_ENTROPY );

        case MBEDTLS_ERR_GCM_AUTH_FAILED:
            return( PSA_ERROR_INVALID_SIGNATURE );
        case MBEDTLS_ERR_GCM_BAD_INPUT:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_GCM_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );

#if !defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG) &&        \
    defined(MBEDTLS_PSA_HMAC_DRBG_MD_TYPE)
        /* Only check HMAC_DRBG error codes if underlying mbedtls_xxx
         * functions are passed a HMAC_DRBG instance. */
        case MBEDTLS_ERR_HMAC_DRBG_ENTROPY_SOURCE_FAILED:
            return( PSA_ERROR_INSUFFICIENT_ENTROPY );
        case MBEDTLS_ERR_HMAC_DRBG_REQUEST_TOO_BIG:
        case MBEDTLS_ERR_HMAC_DRBG_INPUT_TOO_BIG:
            return( PSA_ERROR_NOT_SUPPORTED );
        case MBEDTLS_ERR_HMAC_DRBG_FILE_IO_ERROR:
            return( PSA_ERROR_INSUFFICIENT_ENTROPY );
#endif

        case MBEDTLS_ERR_MD2_HW_ACCEL_FAILED:
        case MBEDTLS_ERR_MD4_HW_ACCEL_FAILED:
        case MBEDTLS_ERR_MD5_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );

        case MBEDTLS_ERR_MD_FEATURE_UNAVAILABLE:
            return( PSA_ERROR_NOT_SUPPORTED );
        case MBEDTLS_ERR_MD_BAD_INPUT_DATA:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_MD_ALLOC_FAILED:
            return( PSA_ERROR_INSUFFICIENT_MEMORY );
        case MBEDTLS_ERR_MD_FILE_IO_ERROR:
            return( PSA_ERROR_STORAGE_FAILURE );
        case MBEDTLS_ERR_MD_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );

        case MBEDTLS_ERR_MPI_FILE_IO_ERROR:
            return( PSA_ERROR_STORAGE_FAILURE );
        case MBEDTLS_ERR_MPI_BAD_INPUT_DATA:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_MPI_INVALID_CHARACTER:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_MPI_BUFFER_TOO_SMALL:
            return( PSA_ERROR_BUFFER_TOO_SMALL );
        case MBEDTLS_ERR_MPI_NEGATIVE_VALUE:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_MPI_DIVISION_BY_ZERO:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_MPI_NOT_ACCEPTABLE:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_MPI_ALLOC_FAILED:
            return( PSA_ERROR_INSUFFICIENT_MEMORY );

        case MBEDTLS_ERR_PK_ALLOC_FAILED:
            return( PSA_ERROR_INSUFFICIENT_MEMORY );
        case MBEDTLS_ERR_PK_TYPE_MISMATCH:
        case MBEDTLS_ERR_PK_BAD_INPUT_DATA:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_PK_FILE_IO_ERROR:
            return( PSA_ERROR_STORAGE_FAILURE );
        case MBEDTLS_ERR_PK_KEY_INVALID_VERSION:
        case MBEDTLS_ERR_PK_KEY_INVALID_FORMAT:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_PK_UNKNOWN_PK_ALG:
            return( PSA_ERROR_NOT_SUPPORTED );
        case MBEDTLS_ERR_PK_PASSWORD_REQUIRED:
        case MBEDTLS_ERR_PK_PASSWORD_MISMATCH:
            return( PSA_ERROR_NOT_PERMITTED );
        case MBEDTLS_ERR_PK_INVALID_PUBKEY:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_PK_INVALID_ALG:
        case MBEDTLS_ERR_PK_UNKNOWN_NAMED_CURVE:
        case MBEDTLS_ERR_PK_FEATURE_UNAVAILABLE:
            return( PSA_ERROR_NOT_SUPPORTED );
        case MBEDTLS_ERR_PK_SIG_LEN_MISMATCH:
            return( PSA_ERROR_INVALID_SIGNATURE );
        case MBEDTLS_ERR_PK_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );

        case MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );
        case MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED:
            return( PSA_ERROR_NOT_SUPPORTED );

        case MBEDTLS_ERR_RIPEMD160_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );

        case MBEDTLS_ERR_RSA_BAD_INPUT_DATA:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_RSA_INVALID_PADDING:
            return( PSA_ERROR_INVALID_PADDING );
        case MBEDTLS_ERR_RSA_KEY_GEN_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );
        case MBEDTLS_ERR_RSA_KEY_CHECK_FAILED:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_RSA_PUBLIC_FAILED:
        case MBEDTLS_ERR_RSA_PRIVATE_FAILED:
            return( PSA_ERROR_CORRUPTION_DETECTED );
        case MBEDTLS_ERR_RSA_VERIFY_FAILED:
            return( PSA_ERROR_INVALID_SIGNATURE );
        case MBEDTLS_ERR_RSA_OUTPUT_TOO_LARGE:
            return( PSA_ERROR_BUFFER_TOO_SMALL );
        case MBEDTLS_ERR_RSA_RNG_FAILED:
            return( PSA_ERROR_INSUFFICIENT_ENTROPY );
        case MBEDTLS_ERR_RSA_UNSUPPORTED_OPERATION:
            return( PSA_ERROR_NOT_SUPPORTED );
        case MBEDTLS_ERR_RSA_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );

        case MBEDTLS_ERR_SHA1_HW_ACCEL_FAILED:
        case MBEDTLS_ERR_SHA256_HW_ACCEL_FAILED:
        case MBEDTLS_ERR_SHA512_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );

        case MBEDTLS_ERR_XTEA_INVALID_INPUT_LENGTH:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_XTEA_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );

        case MBEDTLS_ERR_ECP_BAD_INPUT_DATA:
        case MBEDTLS_ERR_ECP_INVALID_KEY:
            return( PSA_ERROR_INVALID_ARGUMENT );
        case MBEDTLS_ERR_ECP_BUFFER_TOO_SMALL:
            return( PSA_ERROR_BUFFER_TOO_SMALL );
        case MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE:
            return( PSA_ERROR_NOT_SUPPORTED );
        case MBEDTLS_ERR_ECP_SIG_LEN_MISMATCH:
        case MBEDTLS_ERR_ECP_VERIFY_FAILED:
            return( PSA_ERROR_INVALID_SIGNATURE );
        case MBEDTLS_ERR_ECP_ALLOC_FAILED:
            return( PSA_ERROR_INSUFFICIENT_MEMORY );
        case MBEDTLS_ERR_ECP_RANDOM_FAILED:
            return( PSA_ERROR_INSUFFICIENT_ENTROPY );
        case MBEDTLS_ERR_ECP_HW_ACCEL_FAILED:
            return( PSA_ERROR_HARDWARE_FAILURE );

        case MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED:
            return( PSA_ERROR_CORRUPTION_DETECTED );

        default:
            return( PSA_ERROR_GENERIC_ERROR );
    }
}




/****************************************************************/
/* Key management */
/****************************************************************/

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
static inline int psa_key_slot_is_external( const psa_key_slot_t *slot )
{
    return( psa_key_lifetime_is_external( slot->attr.lifetime ) );
}
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */

/* For now the MBEDTLS_PSA_ACCEL_ guards are also used here since the
 * current test driver in key_management.c is using this function
 * when accelerators are used for ECC key pair and public key.
 * Once that dependency is resolved these guards can be removed.
 */
#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR) || \
    defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_PUBLIC_KEY) || \
    defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR) || \
    defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY)
mbedtls_ecp_group_id mbedtls_ecc_group_of_psa( psa_ecc_family_t curve,
                                               size_t bits,
                                               int bits_is_sloppy )
{
    switch( curve )
    {
        case PSA_ECC_FAMILY_SECP_R1:
            switch( bits )
            {
                case 192:
                    return( MBEDTLS_ECP_DP_SECP192R1 );
                case 224:
                    return( MBEDTLS_ECP_DP_SECP224R1 );
                case 256:
                    return( MBEDTLS_ECP_DP_SECP256R1 );
                case 384:
                    return( MBEDTLS_ECP_DP_SECP384R1 );
                case 521:
                    return( MBEDTLS_ECP_DP_SECP521R1 );
                case 528:
                    if( bits_is_sloppy )
                        return( MBEDTLS_ECP_DP_SECP521R1 );
                    break;
            }
            break;

        case PSA_ECC_FAMILY_BRAINPOOL_P_R1:
            switch( bits )
            {
                case 256:
                    return( MBEDTLS_ECP_DP_BP256R1 );
                case 384:
                    return( MBEDTLS_ECP_DP_BP384R1 );
                case 512:
                    return( MBEDTLS_ECP_DP_BP512R1 );
            }
            break;

        case PSA_ECC_FAMILY_MONTGOMERY:
            switch( bits )
            {
                case 255:
                    return( MBEDTLS_ECP_DP_CURVE25519 );
                case 256:
                    if( bits_is_sloppy )
                        return( MBEDTLS_ECP_DP_CURVE25519 );
                    break;
                case 448:
                    return( MBEDTLS_ECP_DP_CURVE448 );
            }
            break;

        case PSA_ECC_FAMILY_SECP_K1:
            switch( bits )
            {
                case 192:
                    return( MBEDTLS_ECP_DP_SECP192K1 );
                case 224:
                    return( MBEDTLS_ECP_DP_SECP224K1 );
                case 256:
                    return( MBEDTLS_ECP_DP_SECP256K1 );
            }
            break;
    }

    return( MBEDTLS_ECP_DP_NONE );
}
#endif /* defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR) ||
        * defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_PUBLIC_KEY) ||
        * defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_ECC_KEY_PAIR) ||
        * defined(MBEDTLS_PSA_ACCEL_KEY_TYPE_ECC_PUBLIC_KEY) */

static psa_status_t validate_unstructured_key_bit_size( psa_key_type_t type,
                                                        size_t bits )
{
    /* Check that the bit size is acceptable for the key type */
    switch( type )
    {
        case PSA_KEY_TYPE_RAW_DATA:
        case PSA_KEY_TYPE_HMAC:
        case PSA_KEY_TYPE_DERIVE:
            break;
#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_AES)
        case PSA_KEY_TYPE_AES:
            if( bits != 128 && bits != 192 && bits != 256 )
                return( PSA_ERROR_INVALID_ARGUMENT );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_CAMELLIA)
        case PSA_KEY_TYPE_CAMELLIA:
            if( bits != 128 && bits != 192 && bits != 256 )
                return( PSA_ERROR_INVALID_ARGUMENT );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_DES)
        case PSA_KEY_TYPE_DES:
            if( bits != 64 && bits != 128 && bits != 192 )
                return( PSA_ERROR_INVALID_ARGUMENT );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ARC4)
        case PSA_KEY_TYPE_ARC4:
            if( bits < 8 || bits > 2048 )
                return( PSA_ERROR_INVALID_ARGUMENT );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_CHACHA20)
        case PSA_KEY_TYPE_CHACHA20:
            if( bits != 256 )
                return( PSA_ERROR_INVALID_ARGUMENT );
            break;
#endif
        default:
            return( PSA_ERROR_NOT_SUPPORTED );
    }
    if( bits % 8 != 0 )
        return( PSA_ERROR_INVALID_ARGUMENT );

    return( PSA_SUCCESS );
}

/** Return the size of the key in the given slot, in bits.
 *
 * \param[in] slot      A key slot.
 *
 * \return The key size in bits, read from the metadata in the slot.
 */
static inline size_t psa_get_key_slot_bits( const psa_key_slot_t *slot )
{
    return( slot->attr.bits );
}

/** Check whether a given key type is valid for use with a given MAC algorithm
 *
 * Upon successful return of this function, the behavior of #PSA_MAC_LENGTH
 * when called with the validated \p algorithm and \p key_type is well-defined.
 *
 * \param[in] algorithm     The specific MAC algorithm (can be wildcard).
 * \param[in] key_type      The key type of the key to be used with the
 *                          \p algorithm.
 *
 * \retval #PSA_SUCCESS
 *         The \p key_type is valid for use with the \p algorithm
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         The \p key_type is not valid for use with the \p algorithm
 */
MBEDTLS_STATIC_TESTABLE psa_status_t psa_mac_key_can_do(
    psa_algorithm_t algorithm,
    psa_key_type_t key_type )
{
    if( PSA_ALG_IS_HMAC( algorithm ) )
    {
        if( key_type == PSA_KEY_TYPE_HMAC )
            return( PSA_SUCCESS );
    }

    if( PSA_ALG_IS_BLOCK_CIPHER_MAC( algorithm ) )
    {
        /* Check that we're calling PSA_BLOCK_CIPHER_BLOCK_LENGTH with a cipher
         * key. */
        if( ( key_type & PSA_KEY_TYPE_CATEGORY_MASK ) ==
            PSA_KEY_TYPE_CATEGORY_SYMMETRIC )
        {
            /* PSA_BLOCK_CIPHER_BLOCK_LENGTH returns 1 for stream ciphers and
             * the block length (larger than 1) for block ciphers. */
            if( PSA_BLOCK_CIPHER_BLOCK_LENGTH( key_type ) > 1 )
                return( PSA_SUCCESS );
        }
    }

    return( PSA_ERROR_INVALID_ARGUMENT );
}

/** Try to allocate a buffer to an empty key slot.
 *
 * \param[in,out] slot          Key slot to attach buffer to.
 * \param[in] buffer_length     Requested size of the buffer.
 *
 * \retval #PSA_SUCCESS
 *         The buffer has been successfully allocated.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY
 *         Not enough memory was available for allocation.
 * \retval #PSA_ERROR_ALREADY_EXISTS
 *         Trying to allocate a buffer to a non-empty key slot.
 */
static psa_status_t psa_allocate_buffer_to_slot( psa_key_slot_t *slot,
                                                 size_t buffer_length )
{
    if( slot->key.data != NULL )
        return( PSA_ERROR_ALREADY_EXISTS );

    slot->key.data = mbedtls_calloc( 1, buffer_length );
    if( slot->key.data == NULL )
        return( PSA_ERROR_INSUFFICIENT_MEMORY );

    slot->key.bytes = buffer_length;
    return( PSA_SUCCESS );
}

psa_status_t psa_copy_key_material_into_slot( psa_key_slot_t *slot,
                                              const uint8_t* data,
                                              size_t data_length )
{
    psa_status_t status = psa_allocate_buffer_to_slot( slot,
                                                       data_length );
    if( status != PSA_SUCCESS )
        return( status );

    memcpy( slot->key.data, data, data_length );
    return( PSA_SUCCESS );
}

psa_status_t psa_import_key_into_slot(
    const psa_key_attributes_t *attributes,
    const uint8_t *data, size_t data_length,
    uint8_t *key_buffer, size_t key_buffer_size,
    size_t *key_buffer_length, size_t *bits )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_type_t type = attributes->core.type;

    /* zero-length keys are never supported. */
    if( data_length == 0 )
        return( PSA_ERROR_NOT_SUPPORTED );

    if( key_type_is_raw_bytes( type ) )
    {
        *bits = PSA_BYTES_TO_BITS( data_length );

        /* Ensure that the bytes-to-bits conversion hasn't overflown. */
        if( data_length > SIZE_MAX / 8 )
            return( PSA_ERROR_NOT_SUPPORTED );

        /* Enforce a size limit, and in particular ensure that the bit
         * size fits in its representation type. */
        if( ( *bits ) > PSA_MAX_KEY_BITS )
            return( PSA_ERROR_NOT_SUPPORTED );

        status = validate_unstructured_key_bit_size( type, *bits );
        if( status != PSA_SUCCESS )
            return( status );

        /* Copy the key material. */
        memcpy( key_buffer, data, data_length );
        *key_buffer_length = data_length;
        (void)key_buffer_size;

        return( PSA_SUCCESS );
    }
    else if( PSA_KEY_TYPE_IS_ASYMMETRIC( type ) )
    {
#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR) || \
    defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_PUBLIC_KEY)
        if( PSA_KEY_TYPE_IS_ECC( type ) )
        {
            return( mbedtls_psa_ecp_import_key( attributes,
                                                data, data_length,
                                                key_buffer, key_buffer_size,
                                                key_buffer_length,
                                                bits ) );
        }
#endif /* defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR) ||
        * defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_PUBLIC_KEY) */
#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR) || \
    defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY)
        if( PSA_KEY_TYPE_IS_RSA( type ) )
        {
            return( mbedtls_psa_rsa_import_key( attributes,
                                                data, data_length,
                                                key_buffer, key_buffer_size,
                                                key_buffer_length,
                                                bits ) );
        }
#endif /* defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR) ||
        * defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY) */
    }

    return( PSA_ERROR_NOT_SUPPORTED );
}

/** Calculate the intersection of two algorithm usage policies.
 *
 * Return 0 (which allows no operation) on incompatibility.
 */
static psa_algorithm_t psa_key_policy_algorithm_intersection(
    psa_key_type_t key_type,
    psa_algorithm_t alg1,
    psa_algorithm_t alg2 )
{
    /* Common case: both sides actually specify the same policy. */
    if( alg1 == alg2 )
        return( alg1 );
    /* If the policies are from the same hash-and-sign family, check
     * if one is a wildcard. If so the other has the specific algorithm. */
    if( PSA_ALG_IS_HASH_AND_SIGN( alg1 ) &&
        PSA_ALG_IS_HASH_AND_SIGN( alg2 ) &&
        ( alg1 & ~PSA_ALG_HASH_MASK ) == ( alg2 & ~PSA_ALG_HASH_MASK ) )
    {
        if( PSA_ALG_SIGN_GET_HASH( alg1 ) == PSA_ALG_ANY_HASH )
            return( alg2 );
        if( PSA_ALG_SIGN_GET_HASH( alg2 ) == PSA_ALG_ANY_HASH )
            return( alg1 );
    }
    /* If the policies are from the same AEAD family, check whether
     * one of them is a minimum-tag-length wildcard. Calculate the most
     * restrictive tag length. */
    if( PSA_ALG_IS_AEAD( alg1 ) && PSA_ALG_IS_AEAD( alg2 ) &&
        ( PSA_ALG_AEAD_WITH_SHORTENED_TAG( alg1, 0 ) ==
          PSA_ALG_AEAD_WITH_SHORTENED_TAG( alg2, 0 ) ) )
    {
        size_t alg1_len = PSA_ALG_AEAD_GET_TAG_LENGTH( alg1 );
        size_t alg2_len = PSA_ALG_AEAD_GET_TAG_LENGTH( alg2 );
        size_t restricted_len = alg1_len > alg2_len ? alg1_len : alg2_len;

        /* If both are wildcards, return most restrictive wildcard */
        if( ( ( alg1 & PSA_ALG_AEAD_AT_LEAST_THIS_LENGTH_FLAG ) != 0 ) &&
            ( ( alg2 & PSA_ALG_AEAD_AT_LEAST_THIS_LENGTH_FLAG ) != 0 ) )
        {
            return( PSA_ALG_AEAD_WITH_AT_LEAST_THIS_LENGTH_TAG(
                        alg1, restricted_len ) );
        }
        /* If only one is a wildcard, return specific algorithm if compatible. */
        if( ( ( alg1 & PSA_ALG_AEAD_AT_LEAST_THIS_LENGTH_FLAG ) != 0 ) &&
            ( alg1_len <= alg2_len ) )
        {
            return( alg2 );
        }
        if( ( ( alg2 & PSA_ALG_AEAD_AT_LEAST_THIS_LENGTH_FLAG ) != 0 ) &&
            ( alg2_len <= alg1_len ) )
        {
            return( alg1 );
        }
    }
    /* If the policies are from the same MAC family, check whether one
     * of them is a minimum-MAC-length policy. Calculate the most
     * restrictive tag length. */
    if( PSA_ALG_IS_MAC( alg1 ) && PSA_ALG_IS_MAC( alg2 ) &&
        ( PSA_ALG_FULL_LENGTH_MAC( alg1 ) ==
          PSA_ALG_FULL_LENGTH_MAC( alg2 ) ) )
    {
        /* Validate the combination of key type and algorithm. Since the base
         * algorithm of alg1 and alg2 are the same, we only need this once. */
        if( PSA_SUCCESS != psa_mac_key_can_do( alg1, key_type ) )
            return( 0 );

        /* Get the (exact or at-least) output lengths for both sides of the
         * requested intersection. None of the currently supported algorithms
         * have an output length dependent on the actual key size, so setting it
         * to a bogus value of 0 is currently OK.
         *
         * Note that for at-least-this-length wildcard algorithms, the output
         * length is set to the shortest allowed length, which allows us to
         * calculate the most restrictive tag length for the intersection. */
        size_t alg1_len = PSA_MAC_LENGTH( key_type, 0, alg1 );
        size_t alg2_len = PSA_MAC_LENGTH( key_type, 0, alg2 );
        size_t restricted_len = alg1_len > alg2_len ? alg1_len : alg2_len;

        /* If both are wildcards, return most restrictive wildcard */
        if( ( ( alg1 & PSA_ALG_MAC_AT_LEAST_THIS_LENGTH_FLAG ) != 0 ) &&
            ( ( alg2 & PSA_ALG_MAC_AT_LEAST_THIS_LENGTH_FLAG ) != 0 ) )
        {
            return( PSA_ALG_AT_LEAST_THIS_LENGTH_MAC( alg1, restricted_len ) );
        }

        /* If only one is an at-least-this-length policy, the intersection would
         * be the other (fixed-length) policy as long as said fixed length is
         * equal to or larger than the shortest allowed length. */
        if( ( alg1 & PSA_ALG_MAC_AT_LEAST_THIS_LENGTH_FLAG ) != 0 )
        {
            return( ( alg1_len <= alg2_len ) ? alg2 : 0 );
        }
        if( ( alg2 & PSA_ALG_MAC_AT_LEAST_THIS_LENGTH_FLAG ) != 0 )
        {
            return( ( alg2_len <= alg1_len ) ? alg1 : 0 );
        }

        /* If none of them are wildcards, check whether they define the same tag
         * length. This is still possible here when one is default-length and
         * the other specific-length. Ensure to always return the
         * specific-length version for the intersection. */
        if( alg1_len == alg2_len )
            return( PSA_ALG_TRUNCATED_MAC( alg1, alg1_len ) );
    }
    /* If the policies are incompatible, allow nothing. */
    return( 0 );
}

static int psa_key_algorithm_permits( psa_key_type_t key_type,
                                      psa_algorithm_t policy_alg,
                                      psa_algorithm_t requested_alg )
{
    /* Common case: the policy only allows requested_alg. */
    if( requested_alg == policy_alg )
        return( 1 );
    /* If policy_alg is a hash-and-sign with a wildcard for the hash,
     * and requested_alg is the same hash-and-sign family with any hash,
     * then requested_alg is compliant with policy_alg. */
    if( PSA_ALG_IS_HASH_AND_SIGN( requested_alg ) &&
        PSA_ALG_SIGN_GET_HASH( policy_alg ) == PSA_ALG_ANY_HASH )
    {
        return( ( policy_alg & ~PSA_ALG_HASH_MASK ) ==
                ( requested_alg & ~PSA_ALG_HASH_MASK ) );
    }
    /* If policy_alg is a wildcard AEAD algorithm of the same base as
     * the requested algorithm, check the requested tag length to be
     * equal-length or longer than the wildcard-specified length. */
    if( PSA_ALG_IS_AEAD( policy_alg ) &&
        PSA_ALG_IS_AEAD( requested_alg ) &&
        ( PSA_ALG_AEAD_WITH_SHORTENED_TAG( policy_alg, 0 ) ==
          PSA_ALG_AEAD_WITH_SHORTENED_TAG( requested_alg, 0 ) ) &&
        ( ( policy_alg & PSA_ALG_AEAD_AT_LEAST_THIS_LENGTH_FLAG ) != 0 ) )
    {
        return( PSA_ALG_AEAD_GET_TAG_LENGTH( policy_alg ) <=
                PSA_ALG_AEAD_GET_TAG_LENGTH( requested_alg ) );
    }
    /* If policy_alg is a MAC algorithm of the same base as the requested
     * algorithm, check whether their MAC lengths are compatible. */
    if( PSA_ALG_IS_MAC( policy_alg ) &&
        PSA_ALG_IS_MAC( requested_alg ) &&
        ( PSA_ALG_FULL_LENGTH_MAC( policy_alg ) ==
          PSA_ALG_FULL_LENGTH_MAC( requested_alg ) ) )
    {
        /* Validate the combination of key type and algorithm. Since the policy
         * and requested algorithms are the same, we only need this once. */
        if( PSA_SUCCESS != psa_mac_key_can_do( policy_alg, key_type ) )
            return( 0 );

        /* Get both the requested output length for the algorithm which is to be
         * verified, and the default output length for the base algorithm.
         * Note that none of the currently supported algorithms have an output
         * length dependent on actual key size, so setting it to a bogus value
         * of 0 is currently OK. */
        size_t requested_output_length = PSA_MAC_LENGTH(
                                            key_type, 0, requested_alg );
        size_t default_output_length = PSA_MAC_LENGTH(
                                        key_type, 0,
                                        PSA_ALG_FULL_LENGTH_MAC( requested_alg ) );

        /* If the policy is default-length, only allow an algorithm with
         * a declared exact-length matching the default. */
        if( PSA_MAC_TRUNCATED_LENGTH( policy_alg ) == 0 )
            return( requested_output_length == default_output_length );

        /* If the requested algorithm is default-length, allow it if the policy
         * length exactly matches the default length. */
        if( PSA_MAC_TRUNCATED_LENGTH( requested_alg ) == 0 &&
            PSA_MAC_TRUNCATED_LENGTH( policy_alg ) == default_output_length )
        {
            return( 1 );
        }

        /* If policy_alg is an at-least-this-length wildcard MAC algorithm,
         * check for the requested MAC length to be equal to or longer than the
         * minimum allowed length. */
        if( ( policy_alg & PSA_ALG_MAC_AT_LEAST_THIS_LENGTH_FLAG ) != 0 )
        {
            return( PSA_MAC_TRUNCATED_LENGTH( policy_alg ) <=
                    requested_output_length );
        }
    }
    /* If policy_alg is a generic key agreement operation, then using it for
     * a key derivation with that key agreement should also be allowed. This
     * behaviour is expected to be defined in a future specification version. */
    if( PSA_ALG_IS_RAW_KEY_AGREEMENT( policy_alg ) &&
        PSA_ALG_IS_KEY_AGREEMENT( requested_alg ) )
    {
        return( PSA_ALG_KEY_AGREEMENT_GET_BASE( requested_alg ) ==
                policy_alg );
    }
    /* If it isn't explicitly permitted, it's forbidden. */
    return( 0 );
}

/** Test whether a policy permits an algorithm.
 *
 * The caller must test usage flags separately.
 *
 * \note This function requires providing the key type for which the policy is
 *       being validated, since some algorithm policy definitions (e.g. MAC)
 *       have different properties depending on what kind of cipher it is
 *       combined with.
 *
 * \retval PSA_SUCCESS                  When \p alg is a specific algorithm
 *                                      allowed by the \p policy.
 * \retval PSA_ERROR_INVALID_ARGUMENT   When \p alg is not a specific algorithm
 * \retval PSA_ERROR_NOT_PERMITTED      When \p alg is a specific algorithm, but
 *                                      the \p policy does not allow it.
 */
static psa_status_t psa_key_policy_permits( const psa_key_policy_t *policy,
                                            psa_key_type_t key_type,
                                            psa_algorithm_t alg )
{
    /* '0' is not a valid algorithm */
    if( alg == 0 )
        return( PSA_ERROR_INVALID_ARGUMENT );

    /* A requested algorithm cannot be a wildcard. */
    if( PSA_ALG_IS_WILDCARD( alg ) )
        return( PSA_ERROR_INVALID_ARGUMENT );

    if( psa_key_algorithm_permits( key_type, policy->alg, alg ) ||
        psa_key_algorithm_permits( key_type, policy->alg2, alg ) )
        return( PSA_SUCCESS );
    else
        return( PSA_ERROR_NOT_PERMITTED );
}

/** Restrict a key policy based on a constraint.
 *
 * \note This function requires providing the key type for which the policy is
 *       being restricted, since some algorithm policy definitions (e.g. MAC)
 *       have different properties depending on what kind of cipher it is
 *       combined with.
 *
 * \param[in] key_type      The key type for which to restrict the policy
 * \param[in,out] policy    The policy to restrict.
 * \param[in] constraint    The policy constraint to apply.
 *
 * \retval #PSA_SUCCESS
 *         \c *policy contains the intersection of the original value of
 *         \c *policy and \c *constraint.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         \c key_type, \c *policy and \c *constraint are incompatible.
 *         \c *policy is unchanged.
 */
static psa_status_t psa_restrict_key_policy(
    psa_key_type_t key_type,
    psa_key_policy_t *policy,
    const psa_key_policy_t *constraint )
{
    psa_algorithm_t intersection_alg =
        psa_key_policy_algorithm_intersection( key_type, policy->alg,
                                               constraint->alg );
    psa_algorithm_t intersection_alg2 =
        psa_key_policy_algorithm_intersection( key_type, policy->alg2,
                                               constraint->alg2 );
    if( intersection_alg == 0 && policy->alg != 0 && constraint->alg != 0 )
        return( PSA_ERROR_INVALID_ARGUMENT );
    if( intersection_alg2 == 0 && policy->alg2 != 0 && constraint->alg2 != 0 )
        return( PSA_ERROR_INVALID_ARGUMENT );
    policy->usage &= constraint->usage;
    policy->alg = intersection_alg;
    policy->alg2 = intersection_alg2;
    return( PSA_SUCCESS );
}

/** Get the description of a key given its identifier and policy constraints
 *  and lock it.
 *
 * The key must have allow all the usage flags set in \p usage. If \p alg is
 * nonzero, the key must allow operations with this algorithm. If \p alg is
 * zero, the algorithm is not checked.
 *
 * In case of a persistent key, the function loads the description of the key
 * into a key slot if not already done.
 *
 * On success, the returned key slot is locked. It is the responsibility of
 * the caller to unlock the key slot when it does not access it anymore.
 */
static psa_status_t psa_get_and_lock_key_slot_with_policy(
    mbedtls_svc_key_id_t key,
    psa_key_slot_t **p_slot,
    psa_key_usage_t usage,
    psa_algorithm_t alg )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;

    status = psa_get_and_lock_key_slot( key, p_slot );
    if( status != PSA_SUCCESS )
        return( status );
    slot = *p_slot;

    /* Enforce that usage policy for the key slot contains all the flags
     * required by the usage parameter. There is one exception: public
     * keys can always be exported, so we treat public key objects as
     * if they had the export flag. */
    if( PSA_KEY_TYPE_IS_PUBLIC_KEY( slot->attr.type ) )
        usage &= ~PSA_KEY_USAGE_EXPORT;

    if( ( slot->attr.policy.usage & usage ) != usage )
    {
        status = PSA_ERROR_NOT_PERMITTED;
        goto error;
    }

    /* Enforce that the usage policy permits the requested algortihm. */
    if( alg != 0 )
    {
        status = psa_key_policy_permits( &slot->attr.policy,
                                         slot->attr.type,
                                         alg );
        if( status != PSA_SUCCESS )
            goto error;
    }

    return( PSA_SUCCESS );

error:
    *p_slot = NULL;
    psa_unlock_key_slot( slot );

    return( status );
}

/** Get a key slot containing a transparent key and lock it.
 *
 * A transparent key is a key for which the key material is directly
 * available, as opposed to a key in a secure element.
 *
 * This is a temporary function to use instead of
 * psa_get_and_lock_key_slot_with_policy() until secure element support is
 * fully implemented.
 *
 * On success, the returned key slot is locked. It is the responsibility of the
 * caller to unlock the key slot when it does not access it anymore.
 */
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
static psa_status_t psa_get_and_lock_transparent_key_slot_with_policy(
    mbedtls_svc_key_id_t key,
    psa_key_slot_t **p_slot,
    psa_key_usage_t usage,
    psa_algorithm_t alg )
{
    psa_status_t status = psa_get_and_lock_key_slot_with_policy( key, p_slot,
                                                                 usage, alg );
    if( status != PSA_SUCCESS )
        return( status );

    if( psa_key_slot_is_external( *p_slot ) )
    {
        psa_unlock_key_slot( *p_slot );
        *p_slot = NULL;
        return( PSA_ERROR_NOT_SUPPORTED );
    }

    return( PSA_SUCCESS );
}
#else /* MBEDTLS_PSA_CRYPTO_SE_C */
/* With no secure element support, all keys are transparent. */
#define psa_get_and_lock_transparent_key_slot_with_policy( key, p_slot, usage, alg )   \
    psa_get_and_lock_key_slot_with_policy( key, p_slot, usage, alg )
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */

/** Wipe key data from a slot. Preserve metadata such as the policy. */
static psa_status_t psa_remove_key_data_from_memory( psa_key_slot_t *slot )
{
    /* Data pointer will always be either a valid pointer or NULL in an
     * initialized slot, so we can just free it. */
    if( slot->key.data != NULL )
        mbedtls_platform_zeroize( slot->key.data, slot->key.bytes);

    mbedtls_free( slot->key.data );
    slot->key.data = NULL;
    slot->key.bytes = 0;

    return( PSA_SUCCESS );
}

/** Completely wipe a slot in memory, including its policy.
 * Persistent storage is not affected. */
psa_status_t psa_wipe_key_slot( psa_key_slot_t *slot )
{
    psa_status_t status = psa_remove_key_data_from_memory( slot );

    /*
     * As the return error code may not be handled in case of multiple errors,
     * do our best to report an unexpected lock counter: if available
     * call MBEDTLS_PARAM_FAILED that may terminate execution (if called as
     * part of the execution of a test suite this will stop the test suite
     * execution).
     */
    if( slot->lock_count != 1 )
    {
#ifdef MBEDTLS_CHECK_PARAMS
        MBEDTLS_PARAM_FAILED( slot->lock_count == 1 );
#endif
        status = PSA_ERROR_CORRUPTION_DETECTED;
    }

    /* Multipart operations may still be using the key. This is safe
     * because all multipart operation objects are independent from
     * the key slot: if they need to access the key after the setup
     * phase, they have a copy of the key. Note that this means that
     * key material can linger until all operations are completed. */
    /* At this point, key material and other type-specific content has
     * been wiped. Clear remaining metadata. We can call memset and not
     * zeroize because the metadata is not particularly sensitive. */
    memset( slot, 0, sizeof( *slot ) );
    return( status );
}

psa_status_t psa_destroy_key( mbedtls_svc_key_id_t key )
{
    psa_key_slot_t *slot;
    psa_status_t status; /* status of the last operation */
    psa_status_t overall_status = PSA_SUCCESS;
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    psa_se_drv_table_entry_t *driver;
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */

    if( mbedtls_svc_key_id_is_null( key ) )
        return( PSA_SUCCESS );

    /*
     * Get the description of the key in a key slot. In case of a persistent
     * key, this will load the key description from persistent memory if not
     * done yet. We cannot avoid this loading as without it we don't know if
     * the key is operated by an SE or not and this information is needed by
     * the current implementation.
     */
    status = psa_get_and_lock_key_slot( key, &slot );
    if( status != PSA_SUCCESS )
        return( status );

    /*
     * If the key slot containing the key description is under access by the
     * library (apart from the present access), the key cannot be destroyed
     * yet. For the time being, just return in error. Eventually (to be
     * implemented), the key should be destroyed when all accesses have
     * stopped.
     */
    if( slot->lock_count > 1 )
    {
       psa_unlock_key_slot( slot );
       return( PSA_ERROR_GENERIC_ERROR );
    }

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    driver = psa_get_se_driver_entry( slot->attr.lifetime );
    if( driver != NULL )
    {
        /* For a key in a secure element, we need to do three things:
         * remove the key file in internal storage, destroy the
         * key inside the secure element, and update the driver's
         * persistent data. Start a transaction that will encompass these
         * three actions. */
        psa_crypto_prepare_transaction( PSA_CRYPTO_TRANSACTION_DESTROY_KEY );
        psa_crypto_transaction.key.lifetime = slot->attr.lifetime;
        psa_crypto_transaction.key.slot = psa_key_slot_get_slot_number( slot );
        psa_crypto_transaction.key.id = slot->attr.id;
        status = psa_crypto_save_transaction( );
        if( status != PSA_SUCCESS )
        {
            (void) psa_crypto_stop_transaction( );
            /* We should still try to destroy the key in the secure
             * element and the key metadata in storage. This is especially
             * important if the error is that the storage is full.
             * But how to do it exactly without risking an inconsistent
             * state after a reset?
             * https://github.com/ARMmbed/mbed-crypto/issues/215
             */
            overall_status = status;
            goto exit;
        }

        status = psa_destroy_se_key( driver,
                                     psa_key_slot_get_slot_number( slot ) );
        if( overall_status == PSA_SUCCESS )
            overall_status = status;
    }
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */

#if defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
    if( ! PSA_KEY_LIFETIME_IS_VOLATILE( slot->attr.lifetime ) )
    {
        status = psa_destroy_persistent_key( slot->attr.id );
        if( overall_status == PSA_SUCCESS )
            overall_status = status;

        /* TODO: other slots may have a copy of the same key. We should
         * invalidate them.
         * https://github.com/ARMmbed/mbed-crypto/issues/214
         */
    }
#endif /* defined(MBEDTLS_PSA_CRYPTO_STORAGE_C) */

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    if( driver != NULL )
    {
        status = psa_save_se_persistent_data( driver );
        if( overall_status == PSA_SUCCESS )
            overall_status = status;
        status = psa_crypto_stop_transaction( );
        if( overall_status == PSA_SUCCESS )
            overall_status = status;
    }
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
exit:
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */
    status = psa_wipe_key_slot( slot );
    /* Prioritize CORRUPTION_DETECTED from wiping over a storage error */
    if( overall_status == PSA_SUCCESS )
        overall_status = status;
    return( overall_status );
}

#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR) || \
    defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY)
static psa_status_t psa_get_rsa_public_exponent(
    const mbedtls_rsa_context *rsa,
    psa_key_attributes_t *attributes )
{
    mbedtls_mpi mpi;
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    uint8_t *buffer = NULL;
    size_t buflen;
    mbedtls_mpi_init( &mpi );

    ret = mbedtls_rsa_export( rsa, NULL, NULL, NULL, NULL, &mpi );
    if( ret != 0 )
        goto exit;
    if( mbedtls_mpi_cmp_int( &mpi, 65537 ) == 0 )
    {
        /* It's the default value, which is reported as an empty string,
         * so there's nothing to do. */
        goto exit;
    }

    buflen = mbedtls_mpi_size( &mpi );
    buffer = mbedtls_calloc( 1, buflen );
    if( buffer == NULL )
    {
        ret = MBEDTLS_ERR_MPI_ALLOC_FAILED;
        goto exit;
    }
    ret = mbedtls_mpi_write_binary( &mpi, buffer, buflen );
    if( ret != 0 )
        goto exit;
    attributes->domain_parameters = buffer;
    attributes->domain_parameters_size = buflen;

exit:
    mbedtls_mpi_free( &mpi );
    if( ret != 0 )
        mbedtls_free( buffer );
    return( mbedtls_to_psa_error( ret ) );
}
#endif /* defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR) ||
        * defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY) */

/** Retrieve all the publicly-accessible attributes of a key.
 */
psa_status_t psa_get_key_attributes( mbedtls_svc_key_id_t key,
                                     psa_key_attributes_t *attributes )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;

    psa_reset_key_attributes( attributes );

    status = psa_get_and_lock_key_slot_with_policy( key, &slot, 0, 0 );
    if( status != PSA_SUCCESS )
        return( status );

    attributes->core = slot->attr;
    attributes->core.flags &= ( MBEDTLS_PSA_KA_MASK_EXTERNAL_ONLY |
                                MBEDTLS_PSA_KA_MASK_DUAL_USE );

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    if( psa_key_slot_is_external( slot ) )
        psa_set_key_slot_number( attributes,
                                 psa_key_slot_get_slot_number( slot ) );
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */

    switch( slot->attr.type )
    {
#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR) || \
    defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY)
        case PSA_KEY_TYPE_RSA_KEY_PAIR:
        case PSA_KEY_TYPE_RSA_PUBLIC_KEY:
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
            /* TODO: reporting the public exponent for opaque keys
             * is not yet implemented.
             * https://github.com/ARMmbed/mbed-crypto/issues/216
             */
            if( psa_key_slot_is_external( slot ) )
                break;
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */
            {
                mbedtls_rsa_context *rsa = NULL;

                status = mbedtls_psa_rsa_load_representation(
                             slot->attr.type,
                             slot->key.data,
                             slot->key.bytes,
                             &rsa );
                if( status != PSA_SUCCESS )
                    break;

                status = psa_get_rsa_public_exponent( rsa,
                                                      attributes );
                mbedtls_rsa_free( rsa );
                mbedtls_free( rsa );
            }
            break;
#endif /* defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR) ||
        * defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY) */
        default:
            /* Nothing else to do. */
            break;
    }

    if( status != PSA_SUCCESS )
        psa_reset_key_attributes( attributes );

    unlock_status = psa_unlock_key_slot( slot );

    return( ( status == PSA_SUCCESS ) ? unlock_status : status );
}

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
psa_status_t psa_get_key_slot_number(
    const psa_key_attributes_t *attributes,
    psa_key_slot_number_t *slot_number )
{
    if( attributes->core.flags & MBEDTLS_PSA_KA_FLAG_HAS_SLOT_NUMBER )
    {
        *slot_number = attributes->slot_number;
        return( PSA_SUCCESS );
    }
    else
        return( PSA_ERROR_INVALID_ARGUMENT );
}
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */

static psa_status_t psa_export_key_buffer_internal( const uint8_t *key_buffer,
                                                    size_t key_buffer_size,
                                                    uint8_t *data,
                                                    size_t data_size,
                                                    size_t *data_length )
{
    if( key_buffer_size > data_size )
        return( PSA_ERROR_BUFFER_TOO_SMALL );
    memcpy( data, key_buffer, key_buffer_size );
    memset( data + key_buffer_size, 0,
            data_size - key_buffer_size );
    *data_length = key_buffer_size;
    return( PSA_SUCCESS );
}

psa_status_t psa_export_key_internal(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    uint8_t *data, size_t data_size, size_t *data_length )
{
    psa_key_type_t type = attributes->core.type;

    if( key_type_is_raw_bytes( type ) ||
        PSA_KEY_TYPE_IS_RSA( type )   ||
        PSA_KEY_TYPE_IS_ECC( type )      )
    {
        return( psa_export_key_buffer_internal(
                    key_buffer, key_buffer_size,
                    data, data_size, data_length ) );
    }
    else
    {
        /* This shouldn't happen in the reference implementation, but
           it is valid for a special-purpose implementation to omit
           support for exporting certain key types. */
        return( PSA_ERROR_NOT_SUPPORTED );
    }
}

psa_status_t psa_export_key( mbedtls_svc_key_id_t key,
                             uint8_t *data,
                             size_t data_size,
                             size_t *data_length )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;

    /* Reject a zero-length output buffer now, since this can never be a
     * valid key representation. This way we know that data must be a valid
     * pointer and we can do things like memset(data, ..., data_size). */
    if( data_size == 0 )
        return( PSA_ERROR_BUFFER_TOO_SMALL );

    /* Set the key to empty now, so that even when there are errors, we always
     * set data_length to a value between 0 and data_size. On error, setting
     * the key to empty is a good choice because an empty key representation is
     * unlikely to be accepted anywhere. */
    *data_length = 0;

    /* Export requires the EXPORT flag. There is an exception for public keys,
     * which don't require any flag, but
     * psa_get_and_lock_key_slot_with_policy() takes care of this.
     */
    status = psa_get_and_lock_key_slot_with_policy( key, &slot,
                                                    PSA_KEY_USAGE_EXPORT, 0 );
    if( status != PSA_SUCCESS )
        return( status );

    psa_key_attributes_t attributes = {
        .core = slot->attr
    };
    status = psa_driver_wrapper_export_key( &attributes,
                 slot->key.data, slot->key.bytes,
                 data, data_size, data_length );

    unlock_status = psa_unlock_key_slot( slot );

    return( ( status == PSA_SUCCESS ) ? unlock_status : status );
}

psa_status_t psa_export_public_key_internal(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer,
    size_t key_buffer_size,
    uint8_t *data,
    size_t data_size,
    size_t *data_length )
{
    psa_key_type_t type = attributes->core.type;

    if( PSA_KEY_TYPE_IS_RSA( type ) || PSA_KEY_TYPE_IS_ECC( type ) )
    {
        if( PSA_KEY_TYPE_IS_PUBLIC_KEY( type ) )
        {
            /* Exporting public -> public */
            return( psa_export_key_buffer_internal(
                        key_buffer, key_buffer_size,
                        data, data_size, data_length ) );
        }

        if( PSA_KEY_TYPE_IS_RSA( type ) )
        {
#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR) || \
    defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY)
            return( mbedtls_psa_rsa_export_public_key( attributes,
                                                       key_buffer,
                                                       key_buffer_size,
                                                       data,
                                                       data_size,
                                                       data_length ) );
#else
            /* We don't know how to convert a private RSA key to public. */
            return( PSA_ERROR_NOT_SUPPORTED );
#endif /* defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR) ||
        * defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY) */
        }
        else
        {
#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR) || \
    defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_PUBLIC_KEY)
            return( mbedtls_psa_ecp_export_public_key( attributes,
                                                       key_buffer,
                                                       key_buffer_size,
                                                       data,
                                                       data_size,
                                                       data_length ) );
#else
            /* We don't know how to convert a private ECC key to public */
            return( PSA_ERROR_NOT_SUPPORTED );
#endif /* defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR) ||
        * defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_PUBLIC_KEY) */
        }
    }
    else
    {
        /* This shouldn't happen in the reference implementation, but
           it is valid for a special-purpose implementation to omit
           support for exporting certain key types. */
        return( PSA_ERROR_NOT_SUPPORTED );
    }
}

psa_status_t psa_export_public_key( mbedtls_svc_key_id_t key,
                                    uint8_t *data,
                                    size_t data_size,
                                    size_t *data_length )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;

    /* Reject a zero-length output buffer now, since this can never be a
     * valid key representation. This way we know that data must be a valid
     * pointer and we can do things like memset(data, ..., data_size). */
    if( data_size == 0 )
        return( PSA_ERROR_BUFFER_TOO_SMALL );

    /* Set the key to empty now, so that even when there are errors, we always
     * set data_length to a value between 0 and data_size. On error, setting
     * the key to empty is a good choice because an empty key representation is
     * unlikely to be accepted anywhere. */
    *data_length = 0;

    /* Exporting a public key doesn't require a usage flag. */
    status = psa_get_and_lock_key_slot_with_policy( key, &slot, 0, 0 );
    if( status != PSA_SUCCESS )
        return( status );

    if( ! PSA_KEY_TYPE_IS_ASYMMETRIC( slot->attr.type ) )
    {
         status = PSA_ERROR_INVALID_ARGUMENT;
         goto exit;
    }

    psa_key_attributes_t attributes = {
        .core = slot->attr
    };
    status = psa_driver_wrapper_export_public_key(
        &attributes, slot->key.data, slot->key.bytes,
        data, data_size, data_length );

exit:
    unlock_status = psa_unlock_key_slot( slot );

    return( ( status == PSA_SUCCESS ) ? unlock_status : status );
}

#if defined(static_assert)
static_assert( ( MBEDTLS_PSA_KA_MASK_EXTERNAL_ONLY & MBEDTLS_PSA_KA_MASK_DUAL_USE ) == 0,
               "One or more key attribute flag is listed as both external-only and dual-use" );
static_assert( ( PSA_KA_MASK_INTERNAL_ONLY & MBEDTLS_PSA_KA_MASK_DUAL_USE ) == 0,
               "One or more key attribute flag is listed as both internal-only and dual-use" );
static_assert( ( PSA_KA_MASK_INTERNAL_ONLY & MBEDTLS_PSA_KA_MASK_EXTERNAL_ONLY ) == 0,
               "One or more key attribute flag is listed as both internal-only and external-only" );
#endif

/** Validate that a key policy is internally well-formed.
 *
 * This function only rejects invalid policies. It does not validate the
 * consistency of the policy with respect to other attributes of the key
 * such as the key type.
 */
static psa_status_t psa_validate_key_policy( const psa_key_policy_t *policy )
{
    if( ( policy->usage & ~( PSA_KEY_USAGE_EXPORT |
                             PSA_KEY_USAGE_COPY |
                             PSA_KEY_USAGE_ENCRYPT |
                             PSA_KEY_USAGE_DECRYPT |
                             PSA_KEY_USAGE_SIGN_HASH |
                             PSA_KEY_USAGE_VERIFY_HASH |
                             PSA_KEY_USAGE_DERIVE ) ) != 0 )
        return( PSA_ERROR_INVALID_ARGUMENT );

    return( PSA_SUCCESS );
}

/** Validate the internal consistency of key attributes.
 *
 * This function only rejects invalid attribute values. If does not
 * validate the consistency of the attributes with any key data that may
 * be involved in the creation of the key.
 *
 * Call this function early in the key creation process.
 *
 * \param[in] attributes    Key attributes for the new key.
 * \param[out] p_drv        On any return, the driver for the key, if any.
 *                          NULL for a transparent key.
 *
 */
static psa_status_t psa_validate_key_attributes(
    const psa_key_attributes_t *attributes,
    psa_se_drv_table_entry_t **p_drv )
{
    psa_status_t status = PSA_ERROR_INVALID_ARGUMENT;
    psa_key_lifetime_t lifetime = psa_get_key_lifetime( attributes );
    mbedtls_svc_key_id_t key = psa_get_key_id( attributes );

    status = psa_validate_key_location( lifetime, p_drv );
    if( status != PSA_SUCCESS )
        return( status );

    status = psa_validate_key_persistence( lifetime );
    if( status != PSA_SUCCESS )
        return( status );

    if ( PSA_KEY_LIFETIME_IS_VOLATILE( lifetime ) )
    {
        if( MBEDTLS_SVC_KEY_ID_GET_KEY_ID( key ) != 0 )
            return( PSA_ERROR_INVALID_ARGUMENT );
    }
    else
    {
        status = psa_validate_key_id( psa_get_key_id( attributes ), 0 );
        if( status != PSA_SUCCESS )
            return( status );
    }

    status = psa_validate_key_policy( &attributes->core.policy );
    if( status != PSA_SUCCESS )
        return( status );

    /* Refuse to create overly large keys.
     * Note that this doesn't trigger on import if the attributes don't
     * explicitly specify a size (so psa_get_key_bits returns 0), so
     * psa_import_key() needs its own checks. */
    if( psa_get_key_bits( attributes ) > PSA_MAX_KEY_BITS )
        return( PSA_ERROR_NOT_SUPPORTED );

    /* Reject invalid flags. These should not be reachable through the API. */
    if( attributes->core.flags & ~ ( MBEDTLS_PSA_KA_MASK_EXTERNAL_ONLY |
                                     MBEDTLS_PSA_KA_MASK_DUAL_USE ) )
        return( PSA_ERROR_INVALID_ARGUMENT );

    return( PSA_SUCCESS );
}

/** Prepare a key slot to receive key material.
 *
 * This function allocates a key slot and sets its metadata.
 *
 * If this function fails, call psa_fail_key_creation().
 *
 * This function is intended to be used as follows:
 * -# Call psa_start_key_creation() to allocate a key slot, prepare
 *    it with the specified attributes, and in case of a volatile key assign it
 *    a volatile key identifier.
 * -# Populate the slot with the key material.
 * -# Call psa_finish_key_creation() to finalize the creation of the slot.
 * In case of failure at any step, stop the sequence and call
 * psa_fail_key_creation().
 *
 * On success, the key slot is locked. It is the responsibility of the caller
 * to unlock the key slot when it does not access it anymore.
 *
 * \param method            An identification of the calling function.
 * \param[in] attributes    Key attributes for the new key.
 * \param[out] p_slot       On success, a pointer to the prepared slot.
 * \param[out] p_drv        On any return, the driver for the key, if any.
 *                          NULL for a transparent key.
 *
 * \retval #PSA_SUCCESS
 *         The key slot is ready to receive key material.
 * \return If this function fails, the key slot is an invalid state.
 *         You must call psa_fail_key_creation() to wipe and free the slot.
 */
static psa_status_t psa_start_key_creation(
    psa_key_creation_method_t method,
    const psa_key_attributes_t *attributes,
    psa_key_slot_t **p_slot,
    psa_se_drv_table_entry_t **p_drv )
{
    psa_status_t status;
    psa_key_id_t volatile_key_id;
    psa_key_slot_t *slot;

    (void) method;
    *p_drv = NULL;

    status = psa_validate_key_attributes( attributes, p_drv );
    if( status != PSA_SUCCESS )
        return( status );

    status = psa_get_empty_key_slot( &volatile_key_id, p_slot );
    if( status != PSA_SUCCESS )
        return( status );
    slot = *p_slot;

    /* We're storing the declared bit-size of the key. It's up to each
     * creation mechanism to verify that this information is correct.
     * It's automatically correct for mechanisms that use the bit-size as
     * an input (generate, device) but not for those where the bit-size
     * is optional (import, copy). In case of a volatile key, assign it the
     * volatile key identifier associated to the slot returned to contain its
     * definition. */

    slot->attr = attributes->core;
    if( PSA_KEY_LIFETIME_IS_VOLATILE( slot->attr.lifetime ) )
    {
#if !defined(MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER)
        slot->attr.id = volatile_key_id;
#else
        slot->attr.id.key_id = volatile_key_id;
#endif
    }

    /* Erase external-only flags from the internal copy. To access
     * external-only flags, query `attributes`. Thanks to the check
     * in psa_validate_key_attributes(), this leaves the dual-use
     * flags and any internal flag that psa_get_empty_key_slot()
     * may have set. */
    slot->attr.flags &= ~MBEDTLS_PSA_KA_MASK_EXTERNAL_ONLY;

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    /* For a key in a secure element, we need to do three things
     * when creating or registering a persistent key:
     * create the key file in internal storage, create the
     * key inside the secure element, and update the driver's
     * persistent data. This is done by starting a transaction that will
     * encompass these three actions.
     * For registering a volatile key, we just need to find an appropriate
     * slot number inside the SE. Since the key is designated volatile, creating
     * a transaction is not required. */
    /* The first thing to do is to find a slot number for the new key.
     * We save the slot number in persistent storage as part of the
     * transaction data. It will be needed to recover if the power
     * fails during the key creation process, to clean up on the secure
     * element side after restarting. Obtaining a slot number from the
     * secure element driver updates its persistent state, but we do not yet
     * save the driver's persistent state, so that if the power fails,
     * we can roll back to a state where the key doesn't exist. */
    if( *p_drv != NULL )
    {
        psa_key_slot_number_t slot_number;
        status = psa_find_se_slot_for_key( attributes, method, *p_drv,
                                           &slot_number );
        if( status != PSA_SUCCESS )
            return( status );

        if( ! PSA_KEY_LIFETIME_IS_VOLATILE( attributes->core.lifetime ) )
        {
            psa_crypto_prepare_transaction( PSA_CRYPTO_TRANSACTION_CREATE_KEY );
            psa_crypto_transaction.key.lifetime = slot->attr.lifetime;
            psa_crypto_transaction.key.slot = slot_number;
            psa_crypto_transaction.key.id = slot->attr.id;
            status = psa_crypto_save_transaction( );
            if( status != PSA_SUCCESS )
            {
                (void) psa_crypto_stop_transaction( );
                return( status );
            }
        }

        status = psa_copy_key_material_into_slot(
            slot, (uint8_t *)( &slot_number ), sizeof( slot_number ) );
    }

    if( *p_drv == NULL && method == PSA_KEY_CREATION_REGISTER )
    {
        /* Key registration only makes sense with a secure element. */
        return( PSA_ERROR_INVALID_ARGUMENT );
    }
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */

    return( PSA_SUCCESS );
}

/** Finalize the creation of a key once its key material has been set.
 *
 * This entails writing the key to persistent storage.
 *
 * If this function fails, call psa_fail_key_creation().
 * See the documentation of psa_start_key_creation() for the intended use
 * of this function.
 *
 * If the finalization succeeds, the function unlocks the key slot (it was
 * locked by psa_start_key_creation()) and the key slot cannot be accessed
 * anymore as part of the key creation process.
 *
 * \param[in,out] slot  Pointer to the slot with key material.
 * \param[in] driver    The secure element driver for the key,
 *                      or NULL for a transparent key.
 * \param[out] key      On success, identifier of the key. Note that the
 *                      key identifier is also stored in the key slot.
 *
 * \retval #PSA_SUCCESS
 *         The key was successfully created.
 * \retval #PSA_ERROR_INSUFFICIENT_MEMORY
 * \retval #PSA_ERROR_INSUFFICIENT_STORAGE
 * \retval #PSA_ERROR_ALREADY_EXISTS
 * \retval #PSA_ERROR_DATA_INVALID
 * \retval #PSA_ERROR_DATA_CORRUPT
 * \retval #PSA_ERROR_STORAGE_FAILURE
 *
 * \return If this function fails, the key slot is an invalid state.
 *         You must call psa_fail_key_creation() to wipe and free the slot.
 */
static psa_status_t psa_finish_key_creation(
    psa_key_slot_t *slot,
    psa_se_drv_table_entry_t *driver,
    mbedtls_svc_key_id_t *key)
{
    psa_status_t status = PSA_SUCCESS;
    (void) slot;
    (void) driver;

#if defined(MBEDTLS_PSA_CRYPTO_STORAGE_C)
    if( ! PSA_KEY_LIFETIME_IS_VOLATILE( slot->attr.lifetime ) )
    {
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
        if( driver != NULL )
        {
            psa_se_key_data_storage_t data;
            psa_key_slot_number_t slot_number =
                psa_key_slot_get_slot_number( slot ) ;

#if defined(static_assert)
            static_assert( sizeof( slot_number ) ==
                           sizeof( data.slot_number ),
                           "Slot number size does not match psa_se_key_data_storage_t" );
#endif
            memcpy( &data.slot_number, &slot_number, sizeof( slot_number ) );
            status = psa_save_persistent_key( &slot->attr,
                                              (uint8_t*) &data,
                                              sizeof( data ) );
        }
        else
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */
        {
            /* Key material is saved in export representation in the slot, so
             * just pass the slot buffer for storage. */
            status = psa_save_persistent_key( &slot->attr,
                                              slot->key.data,
                                              slot->key.bytes );
        }
    }
#endif /* defined(MBEDTLS_PSA_CRYPTO_STORAGE_C) */

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    /* Finish the transaction for a key creation. This does not
     * happen when registering an existing key. Detect this case
     * by checking whether a transaction is in progress (actual
     * creation of a persistent key in a secure element requires a transaction,
     * but registration or volatile key creation doesn't use one). */
    if( driver != NULL &&
        psa_crypto_transaction.unknown.type == PSA_CRYPTO_TRANSACTION_CREATE_KEY )
    {
        status = psa_save_se_persistent_data( driver );
        if( status != PSA_SUCCESS )
        {
            psa_destroy_persistent_key( slot->attr.id );
            return( status );
        }
        status = psa_crypto_stop_transaction( );
    }
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */

    if( status == PSA_SUCCESS )
    {
        *key = slot->attr.id;
        status = psa_unlock_key_slot( slot );
        if( status != PSA_SUCCESS )
            *key = MBEDTLS_SVC_KEY_ID_INIT;
    }

    return( status );
}

/** Abort the creation of a key.
 *
 * You may call this function after calling psa_start_key_creation(),
 * or after psa_finish_key_creation() fails. In other circumstances, this
 * function may not clean up persistent storage.
 * See the documentation of psa_start_key_creation() for the intended use
 * of this function.
 *
 * \param[in,out] slot  Pointer to the slot with key material.
 * \param[in] driver    The secure element driver for the key,
 *                      or NULL for a transparent key.
 */
static void psa_fail_key_creation( psa_key_slot_t *slot,
                                   psa_se_drv_table_entry_t *driver )
{
    (void) driver;

    if( slot == NULL )
        return;

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    /* TODO: If the key has already been created in the secure
     * element, and the failure happened later (when saving metadata
     * to internal storage), we need to destroy the key in the secure
     * element.
     * https://github.com/ARMmbed/mbed-crypto/issues/217
     */

    /* Abort the ongoing transaction if any (there may not be one if
     * the creation process failed before starting one, or if the
     * key creation is a registration of a key in a secure element).
     * Earlier functions must already have done what it takes to undo any
     * partial creation. All that's left is to update the transaction data
     * itself. */
    (void) psa_crypto_stop_transaction( );
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */

    psa_wipe_key_slot( slot );
}

/** Validate optional attributes during key creation.
 *
 * Some key attributes are optional during key creation. If they are
 * specified in the attributes structure, check that they are consistent
 * with the data in the slot.
 *
 * This function should be called near the end of key creation, after
 * the slot in memory is fully populated but before saving persistent data.
 */
static psa_status_t psa_validate_optional_attributes(
    const psa_key_slot_t *slot,
    const psa_key_attributes_t *attributes )
{
    if( attributes->core.type != 0 )
    {
        if( attributes->core.type != slot->attr.type )
            return( PSA_ERROR_INVALID_ARGUMENT );
    }

    if( attributes->domain_parameters_size != 0 )
    {
#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR) || \
    defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY)
        if( PSA_KEY_TYPE_IS_RSA( slot->attr.type ) )
        {
            mbedtls_rsa_context *rsa = NULL;
            mbedtls_mpi actual, required;
            int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

            psa_status_t status = mbedtls_psa_rsa_load_representation(
                                      slot->attr.type,
                                      slot->key.data,
                                      slot->key.bytes,
                                      &rsa );
            if( status != PSA_SUCCESS )
                return( status );

            mbedtls_mpi_init( &actual );
            mbedtls_mpi_init( &required );
            ret = mbedtls_rsa_export( rsa,
                                      NULL, NULL, NULL, NULL, &actual );
            mbedtls_rsa_free( rsa );
            mbedtls_free( rsa );
            if( ret != 0 )
                goto rsa_exit;
            ret = mbedtls_mpi_read_binary( &required,
                                           attributes->domain_parameters,
                                           attributes->domain_parameters_size );
            if( ret != 0 )
                goto rsa_exit;
            if( mbedtls_mpi_cmp_mpi( &actual, &required ) != 0 )
                ret = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
        rsa_exit:
            mbedtls_mpi_free( &actual );
            mbedtls_mpi_free( &required );
            if( ret != 0)
                return( mbedtls_to_psa_error( ret ) );
        }
        else
#endif /* defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR) ||
        * defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_PUBLIC_KEY) */
        {
            return( PSA_ERROR_INVALID_ARGUMENT );
        }
    }

    if( attributes->core.bits != 0 )
    {
        if( attributes->core.bits != slot->attr.bits )
            return( PSA_ERROR_INVALID_ARGUMENT );
    }

    return( PSA_SUCCESS );
}

psa_status_t psa_import_key( const psa_key_attributes_t *attributes,
                             const uint8_t *data,
                             size_t data_length,
                             mbedtls_svc_key_id_t *key )
{
    psa_status_t status;
    psa_key_slot_t *slot = NULL;
    psa_se_drv_table_entry_t *driver = NULL;
    size_t bits;

    *key = MBEDTLS_SVC_KEY_ID_INIT;

    /* Reject zero-length symmetric keys (including raw data key objects).
     * This also rejects any key which might be encoded as an empty string,
     * which is never valid. */
    if( data_length == 0 )
        return( PSA_ERROR_INVALID_ARGUMENT );

    status = psa_start_key_creation( PSA_KEY_CREATION_IMPORT, attributes,
                                     &slot, &driver );
    if( status != PSA_SUCCESS )
        goto exit;

    /* In the case of a transparent key or an opaque key stored in local
     * storage (thus not in the case of generating a key in a secure element
     * or cryptoprocessor with storage), we have to allocate a buffer to
     * hold the generated key material. */
    if( slot->key.data == NULL )
    {
        status = psa_allocate_buffer_to_slot( slot, data_length );
        if( status != PSA_SUCCESS )
            goto exit;
    }

    bits = slot->attr.bits;
    status = psa_driver_wrapper_import_key( attributes,
                                            data, data_length,
                                            slot->key.data,
                                            slot->key.bytes,
                                            &slot->key.bytes, &bits );
    if( status != PSA_SUCCESS )
        goto exit;

    if( slot->attr.bits == 0 )
        slot->attr.bits = (psa_key_bits_t) bits;
    else if( bits != slot->attr.bits )
    {
        status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    status = psa_validate_optional_attributes( slot, attributes );
    if( status != PSA_SUCCESS )
        goto exit;

    status = psa_finish_key_creation( slot, driver, key );
exit:
    if( status != PSA_SUCCESS )
        psa_fail_key_creation( slot, driver );

    return( status );
}

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
psa_status_t mbedtls_psa_register_se_key(
    const psa_key_attributes_t *attributes )
{
    psa_status_t status;
    psa_key_slot_t *slot = NULL;
    psa_se_drv_table_entry_t *driver = NULL;
    mbedtls_svc_key_id_t key = MBEDTLS_SVC_KEY_ID_INIT;

    /* Leaving attributes unspecified is not currently supported.
     * It could make sense to query the key type and size from the
     * secure element, but not all secure elements support this
     * and the driver HAL doesn't currently support it. */
    if( psa_get_key_type( attributes ) == PSA_KEY_TYPE_NONE )
        return( PSA_ERROR_NOT_SUPPORTED );
    if( psa_get_key_bits( attributes ) == 0 )
        return( PSA_ERROR_NOT_SUPPORTED );

    status = psa_start_key_creation( PSA_KEY_CREATION_REGISTER, attributes,
                                     &slot, &driver );
    if( status != PSA_SUCCESS )
        goto exit;

    status = psa_finish_key_creation( slot, driver, &key );

exit:
    if( status != PSA_SUCCESS )
        psa_fail_key_creation( slot, driver );

    /* Registration doesn't keep the key in RAM. */
    psa_close_key( key );
    return( status );
}
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */

static psa_status_t psa_copy_key_material( const psa_key_slot_t *source,
                                           psa_key_slot_t *target )
{
    psa_status_t status = psa_copy_key_material_into_slot( target,
                                                           source->key.data,
                                                           source->key.bytes );
    if( status != PSA_SUCCESS )
        return( status );

    target->attr.type = source->attr.type;
    target->attr.bits = source->attr.bits;

    return( PSA_SUCCESS );
}

psa_status_t psa_copy_key( mbedtls_svc_key_id_t source_key,
                           const psa_key_attributes_t *specified_attributes,
                           mbedtls_svc_key_id_t *target_key )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *source_slot = NULL;
    psa_key_slot_t *target_slot = NULL;
    psa_key_attributes_t actual_attributes = *specified_attributes;
    psa_se_drv_table_entry_t *driver = NULL;

    *target_key = MBEDTLS_SVC_KEY_ID_INIT;

    status = psa_get_and_lock_transparent_key_slot_with_policy(
                 source_key, &source_slot, PSA_KEY_USAGE_COPY, 0 );
    if( status != PSA_SUCCESS )
        goto exit;

    status = psa_validate_optional_attributes( source_slot,
                                               specified_attributes );
    if( status != PSA_SUCCESS )
        goto exit;

    status = psa_restrict_key_policy( source_slot->attr.type,
                                      &actual_attributes.core.policy,
                                      &source_slot->attr.policy );
    if( status != PSA_SUCCESS )
        goto exit;

    status = psa_start_key_creation( PSA_KEY_CREATION_COPY, &actual_attributes,
                                     &target_slot, &driver );
    if( status != PSA_SUCCESS )
        goto exit;

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    if( driver != NULL )
    {
        /* Copying to a secure element is not implemented yet. */
        status = PSA_ERROR_NOT_SUPPORTED;
        goto exit;
    }
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */

    status = psa_copy_key_material( source_slot, target_slot );
    if( status != PSA_SUCCESS )
        goto exit;

    status = psa_finish_key_creation( target_slot, driver, target_key );
exit:
    if( status != PSA_SUCCESS )
        psa_fail_key_creation( target_slot, driver );

    unlock_status = psa_unlock_key_slot( source_slot );

    return( ( status == PSA_SUCCESS ) ? unlock_status : status );
}



/****************************************************************/
/* Message digests */
/****************************************************************/

#if defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_SIGN) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_OAEP) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PSS) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_DETERMINISTIC_ECDSA)
const mbedtls_md_info_t *mbedtls_md_info_from_psa( psa_algorithm_t alg )
{
    switch( alg )
    {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD2)
        case PSA_ALG_MD2:
            return( &mbedtls_md2_info );
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD4)
        case PSA_ALG_MD4:
            return( &mbedtls_md4_info );
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD5)
        case PSA_ALG_MD5:
            return( &mbedtls_md5_info );
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_RIPEMD160)
        case PSA_ALG_RIPEMD160:
            return( &mbedtls_ripemd160_info );
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_1)
        case PSA_ALG_SHA_1:
            return( &mbedtls_sha1_info );
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_224)
        case PSA_ALG_SHA_224:
            return( &mbedtls_sha224_info );
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_256)
        case PSA_ALG_SHA_256:
            return( &mbedtls_sha256_info );
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_384)
        case PSA_ALG_SHA_384:
            return( &mbedtls_sha384_info );
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_512)
        case PSA_ALG_SHA_512:
            return( &mbedtls_sha512_info );
#endif
        default:
            return( NULL );
    }
}
#endif /* defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_SIGN) ||
        * defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_OAEP) ||
        * defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PSS) ||
        * defined(MBEDTLS_PSA_BUILTIN_ALG_DETERMINISTIC_ECDSA) */

psa_status_t psa_hash_abort( psa_hash_operation_t *operation )
{
    switch( operation->alg )
    {
        case 0:
            /* The object has (apparently) been initialized but it is not
             * in use. It's ok to call abort on such an object, and there's
             * nothing to do. */
            break;
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD2)
        case PSA_ALG_MD2:
            mbedtls_md2_free( &operation->ctx.md2 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD4)
        case PSA_ALG_MD4:
            mbedtls_md4_free( &operation->ctx.md4 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD5)
        case PSA_ALG_MD5:
            mbedtls_md5_free( &operation->ctx.md5 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_RIPEMD160)
        case PSA_ALG_RIPEMD160:
            mbedtls_ripemd160_free( &operation->ctx.ripemd160 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_1)
        case PSA_ALG_SHA_1:
            mbedtls_sha1_free( &operation->ctx.sha1 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_224)
        case PSA_ALG_SHA_224:
            mbedtls_sha256_free( &operation->ctx.sha256 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_256)
        case PSA_ALG_SHA_256:
            mbedtls_sha256_free( &operation->ctx.sha256 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_384)
        case PSA_ALG_SHA_384:
            mbedtls_sha512_free( &operation->ctx.sha512 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_512)
        case PSA_ALG_SHA_512:
            mbedtls_sha512_free( &operation->ctx.sha512 );
            break;
#endif
        default:
            return( PSA_ERROR_BAD_STATE );
    }
    operation->alg = 0;
    return( PSA_SUCCESS );
}

psa_status_t psa_hash_setup( psa_hash_operation_t *operation,
                             psa_algorithm_t alg )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    /* A context must be freshly initialized before it can be set up. */
    if( operation->alg != 0 )
    {
        return( PSA_ERROR_BAD_STATE );
    }

    switch( alg )
    {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD2)
        case PSA_ALG_MD2:
            mbedtls_md2_init( &operation->ctx.md2 );
            ret = mbedtls_md2_starts_ret( &operation->ctx.md2 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD4)
        case PSA_ALG_MD4:
            mbedtls_md4_init( &operation->ctx.md4 );
            ret = mbedtls_md4_starts_ret( &operation->ctx.md4 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD5)
        case PSA_ALG_MD5:
            mbedtls_md5_init( &operation->ctx.md5 );
            ret = mbedtls_md5_starts_ret( &operation->ctx.md5 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_RIPEMD160)
        case PSA_ALG_RIPEMD160:
            mbedtls_ripemd160_init( &operation->ctx.ripemd160 );
            ret = mbedtls_ripemd160_starts_ret( &operation->ctx.ripemd160 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_1)
        case PSA_ALG_SHA_1:
            mbedtls_sha1_init( &operation->ctx.sha1 );
            ret = mbedtls_sha1_starts_ret( &operation->ctx.sha1 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_224)
        case PSA_ALG_SHA_224:
            mbedtls_sha256_init( &operation->ctx.sha256 );
            ret = mbedtls_sha256_starts_ret( &operation->ctx.sha256, 1 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_256)
        case PSA_ALG_SHA_256:
            mbedtls_sha256_init( &operation->ctx.sha256 );
            ret = mbedtls_sha256_starts_ret( &operation->ctx.sha256, 0 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_384)
        case PSA_ALG_SHA_384:
            mbedtls_sha512_init( &operation->ctx.sha512 );
            ret = mbedtls_sha512_starts_ret( &operation->ctx.sha512, 1 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_512)
        case PSA_ALG_SHA_512:
            mbedtls_sha512_init( &operation->ctx.sha512 );
            ret = mbedtls_sha512_starts_ret( &operation->ctx.sha512, 0 );
            break;
#endif
        default:
            return( PSA_ALG_IS_HASH( alg ) ?
                    PSA_ERROR_NOT_SUPPORTED :
                    PSA_ERROR_INVALID_ARGUMENT );
    }
    if( ret == 0 )
        operation->alg = alg;
    else
        psa_hash_abort( operation );
    return( mbedtls_to_psa_error( ret ) );
}

psa_status_t psa_hash_update( psa_hash_operation_t *operation,
                              const uint8_t *input,
                              size_t input_length )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;

    /* Don't require hash implementations to behave correctly on a
     * zero-length input, which may have an invalid pointer. */
    if( input_length == 0 )
        return( PSA_SUCCESS );

    switch( operation->alg )
    {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD2)
        case PSA_ALG_MD2:
            ret = mbedtls_md2_update_ret( &operation->ctx.md2,
                                          input, input_length );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD4)
        case PSA_ALG_MD4:
            ret = mbedtls_md4_update_ret( &operation->ctx.md4,
                                          input, input_length );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD5)
        case PSA_ALG_MD5:
            ret = mbedtls_md5_update_ret( &operation->ctx.md5,
                                          input, input_length );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_RIPEMD160)
        case PSA_ALG_RIPEMD160:
            ret = mbedtls_ripemd160_update_ret( &operation->ctx.ripemd160,
                                                input, input_length );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_1)
        case PSA_ALG_SHA_1:
            ret = mbedtls_sha1_update_ret( &operation->ctx.sha1,
                                           input, input_length );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_224)
        case PSA_ALG_SHA_224:
            ret = mbedtls_sha256_update_ret( &operation->ctx.sha256,
                                             input, input_length );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_256)
        case PSA_ALG_SHA_256:
            ret = mbedtls_sha256_update_ret( &operation->ctx.sha256,
                                             input, input_length );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_384)
        case PSA_ALG_SHA_384:
            ret = mbedtls_sha512_update_ret( &operation->ctx.sha512,
                                             input, input_length );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_512)
        case PSA_ALG_SHA_512:
            ret = mbedtls_sha512_update_ret( &operation->ctx.sha512,
                                             input, input_length );
            break;
#endif
        default:
            (void)input;
            return( PSA_ERROR_BAD_STATE );
    }

    if( ret != 0 )
        psa_hash_abort( operation );
    return( mbedtls_to_psa_error( ret ) );
}

psa_status_t psa_hash_finish( psa_hash_operation_t *operation,
                              uint8_t *hash,
                              size_t hash_size,
                              size_t *hash_length )
{
    psa_status_t status;
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    size_t actual_hash_length = PSA_HASH_LENGTH( operation->alg );

    /* Fill the output buffer with something that isn't a valid hash
     * (barring an attack on the hash and deliberately-crafted input),
     * in case the caller doesn't check the return status properly. */
    *hash_length = hash_size;
    /* If hash_size is 0 then hash may be NULL and then the
     * call to memset would have undefined behavior. */
    if( hash_size != 0 )
        memset( hash, '!', hash_size );

    if( hash_size < actual_hash_length )
    {
        status = PSA_ERROR_BUFFER_TOO_SMALL;
        goto exit;
    }

    switch( operation->alg )
    {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD2)
        case PSA_ALG_MD2:
            ret = mbedtls_md2_finish_ret( &operation->ctx.md2, hash );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD4)
        case PSA_ALG_MD4:
            ret = mbedtls_md4_finish_ret( &operation->ctx.md4, hash );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD5)
        case PSA_ALG_MD5:
            ret = mbedtls_md5_finish_ret( &operation->ctx.md5, hash );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_RIPEMD160)
        case PSA_ALG_RIPEMD160:
            ret = mbedtls_ripemd160_finish_ret( &operation->ctx.ripemd160, hash );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_1)
        case PSA_ALG_SHA_1:
            ret = mbedtls_sha1_finish_ret( &operation->ctx.sha1, hash );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_224)
        case PSA_ALG_SHA_224:
            ret = mbedtls_sha256_finish_ret( &operation->ctx.sha256, hash );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_256)
        case PSA_ALG_SHA_256:
            ret = mbedtls_sha256_finish_ret( &operation->ctx.sha256, hash );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_384)
        case PSA_ALG_SHA_384:
            ret = mbedtls_sha512_finish_ret( &operation->ctx.sha512, hash );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_512)
        case PSA_ALG_SHA_512:
            ret = mbedtls_sha512_finish_ret( &operation->ctx.sha512, hash );
            break;
#endif
        default:
            return( PSA_ERROR_BAD_STATE );
    }
    status = mbedtls_to_psa_error( ret );

exit:
    if( status == PSA_SUCCESS )
    {
        *hash_length = actual_hash_length;
        return( psa_hash_abort( operation ) );
    }
    else
    {
        psa_hash_abort( operation );
        return( status );
    }
}

psa_status_t psa_hash_verify( psa_hash_operation_t *operation,
                              const uint8_t *hash,
                              size_t hash_length )
{
    uint8_t actual_hash[MBEDTLS_MD_MAX_SIZE];
    size_t actual_hash_length;
    psa_status_t status = psa_hash_finish( operation,
                                           actual_hash, sizeof( actual_hash ),
                                           &actual_hash_length );
    if( status != PSA_SUCCESS )
        return( status );
    if( actual_hash_length != hash_length )
        return( PSA_ERROR_INVALID_SIGNATURE );
    if( safer_memcmp( hash, actual_hash, actual_hash_length ) != 0 )
        return( PSA_ERROR_INVALID_SIGNATURE );
    return( PSA_SUCCESS );
}

psa_status_t psa_hash_compute( psa_algorithm_t alg,
                               const uint8_t *input, size_t input_length,
                               uint8_t *hash, size_t hash_size,
                               size_t *hash_length )
{
    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    *hash_length = hash_size;
    status = psa_hash_setup( &operation, alg );
    if( status != PSA_SUCCESS )
        goto exit;
    status = psa_hash_update( &operation, input, input_length );
    if( status != PSA_SUCCESS )
        goto exit;
    status = psa_hash_finish( &operation, hash, hash_size, hash_length );
    if( status != PSA_SUCCESS )
        goto exit;

exit:
    if( status == PSA_SUCCESS )
        status = psa_hash_abort( &operation );
    else
        psa_hash_abort( &operation );
    return( status );
}

psa_status_t psa_hash_compare( psa_algorithm_t alg,
                               const uint8_t *input, size_t input_length,
                               const uint8_t *hash, size_t hash_length )
{
    psa_hash_operation_t operation = PSA_HASH_OPERATION_INIT;
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    status = psa_hash_setup( &operation, alg );
    if( status != PSA_SUCCESS )
        goto exit;
    status = psa_hash_update( &operation, input, input_length );
    if( status != PSA_SUCCESS )
        goto exit;
    status = psa_hash_verify( &operation, hash, hash_length );
    if( status != PSA_SUCCESS )
        goto exit;

exit:
    if( status == PSA_SUCCESS )
        status = psa_hash_abort( &operation );
    else
        psa_hash_abort( &operation );
    return( status );
}

psa_status_t psa_hash_clone( const psa_hash_operation_t *source_operation,
                             psa_hash_operation_t *target_operation )
{
    if( target_operation->alg != 0 )
        return( PSA_ERROR_BAD_STATE );

    switch( source_operation->alg )
    {
        case 0:
            return( PSA_ERROR_BAD_STATE );
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD2)
        case PSA_ALG_MD2:
            mbedtls_md2_clone( &target_operation->ctx.md2,
                               &source_operation->ctx.md2 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD4)
        case PSA_ALG_MD4:
            mbedtls_md4_clone( &target_operation->ctx.md4,
                               &source_operation->ctx.md4 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_MD5)
        case PSA_ALG_MD5:
            mbedtls_md5_clone( &target_operation->ctx.md5,
                               &source_operation->ctx.md5 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_RIPEMD160)
        case PSA_ALG_RIPEMD160:
            mbedtls_ripemd160_clone( &target_operation->ctx.ripemd160,
                                     &source_operation->ctx.ripemd160 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_1)
        case PSA_ALG_SHA_1:
            mbedtls_sha1_clone( &target_operation->ctx.sha1,
                                &source_operation->ctx.sha1 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_224)
        case PSA_ALG_SHA_224:
            mbedtls_sha256_clone( &target_operation->ctx.sha256,
                                  &source_operation->ctx.sha256 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_256)
        case PSA_ALG_SHA_256:
            mbedtls_sha256_clone( &target_operation->ctx.sha256,
                                  &source_operation->ctx.sha256 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_384)
        case PSA_ALG_SHA_384:
            mbedtls_sha512_clone( &target_operation->ctx.sha512,
                                  &source_operation->ctx.sha512 );
            break;
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_SHA_512)
        case PSA_ALG_SHA_512:
            mbedtls_sha512_clone( &target_operation->ctx.sha512,
                                  &source_operation->ctx.sha512 );
            break;
#endif
        default:
            return( PSA_ERROR_NOT_SUPPORTED );
    }

    target_operation->alg = source_operation->alg;
    return( PSA_SUCCESS );
}


/****************************************************************/
/* MAC */
/****************************************************************/

static const mbedtls_cipher_info_t *mbedtls_cipher_info_from_psa(
    psa_algorithm_t alg,
    psa_key_type_t key_type,
    size_t key_bits,
    mbedtls_cipher_id_t* cipher_id )
{
    mbedtls_cipher_mode_t mode;
    mbedtls_cipher_id_t cipher_id_tmp;

    if( PSA_ALG_IS_AEAD( alg ) )
        alg = PSA_ALG_AEAD_WITH_SHORTENED_TAG( alg, 0 );

    if( PSA_ALG_IS_CIPHER( alg ) || PSA_ALG_IS_AEAD( alg ) )
    {
        switch( alg )
        {
            case PSA_ALG_STREAM_CIPHER:
                mode = MBEDTLS_MODE_STREAM;
                break;
            case PSA_ALG_CTR:
                mode = MBEDTLS_MODE_CTR;
                break;
            case PSA_ALG_CFB:
                mode = MBEDTLS_MODE_CFB;
                break;
            case PSA_ALG_OFB:
                mode = MBEDTLS_MODE_OFB;
                break;
            case PSA_ALG_ECB_NO_PADDING:
                mode = MBEDTLS_MODE_ECB;
                break;
            case PSA_ALG_CBC_NO_PADDING:
                mode = MBEDTLS_MODE_CBC;
                break;
            case PSA_ALG_CBC_PKCS7:
                mode = MBEDTLS_MODE_CBC;
                break;
            case PSA_ALG_AEAD_WITH_SHORTENED_TAG( PSA_ALG_CCM, 0 ):
                mode = MBEDTLS_MODE_CCM;
                break;
            case PSA_ALG_AEAD_WITH_SHORTENED_TAG( PSA_ALG_GCM, 0 ):
                mode = MBEDTLS_MODE_GCM;
                break;
            case PSA_ALG_AEAD_WITH_SHORTENED_TAG( PSA_ALG_CHACHA20_POLY1305, 0 ):
                mode = MBEDTLS_MODE_CHACHAPOLY;
                break;
            default:
                return( NULL );
        }
    }
    else if( alg == PSA_ALG_CMAC )
        mode = MBEDTLS_MODE_ECB;
    else
        return( NULL );

    switch( key_type )
    {
        case PSA_KEY_TYPE_AES:
            cipher_id_tmp = MBEDTLS_CIPHER_ID_AES;
            break;
        case PSA_KEY_TYPE_DES:
            /* key_bits is 64 for Single-DES, 128 for two-key Triple-DES,
             * and 192 for three-key Triple-DES. */
            if( key_bits == 64 )
                cipher_id_tmp = MBEDTLS_CIPHER_ID_DES;
            else
                cipher_id_tmp = MBEDTLS_CIPHER_ID_3DES;
            /* mbedtls doesn't recognize two-key Triple-DES as an algorithm,
             * but two-key Triple-DES is functionally three-key Triple-DES
             * with K1=K3, so that's how we present it to mbedtls. */
            if( key_bits == 128 )
                key_bits = 192;
            break;
        case PSA_KEY_TYPE_CAMELLIA:
            cipher_id_tmp = MBEDTLS_CIPHER_ID_CAMELLIA;
            break;
        case PSA_KEY_TYPE_ARC4:
            cipher_id_tmp = MBEDTLS_CIPHER_ID_ARC4;
            break;
        case PSA_KEY_TYPE_CHACHA20:
            cipher_id_tmp = MBEDTLS_CIPHER_ID_CHACHA20;
            break;
        default:
            return( NULL );
    }
    if( cipher_id != NULL )
        *cipher_id = cipher_id_tmp;

    return( mbedtls_cipher_info_from_values( cipher_id_tmp,
                                             (int) key_bits, mode ) );
}

#if defined(MBEDTLS_PSA_BUILTIN_ALG_HMAC)
static size_t psa_get_hash_block_size( psa_algorithm_t alg )
{
    switch( alg )
    {
        case PSA_ALG_MD2:
            return( 16 );
        case PSA_ALG_MD4:
            return( 64 );
        case PSA_ALG_MD5:
            return( 64 );
        case PSA_ALG_RIPEMD160:
            return( 64 );
        case PSA_ALG_SHA_1:
            return( 64 );
        case PSA_ALG_SHA_224:
            return( 64 );
        case PSA_ALG_SHA_256:
            return( 64 );
        case PSA_ALG_SHA_384:
            return( 128 );
        case PSA_ALG_SHA_512:
            return( 128 );
        default:
            return( 0 );
    }
}
#endif /* defined(MBEDTLS_PSA_BUILTIN_ALG_HMAC) */

/* Initialize the MAC operation structure. Once this function has been
 * called, psa_mac_abort can run and will do the right thing. */
static psa_status_t psa_mac_init( psa_mac_operation_t *operation,
                                  psa_algorithm_t alg )
{
    psa_status_t status = PSA_ERROR_NOT_SUPPORTED;

    operation->alg = PSA_ALG_FULL_LENGTH_MAC( alg );
    operation->key_set = 0;
    operation->iv_set = 0;
    operation->iv_required = 0;
    operation->has_input = 0;
    operation->is_sign = 0;

#if defined(MBEDTLS_CMAC_C)
    if( operation->alg == PSA_ALG_CMAC )
    {
        operation->iv_required = 0;
        mbedtls_cipher_init( &operation->ctx.cmac );
        status = PSA_SUCCESS;
    }
    else
#endif /* MBEDTLS_CMAC_C */
#if defined(MBEDTLS_PSA_BUILTIN_ALG_HMAC)
    if( PSA_ALG_IS_HMAC( operation->alg ) )
    {
        /* We'll set up the hash operation later in psa_hmac_setup_internal. */
        operation->ctx.hmac.hash_ctx.alg = 0;
        status = PSA_SUCCESS;
    }
    else
#endif /* MBEDTLS_PSA_BUILTIN_ALG_HMAC */
    {
        if( ! PSA_ALG_IS_MAC( alg ) )
            status = PSA_ERROR_INVALID_ARGUMENT;
    }

    if( status != PSA_SUCCESS )
        memset( operation, 0, sizeof( *operation ) );
    return( status );
}

#if defined(MBEDTLS_PSA_BUILTIN_ALG_HMAC)
static psa_status_t psa_hmac_abort_internal( psa_hmac_internal_data *hmac )
{
    mbedtls_platform_zeroize( hmac->opad, sizeof( hmac->opad ) );
    return( psa_hash_abort( &hmac->hash_ctx ) );
}
#endif /* MBEDTLS_PSA_BUILTIN_ALG_HMAC */

psa_status_t psa_mac_abort( psa_mac_operation_t *operation )
{
    if( operation->alg == 0 )
    {
        /* The object has (apparently) been initialized but it is not
         * in use. It's ok to call abort on such an object, and there's
         * nothing to do. */
        return( PSA_SUCCESS );
    }
    else
#if defined(MBEDTLS_CMAC_C)
    if( operation->alg == PSA_ALG_CMAC )
    {
        mbedtls_cipher_free( &operation->ctx.cmac );
    }
    else
#endif /* MBEDTLS_CMAC_C */
#if defined(MBEDTLS_PSA_BUILTIN_ALG_HMAC)
    if( PSA_ALG_IS_HMAC( operation->alg ) )
    {
        psa_hmac_abort_internal( &operation->ctx.hmac );
    }
    else
#endif /* MBEDTLS_PSA_BUILTIN_ALG_HMAC */
    {
        /* Sanity check (shouldn't happen: operation->alg should
         * always have been initialized to a valid value). */
        goto bad_state;
    }

    operation->alg = 0;
    operation->key_set = 0;
    operation->iv_set = 0;
    operation->iv_required = 0;
    operation->has_input = 0;
    operation->is_sign = 0;

    return( PSA_SUCCESS );

bad_state:
    /* If abort is called on an uninitialized object, we can't trust
     * anything. Wipe the object in case it contains confidential data.
     * This may result in a memory leak if a pointer gets overwritten,
     * but it's too late to do anything about this. */
    memset( operation, 0, sizeof( *operation ) );
    return( PSA_ERROR_BAD_STATE );
}

#if defined(MBEDTLS_CMAC_C)
static psa_status_t psa_cmac_setup( psa_mac_operation_t *operation,
                                    psa_key_slot_t *slot )
{
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    const mbedtls_cipher_info_t *cipher_info =
            mbedtls_cipher_info_from_psa( PSA_ALG_CMAC,
                                          slot->attr.type, slot->attr.bits,
                                          NULL );
    if( cipher_info == NULL )
        return( PSA_ERROR_NOT_SUPPORTED );

    ret = mbedtls_cipher_setup( &operation->ctx.cmac, cipher_info );
    if( ret != 0 )
        goto exit;

    ret = mbedtls_cipher_cmac_starts( &operation->ctx.cmac,
                                      slot->key.data,
                                      slot->attr.bits );
exit:
    return( mbedtls_to_psa_error( ret ) );
}
#endif /* MBEDTLS_CMAC_C */

#if defined(MBEDTLS_PSA_BUILTIN_ALG_HMAC)
static psa_status_t psa_hmac_setup_internal( psa_hmac_internal_data *hmac,
                                             const uint8_t *key,
                                             size_t key_length,
                                             psa_algorithm_t hash_alg )
{
    uint8_t ipad[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
    size_t i;
    size_t hash_size = PSA_HASH_LENGTH( hash_alg );
    size_t block_size = psa_get_hash_block_size( hash_alg );
    psa_status_t status;

    /* Sanity checks on block_size, to guarantee that there won't be a buffer
     * overflow below. This should never trigger if the hash algorithm
     * is implemented correctly. */
    /* The size checks against the ipad and opad buffers cannot be written
     * `block_size > sizeof( ipad ) || block_size > sizeof( hmac->opad )`
     * because that triggers -Wlogical-op on GCC 7.3. */
    if( block_size > sizeof( ipad ) )
        return( PSA_ERROR_NOT_SUPPORTED );
    if( block_size > sizeof( hmac->opad ) )
        return( PSA_ERROR_NOT_SUPPORTED );
    if( block_size < hash_size )
        return( PSA_ERROR_NOT_SUPPORTED );

    if( key_length > block_size )
    {
        status = psa_hash_compute( hash_alg, key, key_length,
                                   ipad, sizeof( ipad ), &key_length );
        if( status != PSA_SUCCESS )
            goto cleanup;
    }
    /* A 0-length key is not commonly used in HMAC when used as a MAC,
     * but it is permitted. It is common when HMAC is used in HKDF, for
     * example. Don't call `memcpy` in the 0-length because `key` could be
     * an invalid pointer which would make the behavior undefined. */
    else if( key_length != 0 )
        memcpy( ipad, key, key_length );

    /* ipad contains the key followed by garbage. Xor and fill with 0x36
     * to create the ipad value. */
    for( i = 0; i < key_length; i++ )
        ipad[i] ^= 0x36;
    memset( ipad + key_length, 0x36, block_size - key_length );

    /* Copy the key material from ipad to opad, flipping the requisite bits,
     * and filling the rest of opad with the requisite constant. */
    for( i = 0; i < key_length; i++ )
        hmac->opad[i] = ipad[i] ^ 0x36 ^ 0x5C;
    memset( hmac->opad + key_length, 0x5C, block_size - key_length );

    status = psa_hash_setup( &hmac->hash_ctx, hash_alg );
    if( status != PSA_SUCCESS )
        goto cleanup;

    status = psa_hash_update( &hmac->hash_ctx, ipad, block_size );

cleanup:
    mbedtls_platform_zeroize( ipad, sizeof( ipad ) );

    return( status );
}
#endif /* MBEDTLS_PSA_BUILTIN_ALG_HMAC */

static psa_status_t psa_mac_setup( psa_mac_operation_t *operation,
                                   mbedtls_svc_key_id_t key,
                                   psa_algorithm_t alg,
                                   int is_sign )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;
    psa_key_usage_t usage =
        is_sign ? PSA_KEY_USAGE_SIGN_HASH : PSA_KEY_USAGE_VERIFY_HASH;

    /* A context must be freshly initialized before it can be set up. */
    if( operation->alg != 0 )
    {
        return( PSA_ERROR_BAD_STATE );
    }

    status = psa_mac_init( operation, alg );
    if( status != PSA_SUCCESS )
        return( status );
    if( is_sign )
        operation->is_sign = 1;

    status = psa_get_and_lock_transparent_key_slot_with_policy(
                 key, &slot, usage, alg );
    if( status != PSA_SUCCESS )
        goto exit;

    /* Validate the combination of key type and algorithm */
    status = psa_mac_key_can_do( alg, slot->attr.type );
    if( status != PSA_SUCCESS )
        goto exit;

    /* Get the output length for the algorithm and key combination. None of the
     * currently supported algorithms have an output length dependent on actual
     * key size, so setting it to a bogus value is currently OK. */
    operation->mac_size = PSA_MAC_LENGTH( slot->attr.type, 0, alg );

    if( operation->mac_size < 4 )
    {
        /* A very short MAC is too short for security since it can be
         * brute-forced. Ancient protocols with 32-bit MACs do exist,
         * so we make this our minimum, even though 32 bits is still
         * too small for security. */
        status = PSA_ERROR_NOT_SUPPORTED;
        goto exit;
    }

    if( operation->mac_size >
        PSA_MAC_LENGTH( slot->attr.type, 0, PSA_ALG_FULL_LENGTH_MAC( alg ) ) )
    {
        /* It's impossible to "truncate" to a larger length than the full length
         * of the algorithm. */
        status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

#if defined(MBEDTLS_CMAC_C)
    if( PSA_ALG_FULL_LENGTH_MAC( alg ) == PSA_ALG_CMAC )
    {
        status = psa_cmac_setup( operation, slot );
    }
    else
#endif /* MBEDTLS_CMAC_C */
#if defined(MBEDTLS_PSA_BUILTIN_ALG_HMAC)
    if( PSA_ALG_IS_HMAC( alg ) )
    {
        /* Sanity check. This shouldn't fail on a valid configuration. */
        if( operation->mac_size > sizeof( operation->ctx.hmac.opad ) )
        {
            status = PSA_ERROR_NOT_SUPPORTED;
            goto exit;
        }

        if( slot->attr.type != PSA_KEY_TYPE_HMAC )
        {
            status = PSA_ERROR_INVALID_ARGUMENT;
            goto exit;
        }

        status = psa_hmac_setup_internal( &operation->ctx.hmac,
                                          slot->key.data,
                                          slot->key.bytes,
                                          PSA_ALG_HMAC_GET_HASH( alg ) );
    }
    else
#endif /* MBEDTLS_PSA_BUILTIN_ALG_HMAC */
    {
        status = PSA_ERROR_NOT_SUPPORTED;
    }

exit:
    if( status != PSA_SUCCESS )
    {
        psa_mac_abort( operation );
    }
    else
    {
        operation->key_set = 1;
    }

    unlock_status = psa_unlock_key_slot( slot );

    return( ( status == PSA_SUCCESS ) ? unlock_status : status );
}

psa_status_t psa_mac_sign_setup( psa_mac_operation_t *operation,
                                 mbedtls_svc_key_id_t key,
                                 psa_algorithm_t alg )
{
    return( psa_mac_setup( operation, key, alg, 1 ) );
}

psa_status_t psa_mac_verify_setup( psa_mac_operation_t *operation,
                                   mbedtls_svc_key_id_t key,
                                   psa_algorithm_t alg )
{
    return( psa_mac_setup( operation, key, alg, 0 ) );
}

psa_status_t psa_mac_update( psa_mac_operation_t *operation,
                             const uint8_t *input,
                             size_t input_length )
{
    psa_status_t status = PSA_ERROR_BAD_STATE;
    if( ! operation->key_set )
        return( PSA_ERROR_BAD_STATE );
    if( operation->iv_required && ! operation->iv_set )
        return( PSA_ERROR_BAD_STATE );
    operation->has_input = 1;

#if defined(MBEDTLS_CMAC_C)
    if( operation->alg == PSA_ALG_CMAC )
    {
        int ret = mbedtls_cipher_cmac_update( &operation->ctx.cmac,
                                              input, input_length );
        status = mbedtls_to_psa_error( ret );
    }
    else
#endif /* MBEDTLS_CMAC_C */
#if defined(MBEDTLS_PSA_BUILTIN_ALG_HMAC)
    if( PSA_ALG_IS_HMAC( operation->alg ) )
    {
        status = psa_hash_update( &operation->ctx.hmac.hash_ctx, input,
                                  input_length );
    }
    else
#endif /* MBEDTLS_PSA_BUILTIN_ALG_HMAC */
    {
        /* This shouldn't happen if `operation` was initialized by
         * a setup function. */
        return( PSA_ERROR_BAD_STATE );
    }

    if( status != PSA_SUCCESS )
        psa_mac_abort( operation );
    return( status );
}

#if defined(MBEDTLS_PSA_BUILTIN_ALG_HMAC)
static psa_status_t psa_hmac_finish_internal( psa_hmac_internal_data *hmac,
                                              uint8_t *mac,
                                              size_t mac_size )
{
    uint8_t tmp[MBEDTLS_MD_MAX_SIZE];
    psa_algorithm_t hash_alg = hmac->hash_ctx.alg;
    size_t hash_size = 0;
    size_t block_size = psa_get_hash_block_size( hash_alg );
    psa_status_t status;

    status = psa_hash_finish( &hmac->hash_ctx, tmp, sizeof( tmp ), &hash_size );
    if( status != PSA_SUCCESS )
        return( status );
    /* From here on, tmp needs to be wiped. */

    status = psa_hash_setup( &hmac->hash_ctx, hash_alg );
    if( status != PSA_SUCCESS )
        goto exit;

    status = psa_hash_update( &hmac->hash_ctx, hmac->opad, block_size );
    if( status != PSA_SUCCESS )
        goto exit;

    status = psa_hash_update( &hmac->hash_ctx, tmp, hash_size );
    if( status != PSA_SUCCESS )
        goto exit;

    status = psa_hash_finish( &hmac->hash_ctx, tmp, sizeof( tmp ), &hash_size );
    if( status != PSA_SUCCESS )
        goto exit;

    memcpy( mac, tmp, mac_size );

exit:
    mbedtls_platform_zeroize( tmp, hash_size );
    return( status );
}
#endif /* MBEDTLS_PSA_BUILTIN_ALG_HMAC */

static psa_status_t psa_mac_finish_internal( psa_mac_operation_t *operation,
                                             uint8_t *mac,
                                             size_t mac_size )
{
    if( ! operation->key_set )
        return( PSA_ERROR_BAD_STATE );
    if( operation->iv_required && ! operation->iv_set )
        return( PSA_ERROR_BAD_STATE );

    if( mac_size < operation->mac_size )
        return( PSA_ERROR_BUFFER_TOO_SMALL );

#if defined(MBEDTLS_CMAC_C)
    if( operation->alg == PSA_ALG_CMAC )
    {
        uint8_t tmp[PSA_BLOCK_CIPHER_BLOCK_MAX_SIZE];
        int ret = mbedtls_cipher_cmac_finish( &operation->ctx.cmac, tmp );
        if( ret == 0 )
            memcpy( mac, tmp, operation->mac_size );
        mbedtls_platform_zeroize( tmp, sizeof( tmp ) );
        return( mbedtls_to_psa_error( ret ) );
    }
    else
#endif /* MBEDTLS_CMAC_C */
#if defined(MBEDTLS_PSA_BUILTIN_ALG_HMAC)
    if( PSA_ALG_IS_HMAC( operation->alg ) )
    {
        return( psa_hmac_finish_internal( &operation->ctx.hmac,
                                          mac, operation->mac_size ) );
    }
    else
#endif /* MBEDTLS_PSA_BUILTIN_ALG_HMAC */
    {
        /* This shouldn't happen if `operation` was initialized by
         * a setup function. */
        return( PSA_ERROR_BAD_STATE );
    }
}

psa_status_t psa_mac_sign_finish( psa_mac_operation_t *operation,
                                  uint8_t *mac,
                                  size_t mac_size,
                                  size_t *mac_length )
{
    psa_status_t status;

    if( operation->alg == 0 )
    {
        return( PSA_ERROR_BAD_STATE );
    }

    /* Fill the output buffer with something that isn't a valid mac
     * (barring an attack on the mac and deliberately-crafted input),
     * in case the caller doesn't check the return status properly. */
    *mac_length = mac_size;
    /* If mac_size is 0 then mac may be NULL and then the
     * call to memset would have undefined behavior. */
    if( mac_size != 0 )
        memset( mac, '!', mac_size );

    if( ! operation->is_sign )
    {
        return( PSA_ERROR_BAD_STATE );
    }

    status = psa_mac_finish_internal( operation, mac, mac_size );

    if( status == PSA_SUCCESS )
    {
        status = psa_mac_abort( operation );
        if( status == PSA_SUCCESS )
            *mac_length = operation->mac_size;
        else
            memset( mac, '!', mac_size );
    }
    else
        psa_mac_abort( operation );
    return( status );
}

psa_status_t psa_mac_verify_finish( psa_mac_operation_t *operation,
                                    const uint8_t *mac,
                                    size_t mac_length )
{
    uint8_t actual_mac[PSA_MAC_MAX_SIZE];
    psa_status_t status;

    if( operation->alg == 0 )
    {
        return( PSA_ERROR_BAD_STATE );
    }

    if( operation->is_sign )
    {
        return( PSA_ERROR_BAD_STATE );
    }
    if( operation->mac_size != mac_length )
    {
        status = PSA_ERROR_INVALID_SIGNATURE;
        goto cleanup;
    }

    status = psa_mac_finish_internal( operation,
                                      actual_mac, sizeof( actual_mac ) );
    if( status != PSA_SUCCESS )
        goto cleanup;

    if( safer_memcmp( mac, actual_mac, mac_length ) != 0 )
        status = PSA_ERROR_INVALID_SIGNATURE;

cleanup:
    if( status == PSA_SUCCESS )
        status = psa_mac_abort( operation );
    else
        psa_mac_abort( operation );

    mbedtls_platform_zeroize( actual_mac, sizeof( actual_mac ) );

    return( status );
}



/****************************************************************/
/* Asymmetric cryptography */
/****************************************************************/

psa_status_t psa_sign_hash_internal(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg, const uint8_t *hash, size_t hash_length,
    uint8_t *signature, size_t signature_size, size_t *signature_length )
{
#if defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_SIGN) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PSS)
    if( attributes->core.type == PSA_KEY_TYPE_RSA_KEY_PAIR )
    {
        return( mbedtls_psa_rsa_sign_hash(
                    attributes,
                    key_buffer, key_buffer_size,
                    alg, hash, hash_length,
                    signature, signature_size, signature_length ) );
    }
    else
#endif /* defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_SIGN) ||
        * defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PSS) */
#if defined(MBEDTLS_PSA_BUILTIN_ALG_ECDSA) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_DETERMINISTIC_ECDSA)
    if( PSA_KEY_TYPE_IS_ECC( attributes->core.type ) )
    {
        if( PSA_ALG_IS_ECDSA( alg ) )
        {
            return( mbedtls_psa_ecdsa_sign_hash(
                        attributes,
                        key_buffer, key_buffer_size,
                        alg, hash, hash_length,
                        signature, signature_size, signature_length ) );
        }
        else
        {
            return( PSA_ERROR_INVALID_ARGUMENT );
        }
    }
    else
#endif /* defined(MBEDTLS_PSA_BUILTIN_ALG_ECDSA) ||
        * defined(MBEDTLS_PSA_BUILTIN_ALG_DETERMINISTIC_ECDSA) */
    {
        (void)attributes;
        (void)key_buffer;
        (void)key_buffer_size;
        (void)alg;
        (void)hash;
        (void)hash_length;
        (void)signature;
        (void)signature_size;
        (void)signature_length;

        return( PSA_ERROR_NOT_SUPPORTED );
    }
}

psa_status_t psa_sign_hash( mbedtls_svc_key_id_t key,
                            psa_algorithm_t alg,
                            const uint8_t *hash,
                            size_t hash_length,
                            uint8_t *signature,
                            size_t signature_size,
                            size_t *signature_length )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;

    *signature_length = signature_size;
    /* Immediately reject a zero-length signature buffer. This guarantees
     * that signature must be a valid pointer. (On the other hand, the hash
     * buffer can in principle be empty since it doesn't actually have
     * to be a hash.) */
    if( signature_size == 0 )
        return( PSA_ERROR_BUFFER_TOO_SMALL );

    status = psa_get_and_lock_key_slot_with_policy( key, &slot,
                                                    PSA_KEY_USAGE_SIGN_HASH,
                                                    alg );
    if( status != PSA_SUCCESS )
        goto exit;
    if( ! PSA_KEY_TYPE_IS_KEY_PAIR( slot->attr.type ) )
    {
        status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

    psa_key_attributes_t attributes = {
      .core = slot->attr
    };

    status = psa_driver_wrapper_sign_hash(
        &attributes, slot->key.data, slot->key.bytes,
        alg, hash, hash_length,
        signature, signature_size, signature_length );

exit:
    /* Fill the unused part of the output buffer (the whole buffer on error,
     * the trailing part on success) with something that isn't a valid mac
     * (barring an attack on the mac and deliberately-crafted input),
     * in case the caller doesn't check the return status properly. */
    if( status == PSA_SUCCESS )
        memset( signature + *signature_length, '!',
                signature_size - *signature_length );
    else
        memset( signature, '!', signature_size );
    /* If signature_size is 0 then we have nothing to do. We must not call
     * memset because signature may be NULL in this case. */

    unlock_status = psa_unlock_key_slot( slot );

    return( ( status == PSA_SUCCESS ) ? unlock_status : status );
}

psa_status_t psa_verify_hash_internal(
    const psa_key_attributes_t *attributes,
    const uint8_t *key_buffer, size_t key_buffer_size,
    psa_algorithm_t alg, const uint8_t *hash, size_t hash_length,
    const uint8_t *signature, size_t signature_length )
{
#if defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_SIGN) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PSS)
    if( PSA_KEY_TYPE_IS_RSA( attributes->core.type ) )
    {
        return( mbedtls_psa_rsa_verify_hash(
                    attributes,
                    key_buffer, key_buffer_size,
                    alg, hash, hash_length,
                    signature, signature_length ) );
    }
    else
#endif /* defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_SIGN) ||
        * defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PSS) */
    if( PSA_KEY_TYPE_IS_ECC( attributes->core.type ) )
    {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_ECDSA) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_DETERMINISTIC_ECDSA)
        if( PSA_ALG_IS_ECDSA( alg ) )
        {
            return( mbedtls_psa_ecdsa_verify_hash(
                        attributes,
                        key_buffer, key_buffer_size,
                        alg, hash, hash_length,
                        signature, signature_length ) );
        }
        else
#endif /* defined(MBEDTLS_PSA_BUILTIN_ALG_ECDSA) ||
        * defined(MBEDTLS_PSA_BUILTIN_ALG_DETERMINISTIC_ECDSA) */
        {
            return( PSA_ERROR_INVALID_ARGUMENT );
        }
    }
    else
    {
        (void)key_buffer;
        (void)key_buffer_size;
        (void)alg;
        (void)hash;
        (void)hash_length;
        (void)signature;
        (void)signature_length;

        return( PSA_ERROR_NOT_SUPPORTED );
    }
}

psa_status_t psa_verify_hash( mbedtls_svc_key_id_t key,
                              psa_algorithm_t alg,
                              const uint8_t *hash,
                              size_t hash_length,
                              const uint8_t *signature,
                              size_t signature_length )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;

    status = psa_get_and_lock_key_slot_with_policy( key, &slot,
                                                    PSA_KEY_USAGE_VERIFY_HASH,
                                                    alg );
    if( status != PSA_SUCCESS )
        return( status );

    psa_key_attributes_t attributes = {
      .core = slot->attr
    };

    status = psa_driver_wrapper_verify_hash(
        &attributes, slot->key.data, slot->key.bytes,
        alg, hash, hash_length,
        signature, signature_length );

    unlock_status = psa_unlock_key_slot( slot );

    return( ( status == PSA_SUCCESS ) ? unlock_status : status );
}

#if defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_OAEP)
static void psa_rsa_oaep_set_padding_mode( psa_algorithm_t alg,
                                           mbedtls_rsa_context *rsa )
{
    psa_algorithm_t hash_alg = PSA_ALG_RSA_OAEP_GET_HASH( alg );
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_psa( hash_alg );
    mbedtls_md_type_t md_alg = mbedtls_md_get_type( md_info );
    mbedtls_rsa_set_padding( rsa, MBEDTLS_RSA_PKCS_V21, md_alg );
}
#endif /* defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_OAEP) */

psa_status_t psa_asymmetric_encrypt( mbedtls_svc_key_id_t key,
                                     psa_algorithm_t alg,
                                     const uint8_t *input,
                                     size_t input_length,
                                     const uint8_t *salt,
                                     size_t salt_length,
                                     uint8_t *output,
                                     size_t output_size,
                                     size_t *output_length )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;

    (void) input;
    (void) input_length;
    (void) salt;
    (void) output;
    (void) output_size;

    *output_length = 0;

    if( ! PSA_ALG_IS_RSA_OAEP( alg ) && salt_length != 0 )
        return( PSA_ERROR_INVALID_ARGUMENT );

    status = psa_get_and_lock_transparent_key_slot_with_policy(
                 key, &slot, PSA_KEY_USAGE_ENCRYPT, alg );
    if( status != PSA_SUCCESS )
        return( status );
    if( ! ( PSA_KEY_TYPE_IS_PUBLIC_KEY( slot->attr.type ) ||
            PSA_KEY_TYPE_IS_KEY_PAIR( slot->attr.type ) ) )
    {
        status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

#if defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_CRYPT) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_OAEP)
    if( PSA_KEY_TYPE_IS_RSA( slot->attr.type ) )
    {
        mbedtls_rsa_context *rsa = NULL;
        status = mbedtls_psa_rsa_load_representation( slot->attr.type,
                                                      slot->key.data,
                                                      slot->key.bytes,
                                                      &rsa );
        if( status != PSA_SUCCESS )
            goto rsa_exit;

        if( output_size < mbedtls_rsa_get_len( rsa ) )
        {
            status = PSA_ERROR_BUFFER_TOO_SMALL;
            goto rsa_exit;
        }
#if defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_CRYPT)
        if( alg == PSA_ALG_RSA_PKCS1V15_CRYPT )
        {
            status = mbedtls_to_psa_error(
                    mbedtls_rsa_pkcs1_encrypt( rsa,
                                               mbedtls_psa_get_random,
                                               MBEDTLS_PSA_RANDOM_STATE,
                                               MBEDTLS_RSA_PUBLIC,
                                               input_length,
                                               input,
                                               output ) );
        }
        else
#endif /* MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_CRYPT */
#if defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_OAEP)
        if( PSA_ALG_IS_RSA_OAEP( alg ) )
        {
            psa_rsa_oaep_set_padding_mode( alg, rsa );
            status = mbedtls_to_psa_error(
                mbedtls_rsa_rsaes_oaep_encrypt( rsa,
                                                mbedtls_psa_get_random,
                                                MBEDTLS_PSA_RANDOM_STATE,
                                                MBEDTLS_RSA_PUBLIC,
                                                salt, salt_length,
                                                input_length,
                                                input,
                                                output ) );
        }
        else
#endif /* MBEDTLS_PSA_BUILTIN_ALG_RSA_OAEP */
        {
            status = PSA_ERROR_INVALID_ARGUMENT;
            goto rsa_exit;
        }
rsa_exit:
        if( status == PSA_SUCCESS )
            *output_length = mbedtls_rsa_get_len( rsa );

        mbedtls_rsa_free( rsa );
        mbedtls_free( rsa );
    }
    else
#endif /* defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_CRYPT) ||
        * defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_OAEP) */
    {
        status = PSA_ERROR_NOT_SUPPORTED;
    }

exit:
    unlock_status = psa_unlock_key_slot( slot );

    return( ( status == PSA_SUCCESS ) ? unlock_status : status );
}

psa_status_t psa_asymmetric_decrypt( mbedtls_svc_key_id_t key,
                                     psa_algorithm_t alg,
                                     const uint8_t *input,
                                     size_t input_length,
                                     const uint8_t *salt,
                                     size_t salt_length,
                                     uint8_t *output,
                                     size_t output_size,
                                     size_t *output_length )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;

    (void) input;
    (void) input_length;
    (void) salt;
    (void) output;
    (void) output_size;

    *output_length = 0;

    if( ! PSA_ALG_IS_RSA_OAEP( alg ) && salt_length != 0 )
        return( PSA_ERROR_INVALID_ARGUMENT );

    status = psa_get_and_lock_transparent_key_slot_with_policy(
                 key, &slot, PSA_KEY_USAGE_DECRYPT, alg );
    if( status != PSA_SUCCESS )
        return( status );
    if( ! PSA_KEY_TYPE_IS_KEY_PAIR( slot->attr.type ) )
    {
        status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }

#if defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_CRYPT) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_OAEP)
    if( slot->attr.type == PSA_KEY_TYPE_RSA_KEY_PAIR )
    {
        mbedtls_rsa_context *rsa = NULL;
        status = mbedtls_psa_rsa_load_representation( slot->attr.type,
                                                      slot->key.data,
                                                      slot->key.bytes,
                                                      &rsa );
        if( status != PSA_SUCCESS )
            goto exit;

        if( input_length != mbedtls_rsa_get_len( rsa ) )
        {
            status = PSA_ERROR_INVALID_ARGUMENT;
            goto rsa_exit;
        }

#if defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_CRYPT)
        if( alg == PSA_ALG_RSA_PKCS1V15_CRYPT )
        {
            status = mbedtls_to_psa_error(
                mbedtls_rsa_pkcs1_decrypt( rsa,
                                           mbedtls_psa_get_random,
                                           MBEDTLS_PSA_RANDOM_STATE,
                                           MBEDTLS_RSA_PRIVATE,
                                           output_length,
                                           input,
                                           output,
                                           output_size ) );
        }
        else
#endif /* MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_CRYPT */
#if defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_OAEP)
        if( PSA_ALG_IS_RSA_OAEP( alg ) )
        {
            psa_rsa_oaep_set_padding_mode( alg, rsa );
            status = mbedtls_to_psa_error(
                mbedtls_rsa_rsaes_oaep_decrypt( rsa,
                                                mbedtls_psa_get_random,
                                                MBEDTLS_PSA_RANDOM_STATE,
                                                MBEDTLS_RSA_PRIVATE,
                                                salt, salt_length,
                                                output_length,
                                                input,
                                                output,
                                                output_size ) );
        }
        else
#endif /* MBEDTLS_PSA_BUILTIN_ALG_RSA_OAEP */
        {
            status = PSA_ERROR_INVALID_ARGUMENT;
        }

rsa_exit:
        mbedtls_rsa_free( rsa );
        mbedtls_free( rsa );
    }
    else
#endif /* defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_PKCS1V15_CRYPT) ||
        * defined(MBEDTLS_PSA_BUILTIN_ALG_RSA_OAEP) */
    {
        status = PSA_ERROR_NOT_SUPPORTED;
    }

exit:
    unlock_status = psa_unlock_key_slot( slot );

    return( ( status == PSA_SUCCESS ) ? unlock_status : status );
}



/****************************************************************/
/* Symmetric cryptography */
/****************************************************************/

static psa_status_t psa_cipher_setup( psa_cipher_operation_t *operation,
                                      mbedtls_svc_key_id_t key,
                                      psa_algorithm_t alg,
                                      mbedtls_operation_t cipher_operation )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    int ret = 0;
    psa_key_slot_t *slot;
    size_t key_bits;
    const mbedtls_cipher_info_t *cipher_info = NULL;
    psa_key_usage_t usage = ( cipher_operation == MBEDTLS_ENCRYPT ?
                              PSA_KEY_USAGE_ENCRYPT :
                              PSA_KEY_USAGE_DECRYPT );

    /* A context must be freshly initialized before it can be set up. */
    if( operation->alg != 0 )
        return( PSA_ERROR_BAD_STATE );

    /* The requested algorithm must be one that can be processed by cipher. */
    if( ! PSA_ALG_IS_CIPHER( alg ) )
        return( PSA_ERROR_INVALID_ARGUMENT );

    /* Fetch key material from key storage. */
    status = psa_get_and_lock_key_slot_with_policy( key, &slot, usage, alg );
    if( status != PSA_SUCCESS )
        goto exit;

    /* Initialize the operation struct members, except for alg. The alg member
     * is used to indicate to psa_cipher_abort that there are resources to free,
     * so we only set it after resources have been allocated/initialized. */
    operation->key_set = 0;
    operation->iv_set = 0;
    operation->mbedtls_in_use = 0;
    operation->iv_size = 0;
    operation->block_size = 0;
    if( alg == PSA_ALG_ECB_NO_PADDING )
        operation->iv_required = 0;
    else
        operation->iv_required = 1;

    /* Try doing the operation through a driver before using software fallback. */
    if( cipher_operation == MBEDTLS_ENCRYPT )
        status = psa_driver_wrapper_cipher_encrypt_setup( &operation->ctx.driver,
                                                          slot,
                                                          alg );
    else
        status = psa_driver_wrapper_cipher_decrypt_setup( &operation->ctx.driver,
                                                          slot,
                                                          alg );

    if( status == PSA_SUCCESS )
    {
        /* Once the driver context is initialised, it needs to be freed using
        * psa_cipher_abort. Indicate this through setting alg. */
        operation->alg = alg;
    }

    if( status != PSA_ERROR_NOT_SUPPORTED ||
        psa_key_lifetime_is_external( slot->attr.lifetime ) )
        goto exit;

    /* Proceed with initializing an mbed TLS cipher context if no driver is
     * available for the given algorithm & key. */
    mbedtls_cipher_init( &operation->ctx.cipher );

    /* Once the cipher context is initialised, it needs to be freed using
     * psa_cipher_abort. Indicate there is something to be freed through setting
     * alg, and indicate the operation is being done using mbedtls crypto through
     * setting mbedtls_in_use. */
    operation->alg = alg;
    operation->mbedtls_in_use = 1;

    key_bits = psa_get_key_slot_bits( slot );
    cipher_info = mbedtls_cipher_info_from_psa( alg, slot->attr.type, key_bits, NULL );
    if( cipher_info == NULL )
    {
        status = PSA_ERROR_NOT_SUPPORTED;
        goto exit;
    }

    ret = mbedtls_cipher_setup( &operation->ctx.cipher, cipher_info );
    if( ret != 0 )
        goto exit;

#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_DES)
    if( slot->attr.type == PSA_KEY_TYPE_DES && key_bits == 128 )
    {
        /* Two-key Triple-DES is 3-key Triple-DES with K1=K3 */
        uint8_t keys[24];
        memcpy( keys, slot->key.data, 16 );
        memcpy( keys + 16, slot->key.data, 8 );
        ret = mbedtls_cipher_setkey( &operation->ctx.cipher,
                                     keys,
                                     192, cipher_operation );
    }
    else
#endif
    {
        ret = mbedtls_cipher_setkey( &operation->ctx.cipher,
                                     slot->key.data,
                                     (int) key_bits, cipher_operation );
    }
    if( ret != 0 )
        goto exit;

#if defined(MBEDTLS_PSA_BUILTIN_ALG_CBC_NO_PADDING) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_CBC_PKCS7)
    switch( alg )
    {
        case PSA_ALG_CBC_NO_PADDING:
            ret = mbedtls_cipher_set_padding_mode( &operation->ctx.cipher,
                                                   MBEDTLS_PADDING_NONE );
            break;
        case PSA_ALG_CBC_PKCS7:
            ret = mbedtls_cipher_set_padding_mode( &operation->ctx.cipher,
                                                   MBEDTLS_PADDING_PKCS7 );
            break;
        default:
            /* The algorithm doesn't involve padding. */
            ret = 0;
            break;
    }
    if( ret != 0 )
        goto exit;
#endif /* MBEDTLS_PSA_BUILTIN_ALG_CBC_NO_PADDING || MBEDTLS_PSA_BUILTIN_ALG_CBC_PKCS7 */

    operation->block_size = ( PSA_ALG_IS_STREAM_CIPHER( alg ) ? 1 :
                              PSA_BLOCK_CIPHER_BLOCK_LENGTH( slot->attr.type ) );
    if( ( alg & PSA_ALG_CIPHER_FROM_BLOCK_FLAG ) != 0 &&
        alg != PSA_ALG_ECB_NO_PADDING )
    {
        operation->iv_size = PSA_BLOCK_CIPHER_BLOCK_LENGTH( slot->attr.type );
    }
#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_CHACHA20)
    else
    if( alg == PSA_ALG_STREAM_CIPHER && slot->attr.type == PSA_KEY_TYPE_CHACHA20 )
        operation->iv_size = 12;
#endif

    status = PSA_SUCCESS;

exit:
    if( ret != 0 )
        status = mbedtls_to_psa_error( ret );
    if( status == PSA_SUCCESS )
    {
        /* Update operation flags for both driver and software implementations */
        operation->key_set = 1;
    }
    else
        psa_cipher_abort( operation );

    unlock_status = psa_unlock_key_slot( slot );

    return( ( status == PSA_SUCCESS ) ? unlock_status : status );
}

psa_status_t psa_cipher_encrypt_setup( psa_cipher_operation_t *operation,
                                       mbedtls_svc_key_id_t key,
                                       psa_algorithm_t alg )
{
    return( psa_cipher_setup( operation, key, alg, MBEDTLS_ENCRYPT ) );
}

psa_status_t psa_cipher_decrypt_setup( psa_cipher_operation_t *operation,
                                       mbedtls_svc_key_id_t key,
                                       psa_algorithm_t alg )
{
    return( psa_cipher_setup( operation, key, alg, MBEDTLS_DECRYPT ) );
}

psa_status_t psa_cipher_generate_iv( psa_cipher_operation_t *operation,
                                     uint8_t *iv,
                                     size_t iv_size,
                                     size_t *iv_length )
{
    psa_status_t status;
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    if( operation->iv_set || ! operation->iv_required )
    {
        return( PSA_ERROR_BAD_STATE );
    }

    if( operation->mbedtls_in_use == 0 )
    {
        status = psa_driver_wrapper_cipher_generate_iv( &operation->ctx.driver,
                                                        iv,
                                                        iv_size,
                                                        iv_length );
        goto exit;
    }

    if( iv_size < operation->iv_size )
    {
        status = PSA_ERROR_BUFFER_TOO_SMALL;
        goto exit;
    }
    ret = mbedtls_psa_get_random( MBEDTLS_PSA_RANDOM_STATE,
                                  iv, operation->iv_size );
    if( ret != 0 )
    {
        status = mbedtls_to_psa_error( ret );
        goto exit;
    }

    *iv_length = operation->iv_size;
    status = psa_cipher_set_iv( operation, iv, *iv_length );

exit:
    if( status == PSA_SUCCESS )
        operation->iv_set = 1;
    else
        psa_cipher_abort( operation );
    return( status );
}

psa_status_t psa_cipher_set_iv( psa_cipher_operation_t *operation,
                                const uint8_t *iv,
                                size_t iv_length )
{
    psa_status_t status;
    int ret = MBEDTLS_ERR_ERROR_CORRUPTION_DETECTED;
    if( operation->iv_set || ! operation->iv_required )
    {
        return( PSA_ERROR_BAD_STATE );
    }

    if( operation->mbedtls_in_use == 0 )
    {
        status = psa_driver_wrapper_cipher_set_iv( &operation->ctx.driver,
                                                   iv,
                                                   iv_length );
        goto exit;
    }

    if( iv_length != operation->iv_size )
    {
        status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }
    ret = mbedtls_cipher_set_iv( &operation->ctx.cipher, iv, iv_length );
    status = mbedtls_to_psa_error( ret );
exit:
    if( status == PSA_SUCCESS )
        operation->iv_set = 1;
    else
        psa_cipher_abort( operation );
    return( status );
}

/* Process input for which the algorithm is set to ECB mode. This requires
 * manual processing, since the PSA API is defined as being able to process
 * arbitrary-length calls to psa_cipher_update() with ECB mode, but the
 * underlying mbedtls_cipher_update only takes full blocks. */
static psa_status_t psa_cipher_update_ecb_internal(
    mbedtls_cipher_context_t *ctx,
    const uint8_t *input,
    size_t input_length,
    uint8_t *output,
    size_t output_size,
    size_t *output_length )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    size_t block_size = ctx->cipher_info->block_size;
    size_t internal_output_length = 0;
    *output_length = 0;

    if( input_length == 0 )
    {
        status = PSA_SUCCESS;
        goto exit;
    }

    if( ctx->unprocessed_len > 0 )
    {
        /* Fill up to block size, and run the block if there's a full one. */
        size_t bytes_to_copy = block_size - ctx->unprocessed_len;

        if( input_length < bytes_to_copy )
            bytes_to_copy = input_length;

        memcpy( &( ctx->unprocessed_data[ctx->unprocessed_len] ),
                input, bytes_to_copy );
        input_length -= bytes_to_copy;
        input += bytes_to_copy;
        ctx->unprocessed_len += bytes_to_copy;

        if( ctx->unprocessed_len == block_size )
        {
            status = mbedtls_to_psa_error(
                mbedtls_cipher_update( ctx,
                                       ctx->unprocessed_data,
                                       block_size,
                                       output, &internal_output_length ) );

            if( status != PSA_SUCCESS )
                goto exit;

            output += internal_output_length;
            output_size -= internal_output_length;
            *output_length += internal_output_length;
            ctx->unprocessed_len = 0;
        }
    }

    while( input_length >= block_size )
    {
        /* Run all full blocks we have, one by one */
        status = mbedtls_to_psa_error(
            mbedtls_cipher_update( ctx, input,
                                   block_size,
                                   output, &internal_output_length ) );

        if( status != PSA_SUCCESS )
            goto exit;

        input_length -= block_size;
        input += block_size;

        output += internal_output_length;
        output_size -= internal_output_length;
        *output_length += internal_output_length;
    }

    if( input_length > 0 )
    {
        /* Save unprocessed bytes for later processing */
        memcpy( &( ctx->unprocessed_data[ctx->unprocessed_len] ),
                input, input_length );
        ctx->unprocessed_len += input_length;
    }

    status = PSA_SUCCESS;

exit:
    return( status );
}

psa_status_t psa_cipher_update( psa_cipher_operation_t *operation,
                                const uint8_t *input,
                                size_t input_length,
                                uint8_t *output,
                                size_t output_size,
                                size_t *output_length )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    size_t expected_output_size;
    if( operation->alg == 0 )
    {
        return( PSA_ERROR_BAD_STATE );
    }
    if( operation->iv_required && ! operation->iv_set )
    {
        return( PSA_ERROR_BAD_STATE );
    }

    if( operation->mbedtls_in_use == 0 )
    {
        status = psa_driver_wrapper_cipher_update( &operation->ctx.driver,
                                                   input,
                                                   input_length,
                                                   output,
                                                   output_size,
                                                   output_length );
        goto exit;
    }

    if( ! PSA_ALG_IS_STREAM_CIPHER( operation->alg ) )
    {
        /* Take the unprocessed partial block left over from previous
         * update calls, if any, plus the input to this call. Remove
         * the last partial block, if any. You get the data that will be
         * output in this call. */
        expected_output_size =
            ( operation->ctx.cipher.unprocessed_len + input_length )
            / operation->block_size * operation->block_size;
    }
    else
    {
        expected_output_size = input_length;
    }

    if( output_size < expected_output_size )
    {
        status = PSA_ERROR_BUFFER_TOO_SMALL;
        goto exit;
    }

    if( operation->alg == PSA_ALG_ECB_NO_PADDING )
    {
        /* mbedtls_cipher_update has an API inconsistency: it will only
        * process a single block at a time in ECB mode. Abstract away that
        * inconsistency here to match the PSA API behaviour. */
        status = psa_cipher_update_ecb_internal( &operation->ctx.cipher,
                                                 input,
                                                 input_length,
                                                 output,
                                                 output_size,
                                                 output_length );
    }
    else
    {
        status = mbedtls_to_psa_error(
            mbedtls_cipher_update( &operation->ctx.cipher, input,
                                   input_length, output, output_length ) );
    }
exit:
    if( status != PSA_SUCCESS )
        psa_cipher_abort( operation );
    return( status );
}

psa_status_t psa_cipher_finish( psa_cipher_operation_t *operation,
                                uint8_t *output,
                                size_t output_size,
                                size_t *output_length )
{
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;
    uint8_t temp_output_buffer[MBEDTLS_MAX_BLOCK_LENGTH];
    if( operation->alg == 0 )
    {
        return( PSA_ERROR_BAD_STATE );
    }
    if( operation->iv_required && ! operation->iv_set )
    {
        return( PSA_ERROR_BAD_STATE );
    }

    if( operation->mbedtls_in_use == 0 )
    {
        status = psa_driver_wrapper_cipher_finish( &operation->ctx.driver,
                                                   output,
                                                   output_size,
                                                   output_length );
        goto exit;
    }

    if( operation->ctx.cipher.unprocessed_len != 0 )
    {
        if( operation->alg == PSA_ALG_ECB_NO_PADDING ||
            operation->alg == PSA_ALG_CBC_NO_PADDING )
        {
            status = PSA_ERROR_INVALID_ARGUMENT;
            goto exit;
        }
    }

    status = mbedtls_to_psa_error(
        mbedtls_cipher_finish( &operation->ctx.cipher,
                               temp_output_buffer,
                               output_length ) );
    if( status != PSA_SUCCESS )
        goto exit;

    if( *output_length == 0 )
        ; /* Nothing to copy. Note that output may be NULL in this case. */
    else if( output_size >= *output_length )
        memcpy( output, temp_output_buffer, *output_length );
    else
        status = PSA_ERROR_BUFFER_TOO_SMALL;

exit:
    if( operation->mbedtls_in_use == 1 )
        mbedtls_platform_zeroize( temp_output_buffer, sizeof( temp_output_buffer ) );

    if( status == PSA_SUCCESS )
        return( psa_cipher_abort( operation ) );
    else
    {
        *output_length = 0;
        (void) psa_cipher_abort( operation );

        return( status );
    }
}

psa_status_t psa_cipher_abort( psa_cipher_operation_t *operation )
{
    if( operation->alg == 0 )
    {
        /* The object has (apparently) been initialized but it is not (yet)
         * in use. It's ok to call abort on such an object, and there's
         * nothing to do. */
        return( PSA_SUCCESS );
    }

    /* Sanity check (shouldn't happen: operation->alg should
     * always have been initialized to a valid value). */
    if( ! PSA_ALG_IS_CIPHER( operation->alg ) )
        return( PSA_ERROR_BAD_STATE );

    if( operation->mbedtls_in_use == 0 )
        psa_driver_wrapper_cipher_abort( &operation->ctx.driver );
    else
        mbedtls_cipher_free( &operation->ctx.cipher );

    operation->alg = 0;
    operation->key_set = 0;
    operation->iv_set = 0;
    operation->mbedtls_in_use = 0;
    operation->iv_size = 0;
    operation->block_size = 0;
    operation->iv_required = 0;

    return( PSA_SUCCESS );
}




/****************************************************************/
/* AEAD */
/****************************************************************/

typedef struct
{
    psa_key_slot_t *slot;
    const mbedtls_cipher_info_t *cipher_info;
    union
    {
        unsigned dummy; /* Make the union non-empty even with no supported algorithms. */
#if defined(MBEDTLS_CCM_C)
        mbedtls_ccm_context ccm;
#endif /* MBEDTLS_CCM_C */
#if defined(MBEDTLS_GCM_C)
        mbedtls_gcm_context gcm;
#endif /* MBEDTLS_GCM_C */
#if defined(MBEDTLS_CHACHAPOLY_C)
        mbedtls_chachapoly_context chachapoly;
#endif /* MBEDTLS_CHACHAPOLY_C */
    } ctx;
    psa_algorithm_t core_alg;
    uint8_t full_tag_length;
    uint8_t tag_length;
} aead_operation_t;

#define AEAD_OPERATION_INIT {0, 0, {0}, 0, 0, 0}

static void psa_aead_abort_internal( aead_operation_t *operation )
{
    switch( operation->core_alg )
    {
#if defined(MBEDTLS_CCM_C)
        case PSA_ALG_CCM:
            mbedtls_ccm_free( &operation->ctx.ccm );
            break;
#endif /* MBEDTLS_CCM_C */
#if defined(MBEDTLS_GCM_C)
        case PSA_ALG_GCM:
            mbedtls_gcm_free( &operation->ctx.gcm );
            break;
#endif /* MBEDTLS_GCM_C */
    }

    psa_unlock_key_slot( operation->slot );
}

static psa_status_t psa_aead_setup( aead_operation_t *operation,
                                    mbedtls_svc_key_id_t key,
                                    psa_key_usage_t usage,
                                    psa_algorithm_t alg )
{
    psa_status_t status;
    size_t key_bits;
    mbedtls_cipher_id_t cipher_id;

    status = psa_get_and_lock_transparent_key_slot_with_policy(
                 key, &operation->slot, usage, alg );
    if( status != PSA_SUCCESS )
        return( status );

    key_bits = psa_get_key_slot_bits( operation->slot );

    operation->cipher_info =
        mbedtls_cipher_info_from_psa( alg, operation->slot->attr.type, key_bits,
                                      &cipher_id );
    if( operation->cipher_info == NULL )
    {
        status = PSA_ERROR_NOT_SUPPORTED;
        goto cleanup;
    }

    switch( PSA_ALG_AEAD_WITH_SHORTENED_TAG( alg, 0 ) )
    {
#if defined(MBEDTLS_CCM_C)
        case PSA_ALG_AEAD_WITH_SHORTENED_TAG( PSA_ALG_CCM, 0 ):
            operation->core_alg = PSA_ALG_CCM;
            operation->full_tag_length = 16;
            /* CCM allows the following tag lengths: 4, 6, 8, 10, 12, 14, 16.
             * The call to mbedtls_ccm_encrypt_and_tag or
             * mbedtls_ccm_auth_decrypt will validate the tag length. */
            if( PSA_BLOCK_CIPHER_BLOCK_LENGTH( operation->slot->attr.type ) != 16 )
            {
                status = PSA_ERROR_INVALID_ARGUMENT;
                goto cleanup;
            }
            mbedtls_ccm_init( &operation->ctx.ccm );
            status = mbedtls_to_psa_error(
                mbedtls_ccm_setkey( &operation->ctx.ccm, cipher_id,
                                    operation->slot->key.data,
                                    (unsigned int) key_bits ) );
            if( status != 0 )
                goto cleanup;
            break;
#endif /* MBEDTLS_CCM_C */

#if defined(MBEDTLS_GCM_C)
        case PSA_ALG_AEAD_WITH_SHORTENED_TAG( PSA_ALG_GCM, 0 ):
            operation->core_alg = PSA_ALG_GCM;
            operation->full_tag_length = 16;
            /* GCM allows the following tag lengths: 4, 8, 12, 13, 14, 15, 16.
             * The call to mbedtls_gcm_crypt_and_tag or
             * mbedtls_gcm_auth_decrypt will validate the tag length. */
            if( PSA_BLOCK_CIPHER_BLOCK_LENGTH( operation->slot->attr.type ) != 16 )
            {
                status = PSA_ERROR_INVALID_ARGUMENT;
                goto cleanup;
            }
            mbedtls_gcm_init( &operation->ctx.gcm );
            status = mbedtls_to_psa_error(
                mbedtls_gcm_setkey( &operation->ctx.gcm, cipher_id,
                                    operation->slot->key.data,
                                    (unsigned int) key_bits ) );
            if( status != 0 )
                goto cleanup;
            break;
#endif /* MBEDTLS_GCM_C */

#if defined(MBEDTLS_CHACHAPOLY_C)
        case PSA_ALG_AEAD_WITH_SHORTENED_TAG( PSA_ALG_CHACHA20_POLY1305, 0 ):
            operation->core_alg = PSA_ALG_CHACHA20_POLY1305;
            operation->full_tag_length = 16;
            /* We only support the default tag length. */
            if( alg != PSA_ALG_CHACHA20_POLY1305 )
            {
                status = PSA_ERROR_NOT_SUPPORTED;
                goto cleanup;
            }
            mbedtls_chachapoly_init( &operation->ctx.chachapoly );
            status = mbedtls_to_psa_error(
                mbedtls_chachapoly_setkey( &operation->ctx.chachapoly,
                                           operation->slot->key.data ) );
            if( status != 0 )
                goto cleanup;
            break;
#endif /* MBEDTLS_CHACHAPOLY_C */

        default:
            status = PSA_ERROR_NOT_SUPPORTED;
            goto cleanup;
    }

    if( PSA_AEAD_TAG_LENGTH( alg ) > operation->full_tag_length )
    {
        status = PSA_ERROR_INVALID_ARGUMENT;
        goto cleanup;
    }
    operation->tag_length = PSA_AEAD_TAG_LENGTH( alg );

    return( PSA_SUCCESS );

cleanup:
    psa_aead_abort_internal( operation );
    return( status );
}

psa_status_t psa_aead_encrypt( mbedtls_svc_key_id_t key,
                               psa_algorithm_t alg,
                               const uint8_t *nonce,
                               size_t nonce_length,
                               const uint8_t *additional_data,
                               size_t additional_data_length,
                               const uint8_t *plaintext,
                               size_t plaintext_length,
                               uint8_t *ciphertext,
                               size_t ciphertext_size,
                               size_t *ciphertext_length )
{
    psa_status_t status;
    aead_operation_t operation = AEAD_OPERATION_INIT;
    uint8_t *tag;

    *ciphertext_length = 0;

    status = psa_aead_setup( &operation, key, PSA_KEY_USAGE_ENCRYPT, alg );
    if( status != PSA_SUCCESS )
        return( status );

    /* For all currently supported modes, the tag is at the end of the
     * ciphertext. */
    if( ciphertext_size < ( plaintext_length + operation.tag_length ) )
    {
        status = PSA_ERROR_BUFFER_TOO_SMALL;
        goto exit;
    }
    tag = ciphertext + plaintext_length;

#if defined(MBEDTLS_GCM_C)
    if( operation.core_alg == PSA_ALG_GCM )
    {
        status = mbedtls_to_psa_error(
            mbedtls_gcm_crypt_and_tag( &operation.ctx.gcm,
                                       MBEDTLS_GCM_ENCRYPT,
                                       plaintext_length,
                                       nonce, nonce_length,
                                       additional_data, additional_data_length,
                                       plaintext, ciphertext,
                                       operation.tag_length, tag ) );
    }
    else
#endif /* MBEDTLS_GCM_C */
#if defined(MBEDTLS_CCM_C)
    if( operation.core_alg == PSA_ALG_CCM )
    {
        status = mbedtls_to_psa_error(
            mbedtls_ccm_encrypt_and_tag( &operation.ctx.ccm,
                                         plaintext_length,
                                         nonce, nonce_length,
                                         additional_data,
                                         additional_data_length,
                                         plaintext, ciphertext,
                                         tag, operation.tag_length ) );
    }
    else
#endif /* MBEDTLS_CCM_C */
#if defined(MBEDTLS_CHACHAPOLY_C)
    if( operation.core_alg == PSA_ALG_CHACHA20_POLY1305 )
    {
        if( nonce_length != 12 || operation.tag_length != 16 )
        {
            status = PSA_ERROR_NOT_SUPPORTED;
            goto exit;
        }
        status = mbedtls_to_psa_error(
            mbedtls_chachapoly_encrypt_and_tag( &operation.ctx.chachapoly,
                                                plaintext_length,
                                                nonce,
                                                additional_data,
                                                additional_data_length,
                                                plaintext,
                                                ciphertext,
                                                tag ) );
    }
    else
#endif /* MBEDTLS_CHACHAPOLY_C */
    {
        (void) tag;
        return( PSA_ERROR_NOT_SUPPORTED );
    }

    if( status != PSA_SUCCESS && ciphertext_size != 0 )
        memset( ciphertext, 0, ciphertext_size );

exit:
    psa_aead_abort_internal( &operation );
    if( status == PSA_SUCCESS )
        *ciphertext_length = plaintext_length + operation.tag_length;
    return( status );
}

/* Locate the tag in a ciphertext buffer containing the encrypted data
 * followed by the tag. Return the length of the part preceding the tag in
 * *plaintext_length. This is the size of the plaintext in modes where
 * the encrypted data has the same size as the plaintext, such as
 * CCM and GCM. */
static psa_status_t psa_aead_unpadded_locate_tag( size_t tag_length,
                                                  const uint8_t *ciphertext,
                                                  size_t ciphertext_length,
                                                  size_t plaintext_size,
                                                  const uint8_t **p_tag )
{
    size_t payload_length;
    if( tag_length > ciphertext_length )
        return( PSA_ERROR_INVALID_ARGUMENT );
    payload_length = ciphertext_length - tag_length;
    if( payload_length > plaintext_size )
        return( PSA_ERROR_BUFFER_TOO_SMALL );
    *p_tag = ciphertext + payload_length;
    return( PSA_SUCCESS );
}

psa_status_t psa_aead_decrypt( mbedtls_svc_key_id_t key,
                               psa_algorithm_t alg,
                               const uint8_t *nonce,
                               size_t nonce_length,
                               const uint8_t *additional_data,
                               size_t additional_data_length,
                               const uint8_t *ciphertext,
                               size_t ciphertext_length,
                               uint8_t *plaintext,
                               size_t plaintext_size,
                               size_t *plaintext_length )
{
    psa_status_t status;
    aead_operation_t operation = AEAD_OPERATION_INIT;
    const uint8_t *tag = NULL;

    *plaintext_length = 0;

    status = psa_aead_setup( &operation, key, PSA_KEY_USAGE_DECRYPT, alg );
    if( status != PSA_SUCCESS )
        return( status );

    status = psa_aead_unpadded_locate_tag( operation.tag_length,
                                           ciphertext, ciphertext_length,
                                           plaintext_size, &tag );
    if( status != PSA_SUCCESS )
        goto exit;

#if defined(MBEDTLS_GCM_C)
    if( operation.core_alg == PSA_ALG_GCM )
    {
        status = mbedtls_to_psa_error(
            mbedtls_gcm_auth_decrypt( &operation.ctx.gcm,
                                      ciphertext_length - operation.tag_length,
                                      nonce, nonce_length,
                                      additional_data,
                                      additional_data_length,
                                      tag, operation.tag_length,
                                      ciphertext, plaintext ) );
    }
    else
#endif /* MBEDTLS_GCM_C */
#if defined(MBEDTLS_CCM_C)
    if( operation.core_alg == PSA_ALG_CCM )
    {
        status = mbedtls_to_psa_error(
            mbedtls_ccm_auth_decrypt( &operation.ctx.ccm,
                                      ciphertext_length - operation.tag_length,
                                      nonce, nonce_length,
                                      additional_data,
                                      additional_data_length,
                                      ciphertext, plaintext,
                                      tag, operation.tag_length ) );
    }
    else
#endif /* MBEDTLS_CCM_C */
#if defined(MBEDTLS_CHACHAPOLY_C)
    if( operation.core_alg == PSA_ALG_CHACHA20_POLY1305 )
    {
        if( nonce_length != 12 || operation.tag_length != 16 )
        {
            status = PSA_ERROR_NOT_SUPPORTED;
            goto exit;
        }
        status = mbedtls_to_psa_error(
            mbedtls_chachapoly_auth_decrypt( &operation.ctx.chachapoly,
                                             ciphertext_length - operation.tag_length,
                                             nonce,
                                             additional_data,
                                             additional_data_length,
                                             tag,
                                             ciphertext,
                                             plaintext ) );
    }
    else
#endif /* MBEDTLS_CHACHAPOLY_C */
    {
        return( PSA_ERROR_NOT_SUPPORTED );
    }

    if( status != PSA_SUCCESS && plaintext_size != 0 )
        memset( plaintext, 0, plaintext_size );

exit:
    psa_aead_abort_internal( &operation );
    if( status == PSA_SUCCESS )
        *plaintext_length = ciphertext_length - operation.tag_length;
    return( status );
}



/****************************************************************/
/* Generators */
/****************************************************************/

#if defined(MBEDTLS_PSA_BUILTIN_ALG_HKDF) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_TLS12_PRF) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_TLS12_PSK_TO_MS)
#define AT_LEAST_ONE_BUILTIN_KDF
#endif

#define HKDF_STATE_INIT 0 /* no input yet */
#define HKDF_STATE_STARTED 1 /* got salt */
#define HKDF_STATE_KEYED 2 /* got key */
#define HKDF_STATE_OUTPUT 3 /* output started */

static psa_algorithm_t psa_key_derivation_get_kdf_alg(
    const psa_key_derivation_operation_t *operation )
{
    if ( PSA_ALG_IS_KEY_AGREEMENT( operation->alg ) )
        return( PSA_ALG_KEY_AGREEMENT_GET_KDF( operation->alg ) );
    else
        return( operation->alg );
}

psa_status_t psa_key_derivation_abort( psa_key_derivation_operation_t *operation )
{
    psa_status_t status = PSA_SUCCESS;
    psa_algorithm_t kdf_alg = psa_key_derivation_get_kdf_alg( operation );
    if( kdf_alg == 0 )
    {
        /* The object has (apparently) been initialized but it is not
         * in use. It's ok to call abort on such an object, and there's
         * nothing to do. */
    }
    else
#if defined(MBEDTLS_PSA_BUILTIN_ALG_HKDF)
    if( PSA_ALG_IS_HKDF( kdf_alg ) )
    {
        mbedtls_free( operation->ctx.hkdf.info );
        status = psa_hmac_abort_internal( &operation->ctx.hkdf.hmac );
    }
    else
#endif /* defined(MBEDTLS_PSA_BUILTIN_ALG_HKDF */
#if defined(MBEDTLS_PSA_BUILTIN_ALG_TLS12_PRF) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_TLS12_PSK_TO_MS)
    if( PSA_ALG_IS_TLS12_PRF( kdf_alg ) ||
             /* TLS-1.2 PSK-to-MS KDF uses the same core as TLS-1.2 PRF */
             PSA_ALG_IS_TLS12_PSK_TO_MS( kdf_alg ) )
    {
        if( operation->ctx.tls12_prf.seed != NULL )
        {
            mbedtls_platform_zeroize( operation->ctx.tls12_prf.seed,
                                      operation->ctx.tls12_prf.seed_length );
            mbedtls_free( operation->ctx.tls12_prf.seed );
        }

        if( operation->ctx.tls12_prf.label != NULL )
        {
            mbedtls_platform_zeroize( operation->ctx.tls12_prf.label,
                                      operation->ctx.tls12_prf.label_length );
            mbedtls_free( operation->ctx.tls12_prf.label );
        }

        status = psa_hmac_abort_internal( &operation->ctx.tls12_prf.hmac );

        /* We leave the fields Ai and output_block to be erased safely by the
         * mbedtls_platform_zeroize() in the end of this function. */
    }
    else
#endif /* defined(MBEDTLS_PSA_BUILTIN_ALG_TLS12_PRF) ||
        * defined(MBEDTLS_PSA_BUILTIN_ALG_TLS12_PSK_TO_MS) */
    {
        status = PSA_ERROR_BAD_STATE;
    }
    mbedtls_platform_zeroize( operation, sizeof( *operation ) );
    return( status );
}

psa_status_t psa_key_derivation_get_capacity(const psa_key_derivation_operation_t *operation,
                                        size_t *capacity)
{
    if( operation->alg == 0 )
    {
        /* This is a blank key derivation operation. */
        return( PSA_ERROR_BAD_STATE );
    }

    *capacity = operation->capacity;
    return( PSA_SUCCESS );
}

psa_status_t psa_key_derivation_set_capacity( psa_key_derivation_operation_t *operation,
                                         size_t capacity )
{
    if( operation->alg == 0 )
        return( PSA_ERROR_BAD_STATE );
    if( capacity > operation->capacity )
        return( PSA_ERROR_INVALID_ARGUMENT );
    operation->capacity = capacity;
    return( PSA_SUCCESS );
}

#if defined(MBEDTLS_PSA_BUILTIN_ALG_HKDF)
/* Read some bytes from an HKDF-based operation. This performs a chunk
 * of the expand phase of the HKDF algorithm. */
static psa_status_t psa_key_derivation_hkdf_read( psa_hkdf_key_derivation_t *hkdf,
                                             psa_algorithm_t hash_alg,
                                             uint8_t *output,
                                             size_t output_length )
{
    uint8_t hash_length = PSA_HASH_LENGTH( hash_alg );
    psa_status_t status;

    if( hkdf->state < HKDF_STATE_KEYED || ! hkdf->info_set )
        return( PSA_ERROR_BAD_STATE );
    hkdf->state = HKDF_STATE_OUTPUT;

    while( output_length != 0 )
    {
        /* Copy what remains of the current block */
        uint8_t n = hash_length - hkdf->offset_in_block;
        if( n > output_length )
            n = (uint8_t) output_length;
        memcpy( output, hkdf->output_block + hkdf->offset_in_block, n );
        output += n;
        output_length -= n;
        hkdf->offset_in_block += n;
        if( output_length == 0 )
            break;
        /* We can't be wanting more output after block 0xff, otherwise
         * the capacity check in psa_key_derivation_output_bytes() would have
         * prevented this call. It could happen only if the operation
         * object was corrupted or if this function is called directly
         * inside the library. */
        if( hkdf->block_number == 0xff )
            return( PSA_ERROR_BAD_STATE );

        /* We need a new block */
        ++hkdf->block_number;
        hkdf->offset_in_block = 0;
        status = psa_hmac_setup_internal( &hkdf->hmac,
                                          hkdf->prk, hash_length,
                                          hash_alg );
        if( status != PSA_SUCCESS )
            return( status );
        if( hkdf->block_number != 1 )
        {
            status = psa_hash_update( &hkdf->hmac.hash_ctx,
                                      hkdf->output_block,
                                      hash_length );
            if( status != PSA_SUCCESS )
                return( status );
        }
        status = psa_hash_update( &hkdf->hmac.hash_ctx,
                                  hkdf->info,
                                  hkdf->info_length );
        if( status != PSA_SUCCESS )
            return( status );
        status = psa_hash_update( &hkdf->hmac.hash_ctx,
                                  &hkdf->block_number, 1 );
        if( status != PSA_SUCCESS )
            return( status );
        status = psa_hmac_finish_internal( &hkdf->hmac,
                                           hkdf->output_block,
                                           sizeof( hkdf->output_block ) );
        if( status != PSA_SUCCESS )
            return( status );
    }

    return( PSA_SUCCESS );
}
#endif /* MBEDTLS_PSA_BUILTIN_ALG_HKDF */

#if defined(MBEDTLS_PSA_BUILTIN_ALG_TLS12_PRF) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_TLS12_PSK_TO_MS)
static psa_status_t psa_key_derivation_tls12_prf_generate_next_block(
    psa_tls12_prf_key_derivation_t *tls12_prf,
    psa_algorithm_t alg )
{
    psa_algorithm_t hash_alg = PSA_ALG_HKDF_GET_HASH( alg );
    uint8_t hash_length = PSA_HASH_LENGTH( hash_alg );
    psa_hash_operation_t backup = PSA_HASH_OPERATION_INIT;
    psa_status_t status, cleanup_status;

    /* We can't be wanting more output after block 0xff, otherwise
     * the capacity check in psa_key_derivation_output_bytes() would have
     * prevented this call. It could happen only if the operation
     * object was corrupted or if this function is called directly
     * inside the library. */
    if( tls12_prf->block_number == 0xff )
        return( PSA_ERROR_CORRUPTION_DETECTED );

    /* We need a new block */
    ++tls12_prf->block_number;
    tls12_prf->left_in_block = hash_length;

    /* Recall the definition of the TLS-1.2-PRF from RFC 5246:
     *
     * PRF(secret, label, seed) = P_<hash>(secret, label + seed)
     *
     * P_hash(secret, seed) = HMAC_hash(secret, A(1) + seed) +
     *                        HMAC_hash(secret, A(2) + seed) +
     *                        HMAC_hash(secret, A(3) + seed) + ...
     *
     * A(0) = seed
     * A(i) = HMAC_hash(secret, A(i-1))
     *
     * The `psa_tls12_prf_key_derivation` structure saves the block
     * `HMAC_hash(secret, A(i) + seed)` from which the output
     * is currently extracted as `output_block` and where i is
     * `block_number`.
     */

    /* Save the hash context before using it, to preserve the hash state with
     * only the inner padding in it. We need this, because inner padding depends
     * on the key (secret in the RFC's terminology). */
    status = psa_hash_clone( &tls12_prf->hmac.hash_ctx, &backup );
    if( status != PSA_SUCCESS )
        goto cleanup;

    /* Calculate A(i) where i = tls12_prf->block_number. */
    if( tls12_prf->block_number == 1 )
    {
        /* A(1) = HMAC_hash(secret, A(0)), where A(0) = seed. (The RFC overloads
         * the variable seed and in this instance means it in the context of the
         * P_hash function, where seed = label + seed.) */
        status = psa_hash_update( &tls12_prf->hmac.hash_ctx,
                                  tls12_prf->label, tls12_prf->label_length );
        if( status != PSA_SUCCESS )
            goto cleanup;
        status = psa_hash_update( &tls12_prf->hmac.hash_ctx,
                                  tls12_prf->seed, tls12_prf->seed_length );
        if( status != PSA_SUCCESS )
            goto cleanup;
    }
    else
    {
        /* A(i) = HMAC_hash(secret, A(i-1)) */
        status = psa_hash_update( &tls12_prf->hmac.hash_ctx,
                                  tls12_prf->Ai, hash_length );
        if( status != PSA_SUCCESS )
            goto cleanup;
    }

    status = psa_hmac_finish_internal( &tls12_prf->hmac,
                                       tls12_prf->Ai, hash_length );
    if( status != PSA_SUCCESS )
        goto cleanup;
    status = psa_hash_clone( &backup, &tls12_prf->hmac.hash_ctx );
    if( status != PSA_SUCCESS )
        goto cleanup;

    /* Calculate HMAC_hash(secret, A(i) + label + seed). */
    status = psa_hash_update( &tls12_prf->hmac.hash_ctx,
                              tls12_prf->Ai, hash_length );
    if( status != PSA_SUCCESS )
        goto cleanup;
    status = psa_hash_update( &tls12_prf->hmac.hash_ctx,
                              tls12_prf->label, tls12_prf->label_length );
    if( status != PSA_SUCCESS )
        goto cleanup;
    status = psa_hash_update( &tls12_prf->hmac.hash_ctx,
                              tls12_prf->seed, tls12_prf->seed_length );
    if( status != PSA_SUCCESS )
        goto cleanup;
    status = psa_hmac_finish_internal( &tls12_prf->hmac,
                                       tls12_prf->output_block, hash_length );
    if( status != PSA_SUCCESS )
        goto cleanup;
    status = psa_hash_clone( &backup, &tls12_prf->hmac.hash_ctx );
    if( status != PSA_SUCCESS )
        goto cleanup;


cleanup:

    cleanup_status = psa_hash_abort( &backup );
    if( status == PSA_SUCCESS && cleanup_status != PSA_SUCCESS )
        status = cleanup_status;

    return( status );
}

static psa_status_t psa_key_derivation_tls12_prf_read(
    psa_tls12_prf_key_derivation_t *tls12_prf,
    psa_algorithm_t alg,
    uint8_t *output,
    size_t output_length )
{
    psa_algorithm_t hash_alg = PSA_ALG_TLS12_PRF_GET_HASH( alg );
    uint8_t hash_length = PSA_HASH_LENGTH( hash_alg );
    psa_status_t status;
    uint8_t offset, length;

    while( output_length != 0 )
    {
        /* Check if we have fully processed the current block. */
        if( tls12_prf->left_in_block == 0 )
        {
            status = psa_key_derivation_tls12_prf_generate_next_block( tls12_prf,
                                                                       alg );
            if( status != PSA_SUCCESS )
                return( status );

            continue;
        }

        if( tls12_prf->left_in_block > output_length )
            length = (uint8_t) output_length;
        else
            length = tls12_prf->left_in_block;

        offset = hash_length - tls12_prf->left_in_block;
        memcpy( output, tls12_prf->output_block + offset, length );
        output += length;
        output_length -= length;
        tls12_prf->left_in_block -= length;
    }

    return( PSA_SUCCESS );
}
#endif /* MBEDTLS_PSA_BUILTIN_ALG_TLS12_PRF ||
        * MBEDTLS_PSA_BUILTIN_ALG_TLS12_PSK_TO_MS */

psa_status_t psa_key_derivation_output_bytes(
    psa_key_derivation_operation_t *operation,
    uint8_t *output,
    size_t output_length )
{
    psa_status_t status;
    psa_algorithm_t kdf_alg = psa_key_derivation_get_kdf_alg( operation );

    if( operation->alg == 0 )
    {
        /* This is a blank operation. */
        return( PSA_ERROR_BAD_STATE );
    }

    if( output_length > operation->capacity )
    {
        operation->capacity = 0;
        /* Go through the error path to wipe all confidential data now
         * that the operation object is useless. */
        status = PSA_ERROR_INSUFFICIENT_DATA;
        goto exit;
    }
    if( output_length == 0 && operation->capacity == 0 )
    {
        /* Edge case: this is a finished operation, and 0 bytes
         * were requested. The right error in this case could
         * be either INSUFFICIENT_CAPACITY or BAD_STATE. Return
         * INSUFFICIENT_CAPACITY, which is right for a finished
         * operation, for consistency with the case when
         * output_length > 0. */
        return( PSA_ERROR_INSUFFICIENT_DATA );
    }
    operation->capacity -= output_length;

#if defined(MBEDTLS_PSA_BUILTIN_ALG_HKDF)
    if( PSA_ALG_IS_HKDF( kdf_alg ) )
    {
        psa_algorithm_t hash_alg = PSA_ALG_HKDF_GET_HASH( kdf_alg );
        status = psa_key_derivation_hkdf_read( &operation->ctx.hkdf, hash_alg,
                                          output, output_length );
    }
    else
#endif /* MBEDTLS_PSA_BUILTIN_ALG_HKDF */
#if defined(MBEDTLS_PSA_BUILTIN_ALG_TLS12_PRF) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_TLS12_PSK_TO_MS)
    if( PSA_ALG_IS_TLS12_PRF( kdf_alg ) ||
        PSA_ALG_IS_TLS12_PSK_TO_MS( kdf_alg ) )
    {
        status = psa_key_derivation_tls12_prf_read( &operation->ctx.tls12_prf,
                                               kdf_alg, output,
                                               output_length );
    }
    else
#endif /* MBEDTLS_PSA_BUILTIN_ALG_TLS12_PRF ||
        * MBEDTLS_PSA_BUILTIN_ALG_TLS12_PSK_TO_MS */
    {
        (void) kdf_alg;
        return( PSA_ERROR_BAD_STATE );
    }

exit:
    if( status != PSA_SUCCESS )
    {
        /* Preserve the algorithm upon errors, but clear all sensitive state.
         * This allows us to differentiate between exhausted operations and
         * blank operations, so we can return PSA_ERROR_BAD_STATE on blank
         * operations. */
        psa_algorithm_t alg = operation->alg;
        psa_key_derivation_abort( operation );
        operation->alg = alg;
        memset( output, '!', output_length );
    }
    return( status );
}

#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_DES)
static void psa_des_set_key_parity( uint8_t *data, size_t data_size )
{
    if( data_size >= 8 )
        mbedtls_des_key_set_parity( data );
    if( data_size >= 16 )
        mbedtls_des_key_set_parity( data + 8 );
    if( data_size >= 24 )
        mbedtls_des_key_set_parity( data + 16 );
}
#endif /* MBEDTLS_PSA_BUILTIN_KEY_TYPE_DES */

static psa_status_t psa_generate_derived_key_internal(
    psa_key_slot_t *slot,
    size_t bits,
    psa_key_derivation_operation_t *operation )
{
    uint8_t *data = NULL;
    size_t bytes = PSA_BITS_TO_BYTES( bits );
    psa_status_t status;

    if( ! key_type_is_raw_bytes( slot->attr.type ) )
        return( PSA_ERROR_INVALID_ARGUMENT );
    if( bits % 8 != 0 )
        return( PSA_ERROR_INVALID_ARGUMENT );
    data = mbedtls_calloc( 1, bytes );
    if( data == NULL )
        return( PSA_ERROR_INSUFFICIENT_MEMORY );

    status = psa_key_derivation_output_bytes( operation, data, bytes );
    if( status != PSA_SUCCESS )
        goto exit;
#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_DES)
    if( slot->attr.type == PSA_KEY_TYPE_DES )
        psa_des_set_key_parity( data, bytes );
#endif /* MBEDTLS_PSA_BUILTIN_KEY_TYPE_DES */

    status = psa_allocate_buffer_to_slot( slot, bytes );
    if( status != PSA_SUCCESS )
        goto exit;

    slot->attr.bits = (psa_key_bits_t) bits;
    psa_key_attributes_t attributes = {
      .core = slot->attr
    };

    status = psa_driver_wrapper_import_key( &attributes,
                                            data, bytes,
                                            slot->key.data,
                                            slot->key.bytes,
                                            &slot->key.bytes, &bits );
    if( bits != slot->attr.bits )
        status = PSA_ERROR_INVALID_ARGUMENT;

exit:
    mbedtls_free( data );
    return( status );
}

psa_status_t psa_key_derivation_output_key( const psa_key_attributes_t *attributes,
                                       psa_key_derivation_operation_t *operation,
                                       mbedtls_svc_key_id_t *key )
{
    psa_status_t status;
    psa_key_slot_t *slot = NULL;
    psa_se_drv_table_entry_t *driver = NULL;

    *key = MBEDTLS_SVC_KEY_ID_INIT;

    /* Reject any attempt to create a zero-length key so that we don't
     * risk tripping up later, e.g. on a malloc(0) that returns NULL. */
    if( psa_get_key_bits( attributes ) == 0 )
        return( PSA_ERROR_INVALID_ARGUMENT );

    if( ! operation->can_output_key )
        return( PSA_ERROR_NOT_PERMITTED );

    status = psa_start_key_creation( PSA_KEY_CREATION_DERIVE, attributes,
                                     &slot, &driver );
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    if( driver != NULL )
    {
        /* Deriving a key in a secure element is not implemented yet. */
        status = PSA_ERROR_NOT_SUPPORTED;
    }
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */
    if( status == PSA_SUCCESS )
    {
        status = psa_generate_derived_key_internal( slot,
                                                    attributes->core.bits,
                                                    operation );
    }
    if( status == PSA_SUCCESS )
        status = psa_finish_key_creation( slot, driver, key );
    if( status != PSA_SUCCESS )
        psa_fail_key_creation( slot, driver );

    return( status );
}



/****************************************************************/
/* Key derivation */
/****************************************************************/

#if defined(AT_LEAST_ONE_BUILTIN_KDF)
static psa_status_t psa_key_derivation_setup_kdf(
    psa_key_derivation_operation_t *operation,
    psa_algorithm_t kdf_alg )
{
    int is_kdf_alg_supported;

    /* Make sure that operation->ctx is properly zero-initialised. (Macro
     * initialisers for this union leave some bytes unspecified.) */
    memset( &operation->ctx, 0, sizeof( operation->ctx ) );

    /* Make sure that kdf_alg is a supported key derivation algorithm. */
#if defined(MBEDTLS_PSA_BUILTIN_ALG_HKDF)
    if( PSA_ALG_IS_HKDF( kdf_alg ) )
        is_kdf_alg_supported = 1;
    else
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_TLS12_PRF)
    if( PSA_ALG_IS_TLS12_PRF( kdf_alg ) )
        is_kdf_alg_supported = 1;
    else
#endif
#if defined(MBEDTLS_PSA_BUILTIN_ALG_TLS12_PSK_TO_MS)
    if( PSA_ALG_IS_TLS12_PSK_TO_MS( kdf_alg ) )
        is_kdf_alg_supported = 1;
    else
#endif
    is_kdf_alg_supported = 0;

    if( is_kdf_alg_supported )
    {
        psa_algorithm_t hash_alg = PSA_ALG_HKDF_GET_HASH( kdf_alg );
        size_t hash_size = PSA_HASH_LENGTH( hash_alg );
        if( hash_size == 0 )
            return( PSA_ERROR_NOT_SUPPORTED );
        if( ( PSA_ALG_IS_TLS12_PRF( kdf_alg ) ||
              PSA_ALG_IS_TLS12_PSK_TO_MS( kdf_alg ) ) &&
            ! ( hash_alg == PSA_ALG_SHA_256 || hash_alg == PSA_ALG_SHA_384 ) )
        {
            return( PSA_ERROR_NOT_SUPPORTED );
        }
        operation->capacity = 255 * hash_size;
        return( PSA_SUCCESS );
    }

    return( PSA_ERROR_NOT_SUPPORTED );
}
#endif /* AT_LEAST_ONE_BUILTIN_KDF */

psa_status_t psa_key_derivation_setup( psa_key_derivation_operation_t *operation,
                                       psa_algorithm_t alg )
{
    psa_status_t status;

    if( operation->alg != 0 )
        return( PSA_ERROR_BAD_STATE );

    if( PSA_ALG_IS_RAW_KEY_AGREEMENT( alg ) )
        return( PSA_ERROR_INVALID_ARGUMENT );
    else if( PSA_ALG_IS_KEY_AGREEMENT( alg ) )
    {
#if defined(AT_LEAST_ONE_BUILTIN_KDF)
        psa_algorithm_t kdf_alg = PSA_ALG_KEY_AGREEMENT_GET_KDF( alg );
        status = psa_key_derivation_setup_kdf( operation, kdf_alg );
#else
        return( PSA_ERROR_NOT_SUPPORTED );
#endif /* AT_LEAST_ONE_BUILTIN_KDF */
    }
    else if( PSA_ALG_IS_KEY_DERIVATION( alg ) )
    {
#if defined(AT_LEAST_ONE_BUILTIN_KDF)
        status = psa_key_derivation_setup_kdf( operation, alg );
#else
        return( PSA_ERROR_NOT_SUPPORTED );
#endif /* AT_LEAST_ONE_BUILTIN_KDF */
    }
    else
        return( PSA_ERROR_INVALID_ARGUMENT );

    if( status == PSA_SUCCESS )
        operation->alg = alg;
    return( status );
}

#if defined(MBEDTLS_PSA_BUILTIN_ALG_HKDF)
static psa_status_t psa_hkdf_input( psa_hkdf_key_derivation_t *hkdf,
                                    psa_algorithm_t hash_alg,
                                    psa_key_derivation_step_t step,
                                    const uint8_t *data,
                                    size_t data_length )
{
    psa_status_t status;
    switch( step )
    {
        case PSA_KEY_DERIVATION_INPUT_SALT:
            if( hkdf->state != HKDF_STATE_INIT )
                return( PSA_ERROR_BAD_STATE );
            status = psa_hmac_setup_internal( &hkdf->hmac,
                                              data, data_length,
                                              hash_alg );
            if( status != PSA_SUCCESS )
                return( status );
            hkdf->state = HKDF_STATE_STARTED;
            return( PSA_SUCCESS );
        case PSA_KEY_DERIVATION_INPUT_SECRET:
            /* If no salt was provided, use an empty salt. */
            if( hkdf->state == HKDF_STATE_INIT )
            {
                status = psa_hmac_setup_internal( &hkdf->hmac,
                                                  NULL, 0,
                                                  hash_alg );
                if( status != PSA_SUCCESS )
                    return( status );
                hkdf->state = HKDF_STATE_STARTED;
            }
            if( hkdf->state != HKDF_STATE_STARTED )
                return( PSA_ERROR_BAD_STATE );
            status = psa_hash_update( &hkdf->hmac.hash_ctx,
                                      data, data_length );
            if( status != PSA_SUCCESS )
                return( status );
            status = psa_hmac_finish_internal( &hkdf->hmac,
                                               hkdf->prk,
                                               sizeof( hkdf->prk ) );
            if( status != PSA_SUCCESS )
                return( status );
            hkdf->offset_in_block = PSA_HASH_LENGTH( hash_alg );
            hkdf->block_number = 0;
            hkdf->state = HKDF_STATE_KEYED;
            return( PSA_SUCCESS );
        case PSA_KEY_DERIVATION_INPUT_INFO:
            if( hkdf->state == HKDF_STATE_OUTPUT )
                return( PSA_ERROR_BAD_STATE );
            if( hkdf->info_set )
                return( PSA_ERROR_BAD_STATE );
            hkdf->info_length = data_length;
            if( data_length != 0 )
            {
                hkdf->info = mbedtls_calloc( 1, data_length );
                if( hkdf->info == NULL )
                    return( PSA_ERROR_INSUFFICIENT_MEMORY );
                memcpy( hkdf->info, data, data_length );
            }
            hkdf->info_set = 1;
            return( PSA_SUCCESS );
        default:
            return( PSA_ERROR_INVALID_ARGUMENT );
    }
}
#endif /* MBEDTLS_PSA_BUILTIN_ALG_HKDF */

#if defined(MBEDTLS_PSA_BUILTIN_ALG_TLS12_PRF) || \
    defined(MBEDTLS_PSA_BUILTIN_ALG_TLS12_PSK_TO_MS)
static psa_status_t psa_tls12_prf_set_seed( psa_tls12_prf_key_derivation_t *prf,
                                            const uint8_t *data,
                                            size_t data_length )
{
    if( prf->state != PSA_TLS12_PRF_STATE_INIT )
        return( PSA_ERROR_BAD_STATE );

    if( data_length != 0 )
    {
        prf->seed = mbedtls_calloc( 1, data_length );
        if( prf->seed == NULL )
            return( PSA_ERROR_INSUFFICIENT_MEMORY );

        memcpy( prf->seed, data, data_length );
        prf->seed_length = data_length;
    }

    prf->state = PSA_TLS12_PRF_STATE_SEED_SET;

    return( PSA_SUCCESS );
}

static psa_status_t psa_tls12_prf_set_key( psa_tls12_prf_key_derivation_t *prf,
                                           psa_algorithm_t hash_alg,
                                           const uint8_t *data,
                                           size_t data_length )
{
    psa_status_t status;
    if( prf->state != PSA_TLS12_PRF_STATE_SEED_SET )
        return( PSA_ERROR_BAD_STATE );

    status = psa_hmac_setup_internal( &prf->hmac, data, data_length, hash_alg );
    if( status != PSA_SUCCESS )
        return( status );

    prf->state = PSA_TLS12_PRF_STATE_KEY_SET;

    return( PSA_SUCCESS );
}

static psa_status_t psa_tls12_prf_set_label( psa_tls12_prf_key_derivation_t *prf,
                                             const uint8_t *data,
                                             size_t data_length )
{
    if( prf->state != PSA_TLS12_PRF_STATE_KEY_SET )
        return( PSA_ERROR_BAD_STATE );

    if( data_length != 0 )
    {
        prf->label = mbedtls_calloc( 1, data_length );
        if( prf->label == NULL )
            return( PSA_ERROR_INSUFFICIENT_MEMORY );

        memcpy( prf->label, data, data_length );
        prf->label_length = data_length;
    }

    prf->state = PSA_TLS12_PRF_STATE_LABEL_SET;

    return( PSA_SUCCESS );
}

static psa_status_t psa_tls12_prf_input( psa_tls12_prf_key_derivation_t *prf,
                                         psa_algorithm_t hash_alg,
                                         psa_key_derivation_step_t step,
                                         const uint8_t *data,
                                         size_t data_length )
{
    switch( step )
    {
        case PSA_KEY_DERIVATION_INPUT_SEED:
            return( psa_tls12_prf_set_seed( prf, data, data_length ) );
        case PSA_KEY_DERIVATION_INPUT_SECRET:
            return( psa_tls12_prf_set_key( prf, hash_alg, data, data_length ) );
        case PSA_KEY_DERIVATION_INPUT_LABEL:
            return( psa_tls12_prf_set_label( prf, data, data_length ) );
        default:
            return( PSA_ERROR_INVALID_ARGUMENT );
    }
}
#endif /* MBEDTLS_PSA_BUILTIN_ALG_TLS12_PRF) ||
        * MBEDTLS_PSA_BUILTIN_ALG_TLS12_PSK_TO_MS */

#if defined(MBEDTLS_PSA_BUILTIN_ALG_TLS12_PSK_TO_MS)
static psa_status_t psa_tls12_prf_psk_to_ms_set_key(
    psa_tls12_prf_key_derivation_t *prf,
    psa_algorithm_t hash_alg,
    const uint8_t *data,
    size_t data_length )
{
    psa_status_t status;
    uint8_t pms[ 4 + 2 * PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE ];
    uint8_t *cur = pms;

    if( data_length > PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE )
        return( PSA_ERROR_INVALID_ARGUMENT );

    /* Quoting RFC 4279, Section 2:
     *
     * The premaster secret is formed as follows: if the PSK is N octets
     * long, concatenate a uint16 with the value N, N zero octets, a second
     * uint16 with the value N, and the PSK itself.
     */

    *cur++ = ( data_length >> 8 ) & 0xff;
    *cur++ = ( data_length >> 0 ) & 0xff;
    memset( cur, 0, data_length );
    cur += data_length;
    *cur++ = pms[0];
    *cur++ = pms[1];
    memcpy( cur, data, data_length );
    cur += data_length;

    status = psa_tls12_prf_set_key( prf, hash_alg, pms, cur - pms );

    mbedtls_platform_zeroize( pms, sizeof( pms ) );
    return( status );
}

static psa_status_t psa_tls12_prf_psk_to_ms_input(
    psa_tls12_prf_key_derivation_t *prf,
    psa_algorithm_t hash_alg,
    psa_key_derivation_step_t step,
    const uint8_t *data,
    size_t data_length )
{
    if( step == PSA_KEY_DERIVATION_INPUT_SECRET )
    {
        return( psa_tls12_prf_psk_to_ms_set_key( prf, hash_alg,
                                                 data, data_length ) );
    }

    return( psa_tls12_prf_input( prf, hash_alg, step, data, data_length ) );
}
#endif /* MBEDTLS_PSA_BUILTIN_ALG_TLS12_PSK_TO_MS */

/** Check whether the given key type is acceptable for the given
 * input step of a key derivation.
 *
 * Secret inputs must have the type #PSA_KEY_TYPE_DERIVE.
 * Non-secret inputs must have the type #PSA_KEY_TYPE_RAW_DATA.
 * Both secret and non-secret inputs can alternatively have the type
 * #PSA_KEY_TYPE_NONE, which is never the type of a key object, meaning
 * that the input was passed as a buffer rather than via a key object.
 */
static int psa_key_derivation_check_input_type(
    psa_key_derivation_step_t step,
    psa_key_type_t key_type )
{
    switch( step )
    {
        case PSA_KEY_DERIVATION_INPUT_SECRET:
            if( key_type == PSA_KEY_TYPE_DERIVE )
                return( PSA_SUCCESS );
            if( key_type == PSA_KEY_TYPE_NONE )
                return( PSA_SUCCESS );
            break;
        case PSA_KEY_DERIVATION_INPUT_LABEL:
        case PSA_KEY_DERIVATION_INPUT_SALT:
        case PSA_KEY_DERIVATION_INPUT_INFO:
        case PSA_KEY_DERIVATION_INPUT_SEED:
            if( key_type == PSA_KEY_TYPE_RAW_DATA )
                return( PSA_SUCCESS );
            if( key_type == PSA_KEY_TYPE_NONE )
                return( PSA_SUCCESS );
            break;
    }
    return( PSA_ERROR_INVALID_ARGUMENT );
}

static psa_status_t psa_key_derivation_input_internal(
    psa_key_derivation_operation_t *operation,
    psa_key_derivation_step_t step,
    psa_key_type_t key_type,
    const uint8_t *data,
    size_t data_length )
{
    psa_status_t status;
    psa_algorithm_t kdf_alg = psa_key_derivation_get_kdf_alg( operation );

    status = psa_key_derivation_check_input_type( step, key_type );
    if( status != PSA_SUCCESS )
        goto exit;

#if defined(MBEDTLS_PSA_BUILTIN_ALG_HKDF)
    if( PSA_ALG_IS_HKDF( kdf_alg ) )
    {
        status = psa_hkdf_input( &operation->ctx.hkdf,
                                 PSA_ALG_HKDF_GET_HASH( kdf_alg ),
                                 step, data, data_length );
    }
    else
#endif /* MBEDTLS_PSA_BUILTIN_ALG_HKDF */
#if defined(MBEDTLS_PSA_BUILTIN_ALG_TLS12_PRF)
    if( PSA_ALG_IS_TLS12_PRF( kdf_alg ) )
    {
        status = psa_tls12_prf_input( &operation->ctx.tls12_prf,
                                      PSA_ALG_HKDF_GET_HASH( kdf_alg ),
                                      step, data, data_length );
    }
    else
#endif /* MBEDTLS_PSA_BUILTIN_ALG_TLS12_PRF */
#if defined(MBEDTLS_PSA_BUILTIN_ALG_TLS12_PSK_TO_MS)
    if( PSA_ALG_IS_TLS12_PSK_TO_MS( kdf_alg ) )
    {
        status = psa_tls12_prf_psk_to_ms_input( &operation->ctx.tls12_prf,
                                                PSA_ALG_HKDF_GET_HASH( kdf_alg ),
                                                step, data, data_length );
    }
    else
#endif /* MBEDTLS_PSA_BUILTIN_ALG_TLS12_PSK_TO_MS */
    {
        /* This can't happen unless the operation object was not initialized */
        (void) data;
        (void) data_length;
        (void) kdf_alg;
        return( PSA_ERROR_BAD_STATE );
    }

exit:
    if( status != PSA_SUCCESS )
        psa_key_derivation_abort( operation );
    return( status );
}

psa_status_t psa_key_derivation_input_bytes(
    psa_key_derivation_operation_t *operation,
    psa_key_derivation_step_t step,
    const uint8_t *data,
    size_t data_length )
{
    return( psa_key_derivation_input_internal( operation, step,
                                               PSA_KEY_TYPE_NONE,
                                               data, data_length ) );
}

psa_status_t psa_key_derivation_input_key(
    psa_key_derivation_operation_t *operation,
    psa_key_derivation_step_t step,
    mbedtls_svc_key_id_t key )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;

    status = psa_get_and_lock_transparent_key_slot_with_policy(
                 key, &slot, PSA_KEY_USAGE_DERIVE, operation->alg );
    if( status != PSA_SUCCESS )
    {
        psa_key_derivation_abort( operation );
        return( status );
    }

    /* Passing a key object as a SECRET input unlocks the permission
     * to output to a key object. */
    if( step == PSA_KEY_DERIVATION_INPUT_SECRET )
        operation->can_output_key = 1;

    status = psa_key_derivation_input_internal( operation,
                                                step, slot->attr.type,
                                                slot->key.data,
                                                slot->key.bytes );

    unlock_status = psa_unlock_key_slot( slot );

    return( ( status == PSA_SUCCESS ) ? unlock_status : status );
}



/****************************************************************/
/* Key agreement */
/****************************************************************/

#if defined(MBEDTLS_PSA_BUILTIN_ALG_ECDH)
static psa_status_t psa_key_agreement_ecdh( const uint8_t *peer_key,
                                            size_t peer_key_length,
                                            const mbedtls_ecp_keypair *our_key,
                                            uint8_t *shared_secret,
                                            size_t shared_secret_size,
                                            size_t *shared_secret_length )
{
    mbedtls_ecp_keypair *their_key = NULL;
    mbedtls_ecdh_context ecdh;
    psa_status_t status;
    size_t bits = 0;
    psa_ecc_family_t curve = mbedtls_ecc_group_to_psa( our_key->grp.id, &bits );
    mbedtls_ecdh_init( &ecdh );

    status = mbedtls_psa_ecp_load_representation(
                 PSA_KEY_TYPE_ECC_PUBLIC_KEY(curve),
                 bits,
                 peer_key,
                 peer_key_length,
                 &their_key );
    if( status != PSA_SUCCESS )
        goto exit;

    status = mbedtls_to_psa_error(
        mbedtls_ecdh_get_params( &ecdh, their_key, MBEDTLS_ECDH_THEIRS ) );
    if( status != PSA_SUCCESS )
        goto exit;
    status = mbedtls_to_psa_error(
        mbedtls_ecdh_get_params( &ecdh, our_key, MBEDTLS_ECDH_OURS ) );
    if( status != PSA_SUCCESS )
        goto exit;

    status = mbedtls_to_psa_error(
        mbedtls_ecdh_calc_secret( &ecdh,
                                  shared_secret_length,
                                  shared_secret, shared_secret_size,
                                  mbedtls_psa_get_random,
                                  MBEDTLS_PSA_RANDOM_STATE ) );
    if( status != PSA_SUCCESS )
        goto exit;
    if( PSA_BITS_TO_BYTES( bits ) != *shared_secret_length )
        status = PSA_ERROR_CORRUPTION_DETECTED;

exit:
    if( status != PSA_SUCCESS )
        mbedtls_platform_zeroize( shared_secret, shared_secret_size );
    mbedtls_ecdh_free( &ecdh );
    mbedtls_ecp_keypair_free( their_key );
    mbedtls_free( their_key );

    return( status );
}
#endif /* MBEDTLS_PSA_BUILTIN_ALG_ECDH */

#define PSA_KEY_AGREEMENT_MAX_SHARED_SECRET_SIZE MBEDTLS_ECP_MAX_BYTES

static psa_status_t psa_key_agreement_raw_internal( psa_algorithm_t alg,
                                                    psa_key_slot_t *private_key,
                                                    const uint8_t *peer_key,
                                                    size_t peer_key_length,
                                                    uint8_t *shared_secret,
                                                    size_t shared_secret_size,
                                                    size_t *shared_secret_length )
{
    switch( alg )
    {
#if defined(MBEDTLS_PSA_BUILTIN_ALG_ECDH)
        case PSA_ALG_ECDH:
            if( ! PSA_KEY_TYPE_IS_ECC_KEY_PAIR( private_key->attr.type ) )
                return( PSA_ERROR_INVALID_ARGUMENT );
            mbedtls_ecp_keypair *ecp = NULL;
            psa_status_t status = mbedtls_psa_ecp_load_representation(
                                      private_key->attr.type,
                                      private_key->attr.bits,
                                      private_key->key.data,
                                      private_key->key.bytes,
                                      &ecp );
            if( status != PSA_SUCCESS )
                return( status );
            status = psa_key_agreement_ecdh( peer_key, peer_key_length,
                                             ecp,
                                             shared_secret, shared_secret_size,
                                             shared_secret_length );
            mbedtls_ecp_keypair_free( ecp );
            mbedtls_free( ecp );
            return( status );
#endif /* MBEDTLS_PSA_BUILTIN_ALG_ECDH */
        default:
            (void) private_key;
            (void) peer_key;
            (void) peer_key_length;
            (void) shared_secret;
            (void) shared_secret_size;
            (void) shared_secret_length;
            return( PSA_ERROR_NOT_SUPPORTED );
    }
}

/* Note that if this function fails, you must call psa_key_derivation_abort()
 * to potentially free embedded data structures and wipe confidential data.
 */
static psa_status_t psa_key_agreement_internal( psa_key_derivation_operation_t *operation,
                                                psa_key_derivation_step_t step,
                                                psa_key_slot_t *private_key,
                                                const uint8_t *peer_key,
                                                size_t peer_key_length )
{
    psa_status_t status;
    uint8_t shared_secret[PSA_KEY_AGREEMENT_MAX_SHARED_SECRET_SIZE];
    size_t shared_secret_length = 0;
    psa_algorithm_t ka_alg = PSA_ALG_KEY_AGREEMENT_GET_BASE( operation->alg );

    /* Step 1: run the secret agreement algorithm to generate the shared
     * secret. */
    status = psa_key_agreement_raw_internal( ka_alg,
                                             private_key,
                                             peer_key, peer_key_length,
                                             shared_secret,
                                             sizeof( shared_secret ),
                                             &shared_secret_length );
    if( status != PSA_SUCCESS )
        goto exit;

    /* Step 2: set up the key derivation to generate key material from
     * the shared secret. A shared secret is permitted wherever a key
     * of type DERIVE is permitted. */
    status = psa_key_derivation_input_internal( operation, step,
                                                PSA_KEY_TYPE_DERIVE,
                                                shared_secret,
                                                shared_secret_length );
exit:
    mbedtls_platform_zeroize( shared_secret, shared_secret_length );
    return( status );
}

psa_status_t psa_key_derivation_key_agreement( psa_key_derivation_operation_t *operation,
                                               psa_key_derivation_step_t step,
                                               mbedtls_svc_key_id_t private_key,
                                               const uint8_t *peer_key,
                                               size_t peer_key_length )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot;

    if( ! PSA_ALG_IS_KEY_AGREEMENT( operation->alg ) )
        return( PSA_ERROR_INVALID_ARGUMENT );
    status = psa_get_and_lock_transparent_key_slot_with_policy(
                 private_key, &slot, PSA_KEY_USAGE_DERIVE, operation->alg );
    if( status != PSA_SUCCESS )
        return( status );
    status = psa_key_agreement_internal( operation, step,
                                         slot,
                                         peer_key, peer_key_length );
    if( status != PSA_SUCCESS )
        psa_key_derivation_abort( operation );
    else
    {
        /* If a private key has been added as SECRET, we allow the derived
         * key material to be used as a key in PSA Crypto. */
        if( step == PSA_KEY_DERIVATION_INPUT_SECRET )
            operation->can_output_key = 1;
    }

    unlock_status = psa_unlock_key_slot( slot );

    return( ( status == PSA_SUCCESS ) ? unlock_status : status );
}

psa_status_t psa_raw_key_agreement( psa_algorithm_t alg,
                                    mbedtls_svc_key_id_t private_key,
                                    const uint8_t *peer_key,
                                    size_t peer_key_length,
                                    uint8_t *output,
                                    size_t output_size,
                                    size_t *output_length )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_status_t unlock_status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_slot_t *slot = NULL;

    if( ! PSA_ALG_IS_KEY_AGREEMENT( alg ) )
    {
        status = PSA_ERROR_INVALID_ARGUMENT;
        goto exit;
    }
    status = psa_get_and_lock_transparent_key_slot_with_policy(
                 private_key, &slot, PSA_KEY_USAGE_DERIVE, alg );
    if( status != PSA_SUCCESS )
        goto exit;

    status = psa_key_agreement_raw_internal( alg, slot,
                                             peer_key, peer_key_length,
                                             output, output_size,
                                             output_length );

exit:
    if( status != PSA_SUCCESS )
    {
        /* If an error happens and is not handled properly, the output
         * may be used as a key to protect sensitive data. Arrange for such
         * a key to be random, which is likely to result in decryption or
         * verification errors. This is better than filling the buffer with
         * some constant data such as zeros, which would result in the data
         * being protected with a reproducible, easily knowable key.
         */
        psa_generate_random( output, output_size );
        *output_length = output_size;
    }

    unlock_status = psa_unlock_key_slot( slot );

    return( ( status == PSA_SUCCESS ) ? unlock_status : status );
}



/****************************************************************/
/* Random generation */
/****************************************************************/

/** Initialize the PSA random generator.
 */
static void mbedtls_psa_random_init( mbedtls_psa_random_context_t *rng )
{
#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
    memset( rng, 0, sizeof( *rng ) );
#else /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */

    /* Set default configuration if
     * mbedtls_psa_crypto_configure_entropy_sources() hasn't been called. */
    if( rng->entropy_init == NULL )
        rng->entropy_init = mbedtls_entropy_init;
    if( rng->entropy_free == NULL )
        rng->entropy_free = mbedtls_entropy_free;

    rng->entropy_init( &rng->entropy );
#if defined(MBEDTLS_PSA_INJECT_ENTROPY) && \
    defined(MBEDTLS_NO_DEFAULT_ENTROPY_SOURCES)
    /* The PSA entropy injection feature depends on using NV seed as an entropy
     * source. Add NV seed as an entropy source for PSA entropy injection. */
    mbedtls_entropy_add_source( &rng->entropy,
                                mbedtls_nv_seed_poll, NULL,
                                MBEDTLS_ENTROPY_BLOCK_SIZE,
                                MBEDTLS_ENTROPY_SOURCE_STRONG );
#endif

    mbedtls_psa_drbg_init( MBEDTLS_PSA_RANDOM_STATE );
#endif /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */
}

/** Deinitialize the PSA random generator.
 */
static void mbedtls_psa_random_free( mbedtls_psa_random_context_t *rng )
{
#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
    memset( rng, 0, sizeof( *rng ) );
#else /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */
    mbedtls_psa_drbg_free( MBEDTLS_PSA_RANDOM_STATE );
    rng->entropy_free( &rng->entropy );
#endif /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */
}

/** Seed the PSA random generator.
 */
static psa_status_t mbedtls_psa_random_seed( mbedtls_psa_random_context_t *rng )
{
#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
    /* Do nothing: the external RNG seeds itself. */
    (void) rng;
    return( PSA_SUCCESS );
#else /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */
    const unsigned char drbg_seed[] = "PSA";
    int ret = mbedtls_psa_drbg_seed( &rng->entropy,
                                     drbg_seed, sizeof( drbg_seed ) - 1 );
    return mbedtls_to_psa_error( ret );
#endif /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */
}

psa_status_t psa_generate_random( uint8_t *output,
                                  size_t output_size )
{
    GUARD_MODULE_INITIALIZED;

#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)

    size_t output_length = 0;
    psa_status_t status = mbedtls_psa_external_get_random( &global_data.rng,
                                                           output, output_size,
                                                           &output_length );
    if( status != PSA_SUCCESS )
        return( status );
    /* Breaking up a request into smaller chunks is currently not supported
     * for the extrernal RNG interface. */
    if( output_length != output_size )
        return( PSA_ERROR_INSUFFICIENT_ENTROPY );
    return( PSA_SUCCESS );

#else /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */

    while( output_size > 0 )
    {
        size_t request_size =
            ( output_size > MBEDTLS_PSA_RANDOM_MAX_REQUEST ?
              MBEDTLS_PSA_RANDOM_MAX_REQUEST :
              output_size );
        int ret = mbedtls_psa_get_random( MBEDTLS_PSA_RANDOM_STATE,
                                          output, request_size );
        if( ret != 0 )
            return( mbedtls_to_psa_error( ret ) );
        output_size -= request_size;
        output += request_size;
    }
    return( PSA_SUCCESS );
#endif /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */
}

/* Wrapper function allowing the classic API to use the PSA RNG.
 *
 * `mbedtls_psa_get_random(MBEDTLS_PSA_RANDOM_STATE, ...)` calls
 * `psa_generate_random(...)`. The state parameter is ignored since the
 * PSA API doesn't support passing an explicit state.
 *
 * In the non-external case, psa_generate_random() calls an
 * `mbedtls_xxx_drbg_random` function which has exactly the same signature
 * and semantics as mbedtls_psa_get_random(). As an optimization,
 * instead of doing this back-and-forth between the PSA API and the
 * classic API, psa_crypto_random_impl.h defines `mbedtls_psa_get_random`
 * as a constant function pointer to `mbedtls_xxx_drbg_random`.
 */
#if defined (MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
int mbedtls_psa_get_random( void *p_rng,
                            unsigned char *output,
                            size_t output_size )
{
    /* This function takes a pointer to the RNG state because that's what
     * classic mbedtls functions using an RNG expect. The PSA RNG manages
     * its own state internally and doesn't let the caller access that state.
     * So we just ignore the state parameter, and in practice we'll pass
     * NULL. */
    (void) p_rng;
    psa_status_t status = psa_generate_random( output, output_size );
    if( status == PSA_SUCCESS )
        return( 0 );
    else
        return( MBEDTLS_ERR_ENTROPY_SOURCE_FAILED );
}
#endif /* MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */

#if defined(MBEDTLS_PSA_INJECT_ENTROPY)
#include "mbedtls/entropy_poll.h"

psa_status_t mbedtls_psa_inject_entropy( const uint8_t *seed,
                                         size_t seed_size )
{
    if( global_data.initialized )
        return( PSA_ERROR_NOT_PERMITTED );

    if( ( ( seed_size < MBEDTLS_ENTROPY_MIN_PLATFORM ) ||
          ( seed_size < MBEDTLS_ENTROPY_BLOCK_SIZE ) ) ||
          ( seed_size > MBEDTLS_ENTROPY_MAX_SEED_SIZE ) )
            return( PSA_ERROR_INVALID_ARGUMENT );

    return( mbedtls_psa_storage_inject_entropy( seed, seed_size ) );
}
#endif /* MBEDTLS_PSA_INJECT_ENTROPY */

/** Validate the key type and size for key generation
 *
 * \param  type  The key type
 * \param  bits  The number of bits of the key
 *
 * \retval #PSA_SUCCESS
 *         The key type and size are valid.
 * \retval #PSA_ERROR_INVALID_ARGUMENT
 *         The size in bits of the key is not valid.
 * \retval #PSA_ERROR_NOT_SUPPORTED
 *         The type and/or the size in bits of the key or the combination of
 *         the two is not supported.
 */
static psa_status_t psa_validate_key_type_and_size_for_key_generation(
    psa_key_type_t type, size_t bits )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;

    if( key_type_is_raw_bytes( type ) )
    {
        status = validate_unstructured_key_bit_size( type, bits );
        if( status != PSA_SUCCESS )
            return( status );
    }
    else
#if defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR)
    if( PSA_KEY_TYPE_IS_RSA( type ) && PSA_KEY_TYPE_IS_KEY_PAIR( type ) )
    {
        if( bits > PSA_VENDOR_RSA_MAX_KEY_BITS )
            return( PSA_ERROR_NOT_SUPPORTED );

        /* Accept only byte-aligned keys, for the same reasons as
         * in psa_import_rsa_key(). */
        if( bits % 8 != 0 )
            return( PSA_ERROR_NOT_SUPPORTED );
    }
    else
#endif /* defined(PSA_WANT_KEY_TYPE_RSA_KEY_PAIR) */

#if defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR)
    if( PSA_KEY_TYPE_IS_ECC( type ) && PSA_KEY_TYPE_IS_KEY_PAIR( type ) )
    {
        /* To avoid empty block, return successfully here. */
        return( PSA_SUCCESS );
    }
    else
#endif /* defined(PSA_WANT_KEY_TYPE_ECC_KEY_PAIR) */
    {
        return( PSA_ERROR_NOT_SUPPORTED );
    }

    return( PSA_SUCCESS );
}

psa_status_t psa_generate_key_internal(
    const psa_key_attributes_t *attributes,
    uint8_t *key_buffer, size_t key_buffer_size, size_t *key_buffer_length )
{
    psa_status_t status = PSA_ERROR_CORRUPTION_DETECTED;
    psa_key_type_t type = attributes->core.type;

    if( ( attributes->domain_parameters == NULL ) &&
        ( attributes->domain_parameters_size != 0 ) )
        return( PSA_ERROR_INVALID_ARGUMENT );

    if( key_type_is_raw_bytes( type ) )
    {
        status = psa_generate_random( key_buffer, key_buffer_size );
        if( status != PSA_SUCCESS )
            return( status );

#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_DES)
        if( type == PSA_KEY_TYPE_DES )
            psa_des_set_key_parity( key_buffer, key_buffer_size );
#endif /* MBEDTLS_PSA_BUILTIN_KEY_TYPE_DES */
    }
    else

#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR)
    if ( type == PSA_KEY_TYPE_RSA_KEY_PAIR )
    {
        return( mbedtls_psa_rsa_generate_key( attributes,
                                              key_buffer,
                                              key_buffer_size,
                                              key_buffer_length ) );
    }
    else
#endif /* defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_RSA_KEY_PAIR) */

#if defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR)
    if ( PSA_KEY_TYPE_IS_ECC( type ) && PSA_KEY_TYPE_IS_KEY_PAIR( type ) )
    {
        return( mbedtls_psa_ecp_generate_key( attributes,
                                              key_buffer,
                                              key_buffer_size,
                                              key_buffer_length ) );
    }
    else
#endif /* defined(MBEDTLS_PSA_BUILTIN_KEY_TYPE_ECC_KEY_PAIR) */
    {
        (void)key_buffer_length;
        return( PSA_ERROR_NOT_SUPPORTED );
    }

    return( PSA_SUCCESS );
}

psa_status_t psa_generate_key( const psa_key_attributes_t *attributes,
                               mbedtls_svc_key_id_t *key )
{
    psa_status_t status;
    psa_key_slot_t *slot = NULL;
    psa_se_drv_table_entry_t *driver = NULL;
    size_t key_buffer_size;

    *key = MBEDTLS_SVC_KEY_ID_INIT;

    /* Reject any attempt to create a zero-length key so that we don't
     * risk tripping up later, e.g. on a malloc(0) that returns NULL. */
    if( psa_get_key_bits( attributes ) == 0 )
        return( PSA_ERROR_INVALID_ARGUMENT );

    status = psa_start_key_creation( PSA_KEY_CREATION_GENERATE, attributes,
                                     &slot, &driver );
    if( status != PSA_SUCCESS )
        goto exit;

    /* In the case of a transparent key or an opaque key stored in local
     * storage (thus not in the case of generating a key in a secure element
     * or cryptoprocessor with storage), we have to allocate a buffer to
     * hold the generated key material. */
    if( slot->key.data == NULL )
    {
        if ( PSA_KEY_LIFETIME_GET_LOCATION( attributes->core.lifetime ) ==
             PSA_KEY_LOCATION_LOCAL_STORAGE )
        {
            status = psa_validate_key_type_and_size_for_key_generation(
                attributes->core.type, attributes->core.bits );
            if( status != PSA_SUCCESS )
                goto exit;

            key_buffer_size = PSA_EXPORT_KEY_OUTPUT_SIZE(
                                  attributes->core.type,
                                  attributes->core.bits );
        }
        else
        {
            status = psa_driver_wrapper_get_key_buffer_size(
                         attributes, &key_buffer_size );
            if( status != PSA_SUCCESS )
                goto exit;
        }

        status = psa_allocate_buffer_to_slot( slot, key_buffer_size );
        if( status != PSA_SUCCESS )
            goto exit;
    }

    status = psa_driver_wrapper_generate_key( attributes,
        slot->key.data, slot->key.bytes, &slot->key.bytes );

    if( status != PSA_SUCCESS )
        psa_remove_key_data_from_memory( slot );

exit:
    if( status == PSA_SUCCESS )
        status = psa_finish_key_creation( slot, driver, key );
    if( status != PSA_SUCCESS )
        psa_fail_key_creation( slot, driver );

    return( status );
}

/****************************************************************/
/* Module setup */
/****************************************************************/

#if !defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
psa_status_t mbedtls_psa_crypto_configure_entropy_sources(
    void (* entropy_init )( mbedtls_entropy_context *ctx ),
    void (* entropy_free )( mbedtls_entropy_context *ctx ) )
{
    if( global_data.rng_state != RNG_NOT_INITIALIZED )
        return( PSA_ERROR_BAD_STATE );
    global_data.rng.entropy_init = entropy_init;
    global_data.rng.entropy_free = entropy_free;
    return( PSA_SUCCESS );
}
#endif /* !defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG) */

void mbedtls_psa_crypto_free( void )
{
    psa_wipe_all_key_slots( );
    if( global_data.rng_state != RNG_NOT_INITIALIZED )
    {
        mbedtls_psa_random_free( &global_data.rng );
    }
    /* Wipe all remaining data, including configuration.
     * In particular, this sets all state indicator to the value
     * indicating "uninitialized". */
    mbedtls_platform_zeroize( &global_data, sizeof( global_data ) );
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    /* Unregister all secure element drivers, so that we restart from
     * a pristine state. */
    psa_unregister_all_se_drivers( );
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */
}

#if defined(PSA_CRYPTO_STORAGE_HAS_TRANSACTIONS)
/** Recover a transaction that was interrupted by a power failure.
 *
 * This function is called during initialization, before psa_crypto_init()
 * returns. If this function returns a failure status, the initialization
 * fails.
 */
static psa_status_t psa_crypto_recover_transaction(
    const psa_crypto_transaction_t *transaction )
{
    switch( transaction->unknown.type )
    {
        case PSA_CRYPTO_TRANSACTION_CREATE_KEY:
        case PSA_CRYPTO_TRANSACTION_DESTROY_KEY:
            /* TODO - fall through to the failure case until this
             * is implemented.
             * https://github.com/ARMmbed/mbed-crypto/issues/218
             */
        default:
            /* We found an unsupported transaction in the storage.
             * We don't know what state the storage is in. Give up. */
            return( PSA_ERROR_DATA_INVALID );
    }
}
#endif /* PSA_CRYPTO_STORAGE_HAS_TRANSACTIONS */

psa_status_t psa_crypto_init( void )
{
    psa_status_t status;

    /* Double initialization is explicitly allowed. */
    if( global_data.initialized != 0 )
        return( PSA_SUCCESS );

    /* Initialize and seed the random generator. */
    mbedtls_psa_random_init( &global_data.rng );
    global_data.rng_state = RNG_INITIALIZED;
    status = mbedtls_psa_random_seed( &global_data.rng );
    if( status != PSA_SUCCESS )
        goto exit;
    global_data.rng_state = RNG_SEEDED;

    status = psa_initialize_key_slots( );
    if( status != PSA_SUCCESS )
        goto exit;

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    status = psa_init_all_se_drivers( );
    if( status != PSA_SUCCESS )
        goto exit;
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */

#if defined(PSA_CRYPTO_STORAGE_HAS_TRANSACTIONS)
    status = psa_crypto_load_transaction( );
    if( status == PSA_SUCCESS )
    {
        status = psa_crypto_recover_transaction( &psa_crypto_transaction );
        if( status != PSA_SUCCESS )
            goto exit;
        status = psa_crypto_stop_transaction( );
    }
    else if( status == PSA_ERROR_DOES_NOT_EXIST )
    {
        /* There's no transaction to complete. It's all good. */
        status = PSA_SUCCESS;
    }
#endif /* PSA_CRYPTO_STORAGE_HAS_TRANSACTIONS */

    /* All done. */
    global_data.initialized = 1;

exit:
    if( status != PSA_SUCCESS )
        mbedtls_psa_crypto_free( );
    return( status );
}

#endif /* MBEDTLS_PSA_CRYPTO_C */
