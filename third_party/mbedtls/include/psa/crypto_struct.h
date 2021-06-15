/**
 * \file psa/crypto_struct.h
 *
 * \brief PSA cryptography module: Mbed TLS structured type implementations
 *
 * \note This file may not be included directly. Applications must
 * include psa/crypto.h.
 *
 * This file contains the definitions of some data structures with
 * implementation-specific definitions.
 *
 * In implementations with isolation between the application and the
 * cryptography module, it is expected that the front-end and the back-end
 * would have different versions of this file.
 *
 * <h3>Design notes about multipart operation structures</h3>
 *
 * Each multipart operation structure contains a `psa_algorithm_t alg`
 * field which indicates which specific algorithm the structure is for.
 * When the structure is not in use, `alg` is 0. Most of the structure
 * consists of a union which is discriminated by `alg`.
 *
 * Note that when `alg` is 0, the content of other fields is undefined.
 * In particular, it is not guaranteed that a freshly-initialized structure
 * is all-zero: we initialize structures to something like `{0, 0}`, which
 * is only guaranteed to initializes the first member of the union;
 * GCC and Clang initialize the whole structure to 0 (at the time of writing),
 * but MSVC and CompCert don't.
 *
 * In Mbed Crypto, multipart operation structures live independently from
 * the key. This allows Mbed Crypto to free the key objects when destroying
 * a key slot. If a multipart operation needs to remember the key after
 * the setup function returns, the operation structure needs to contain a
 * copy of the key.
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

#ifndef PSA_CRYPTO_STRUCT_H
#define PSA_CRYPTO_STRUCT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Include the Mbed TLS configuration file, the way Mbed TLS does it
 * in each of its header files. */
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "mbedtls/cipher.h"
#include "mbedtls/cmac.h"
#include "mbedtls/gcm.h"
#include "mbedtls/md.h"
#include "mbedtls/md2.h"
#include "mbedtls/md4.h"
#include "mbedtls/md5.h"
#include "mbedtls/ripemd160.h"
#include "mbedtls/sha1.h"
#include "mbedtls/sha256.h"
#include "mbedtls/sha512.h"

typedef struct {
    /** Unique ID indicating which driver got assigned to do the
     * operation. Since driver contexts are driver-specific, swapping
     * drivers halfway through the operation is not supported.
     * ID values are auto-generated in psa_driver_wrappers.h */
    unsigned int id;
    /** Context structure for the assigned driver, when id is not zero. */
    void* ctx;
} psa_operation_driver_context_t;

struct psa_hash_operation_s
{
    psa_algorithm_t alg;
    union
    {
        unsigned dummy; /* Make the union non-empty even with no supported algorithms. */
#if defined(MBEDTLS_MD2_C)
        mbedtls_md2_context md2;
#endif
#if defined(MBEDTLS_MD4_C)
        mbedtls_md4_context md4;
#endif
#if defined(MBEDTLS_MD5_C)
        mbedtls_md5_context md5;
#endif
#if defined(MBEDTLS_RIPEMD160_C)
        mbedtls_ripemd160_context ripemd160;
#endif
#if defined(MBEDTLS_SHA1_C)
        mbedtls_sha1_context sha1;
#endif
#if defined(MBEDTLS_SHA256_C)
        mbedtls_sha256_context sha256;
#endif
#if defined(MBEDTLS_SHA512_C)
        mbedtls_sha512_context sha512;
#endif
    } ctx;
};

#define PSA_HASH_OPERATION_INIT {0, {0}}
static inline struct psa_hash_operation_s psa_hash_operation_init( void )
{
    const struct psa_hash_operation_s v = PSA_HASH_OPERATION_INIT;
    return( v );
}

#if defined(MBEDTLS_MD_C)
typedef struct
{
        /** The hash context. */
        struct psa_hash_operation_s hash_ctx;
        /** The HMAC part of the context. */
        uint8_t opad[PSA_HMAC_MAX_HASH_BLOCK_SIZE];
} psa_hmac_internal_data;
#endif /* MBEDTLS_MD_C */

struct psa_mac_operation_s
{
    psa_algorithm_t alg;
    unsigned int key_set : 1;
    unsigned int iv_required : 1;
    unsigned int iv_set : 1;
    unsigned int has_input : 1;
    unsigned int is_sign : 1;
    uint8_t mac_size;
    union
    {
        unsigned dummy; /* Make the union non-empty even with no supported algorithms. */
#if defined(MBEDTLS_MD_C)
        psa_hmac_internal_data hmac;
#endif
#if defined(MBEDTLS_CMAC_C)
        mbedtls_cipher_context_t cmac;
#endif
    } ctx;
};

#define PSA_MAC_OPERATION_INIT {0, 0, 0, 0, 0, 0, 0, {0}}
static inline struct psa_mac_operation_s psa_mac_operation_init( void )
{
    const struct psa_mac_operation_s v = PSA_MAC_OPERATION_INIT;
    return( v );
}

struct psa_cipher_operation_s
{
    psa_algorithm_t alg;
    unsigned int key_set : 1;
    unsigned int iv_required : 1;
    unsigned int iv_set : 1;
    unsigned int mbedtls_in_use : 1; /* Indicates mbed TLS is handling the operation. */
    uint8_t iv_size;
    uint8_t block_size;
    union
    {
        unsigned dummy; /* Enable easier initializing of the union. */
        mbedtls_cipher_context_t cipher;
        psa_operation_driver_context_t driver;
    } ctx;
};

#define PSA_CIPHER_OPERATION_INIT {0, 0, 0, 0, 0, 0, 0, {0}}
static inline struct psa_cipher_operation_s psa_cipher_operation_init( void )
{
    const struct psa_cipher_operation_s v = PSA_CIPHER_OPERATION_INIT;
    return( v );
}

struct psa_aead_operation_s
{
    psa_algorithm_t alg;
    unsigned int key_set : 1;
    unsigned int iv_set : 1;
    uint8_t iv_size;
    uint8_t block_size;
    union
    {
        unsigned dummy; /* Enable easier initializing of the union. */
        mbedtls_cipher_context_t cipher;
    } ctx;
};

#define PSA_AEAD_OPERATION_INIT {0, 0, 0, 0, 0, {0}}
static inline struct psa_aead_operation_s psa_aead_operation_init( void )
{
    const struct psa_aead_operation_s v = PSA_AEAD_OPERATION_INIT;
    return( v );
}

#if defined(MBEDTLS_MD_C)
typedef struct
{
    uint8_t *info;
    size_t info_length;
    psa_hmac_internal_data hmac;
    uint8_t prk[PSA_HASH_MAX_SIZE];
    uint8_t output_block[PSA_HASH_MAX_SIZE];
#if PSA_HASH_MAX_SIZE > 0xff
#error "PSA_HASH_MAX_SIZE does not fit in uint8_t"
#endif
    uint8_t offset_in_block;
    uint8_t block_number;
    unsigned int state : 2;
    unsigned int info_set : 1;
} psa_hkdf_key_derivation_t;
#endif /* MBEDTLS_MD_C */

#if defined(MBEDTLS_MD_C)
typedef enum
{
    PSA_TLS12_PRF_STATE_INIT,       /* no input provided */
    PSA_TLS12_PRF_STATE_SEED_SET,   /* seed has been set */
    PSA_TLS12_PRF_STATE_KEY_SET,    /* key has been set */
    PSA_TLS12_PRF_STATE_LABEL_SET,  /* label has been set */
    PSA_TLS12_PRF_STATE_OUTPUT      /* output has been started */
} psa_tls12_prf_key_derivation_state_t;

typedef struct psa_tls12_prf_key_derivation_s
{
#if PSA_HASH_MAX_SIZE > 0xff
#error "PSA_HASH_MAX_SIZE does not fit in uint8_t"
#endif

    /* Indicates how many bytes in the current HMAC block have
     * not yet been read by the user. */
    uint8_t left_in_block;

    /* The 1-based number of the block. */
    uint8_t block_number;

    psa_tls12_prf_key_derivation_state_t state;

    uint8_t *seed;
    size_t seed_length;
    uint8_t *label;
    size_t label_length;
    psa_hmac_internal_data hmac;
    uint8_t Ai[PSA_HASH_MAX_SIZE];

    /* `HMAC_hash( prk, A(i) + seed )` in the notation of RFC 5246, Sect. 5. */
    uint8_t output_block[PSA_HASH_MAX_SIZE];
} psa_tls12_prf_key_derivation_t;
#endif /* MBEDTLS_MD_C */

struct psa_key_derivation_s
{
    psa_algorithm_t alg;
    unsigned int can_output_key : 1;
    size_t capacity;
    union
    {
        /* Make the union non-empty even with no supported algorithms. */
        uint8_t dummy;
#if defined(MBEDTLS_MD_C)
        psa_hkdf_key_derivation_t hkdf;
        psa_tls12_prf_key_derivation_t tls12_prf;
#endif
    } ctx;
};

/* This only zeroes out the first byte in the union, the rest is unspecified. */
#define PSA_KEY_DERIVATION_OPERATION_INIT {0, 0, 0, {0}}
static inline struct psa_key_derivation_s psa_key_derivation_operation_init( void )
{
    const struct psa_key_derivation_s v = PSA_KEY_DERIVATION_OPERATION_INIT;
    return( v );
}

struct psa_key_policy_s
{
    psa_key_usage_t usage;
    psa_algorithm_t alg;
    psa_algorithm_t alg2;
};
typedef struct psa_key_policy_s psa_key_policy_t;

#define PSA_KEY_POLICY_INIT {0, 0, 0}
static inline struct psa_key_policy_s psa_key_policy_init( void )
{
    const struct psa_key_policy_s v = PSA_KEY_POLICY_INIT;
    return( v );
}

/* The type used internally for key sizes.
 * Public interfaces use size_t, but internally we use a smaller type. */
typedef uint16_t psa_key_bits_t;
/* The maximum value of the type used to represent bit-sizes.
 * This is used to mark an invalid key size. */
#define PSA_KEY_BITS_TOO_LARGE ( (psa_key_bits_t) ( -1 ) )
/* The maximum size of a key in bits.
 * Currently defined as the maximum that can be represented, rounded down
 * to a whole number of bytes.
 * This is an uncast value so that it can be used in preprocessor
 * conditionals. */
#define PSA_MAX_KEY_BITS 0xfff8

/** A mask of flags that can be stored in key attributes.
 *
 * This type is also used internally to store flags in slots. Internal
 * flags are defined in library/psa_crypto_core.h. Internal flags may have
 * the same value as external flags if they are properly handled during
 * key creation and in psa_get_key_attributes.
 */
typedef uint16_t psa_key_attributes_flag_t;

#define MBEDTLS_PSA_KA_FLAG_HAS_SLOT_NUMBER     \
    ( (psa_key_attributes_flag_t) 0x0001 )

/* A mask of key attribute flags used externally only.
 * Only meant for internal checks inside the library. */
#define MBEDTLS_PSA_KA_MASK_EXTERNAL_ONLY (      \
        MBEDTLS_PSA_KA_FLAG_HAS_SLOT_NUMBER |    \
        0 )

/* A mask of key attribute flags used both internally and externally.
 * Currently there aren't any. */
#define MBEDTLS_PSA_KA_MASK_DUAL_USE (          \
        0 )

typedef struct
{
    psa_key_type_t type;
    psa_key_bits_t bits;
    psa_key_lifetime_t lifetime;
    mbedtls_svc_key_id_t id;
    psa_key_policy_t policy;
    psa_key_attributes_flag_t flags;
} psa_core_key_attributes_t;

#define PSA_CORE_KEY_ATTRIBUTES_INIT {PSA_KEY_TYPE_NONE, 0, PSA_KEY_LIFETIME_VOLATILE, MBEDTLS_SVC_KEY_ID_INIT, PSA_KEY_POLICY_INIT, 0}

struct psa_key_attributes_s
{
    psa_core_key_attributes_t core;
#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
    psa_key_slot_number_t slot_number;
#endif /* MBEDTLS_PSA_CRYPTO_SE_C */
    void *domain_parameters;
    size_t domain_parameters_size;
};

#if defined(MBEDTLS_PSA_CRYPTO_SE_C)
#define PSA_KEY_ATTRIBUTES_INIT {PSA_CORE_KEY_ATTRIBUTES_INIT, 0, NULL, 0}
#else
#define PSA_KEY_ATTRIBUTES_INIT {PSA_CORE_KEY_ATTRIBUTES_INIT, NULL, 0}
#endif

static inline struct psa_key_attributes_s psa_key_attributes_init( void )
{
    const struct psa_key_attributes_s v = PSA_KEY_ATTRIBUTES_INIT;
    return( v );
}

static inline void psa_set_key_id( psa_key_attributes_t *attributes,
                                   mbedtls_svc_key_id_t key )
{
    psa_key_lifetime_t lifetime = attributes->core.lifetime;

    attributes->core.id = key;

    if( PSA_KEY_LIFETIME_IS_VOLATILE( lifetime ) )
    {
        attributes->core.lifetime =
            PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(
                PSA_KEY_LIFETIME_PERSISTENT,
                PSA_KEY_LIFETIME_GET_LOCATION( lifetime ) );
    }
}

static inline mbedtls_svc_key_id_t psa_get_key_id(
    const psa_key_attributes_t *attributes)
{
    return( attributes->core.id );
}

#ifdef MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER
static inline void mbedtls_set_key_owner_id( psa_key_attributes_t *attributes,
                                             mbedtls_key_owner_id_t owner )
{
    attributes->core.id.owner = owner;
}
#endif

static inline void psa_set_key_lifetime(psa_key_attributes_t *attributes,
                                        psa_key_lifetime_t lifetime)
{
    attributes->core.lifetime = lifetime;
    if( PSA_KEY_LIFETIME_IS_VOLATILE( lifetime ) )
    {
#ifdef MBEDTLS_PSA_CRYPTO_KEY_ID_ENCODES_OWNER
        attributes->core.id.key_id = 0;
#else
        attributes->core.id = 0;
#endif
    }
}

static inline psa_key_lifetime_t psa_get_key_lifetime(
    const psa_key_attributes_t *attributes)
{
    return( attributes->core.lifetime );
}

static inline void psa_set_key_usage_flags(psa_key_attributes_t *attributes,
                                           psa_key_usage_t usage_flags)
{
    attributes->core.policy.usage = usage_flags;
}

static inline psa_key_usage_t psa_get_key_usage_flags(
    const psa_key_attributes_t *attributes)
{
    return( attributes->core.policy.usage );
}

static inline void psa_set_key_algorithm(psa_key_attributes_t *attributes,
                                         psa_algorithm_t alg)
{
    attributes->core.policy.alg = alg;
}

static inline psa_algorithm_t psa_get_key_algorithm(
    const psa_key_attributes_t *attributes)
{
    return( attributes->core.policy.alg );
}

/* This function is declared in crypto_extra.h, which comes after this
 * header file, but we need the function here, so repeat the declaration. */
psa_status_t psa_set_key_domain_parameters(psa_key_attributes_t *attributes,
                                           psa_key_type_t type,
                                           const uint8_t *data,
                                           size_t data_length);

static inline void psa_set_key_type(psa_key_attributes_t *attributes,
                                    psa_key_type_t type)
{
    if( attributes->domain_parameters == NULL )
    {
        /* Common case: quick path */
        attributes->core.type = type;
    }
    else
    {
        /* Call the bigger function to free the old domain paramteres.
         * Ignore any errors which may arise due to type requiring
         * non-default domain parameters, since this function can't
         * report errors. */
        (void) psa_set_key_domain_parameters( attributes, type, NULL, 0 );
    }
}

static inline psa_key_type_t psa_get_key_type(
    const psa_key_attributes_t *attributes)
{
    return( attributes->core.type );
}

static inline void psa_set_key_bits(psa_key_attributes_t *attributes,
                                    size_t bits)
{
    if( bits > PSA_MAX_KEY_BITS )
        attributes->core.bits = PSA_KEY_BITS_TOO_LARGE;
    else
        attributes->core.bits = (psa_key_bits_t) bits;
}

static inline size_t psa_get_key_bits(
    const psa_key_attributes_t *attributes)
{
    return( attributes->core.bits );
}

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_STRUCT_H */
