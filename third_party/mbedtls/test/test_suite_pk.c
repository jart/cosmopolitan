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
#include "third_party/mbedtls/test/test.inc"
/*
 * *** THIS FILE WAS MACHINE GENERATED ***
 *
 * This file has been machine generated using the script:
 * generate_test_code.py and then mbedtls_test_suite.sh and then mbedtls_test_suite.sh
 *
 * Test file      : ./test_suite_pk.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_pk.function
 *      Test suite data     : suites/test_suite_pk.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_PK_C)
#include "third_party/mbedtls/pk.h"

/* For error codes */
#include "third_party/mbedtls/asn1.h"
#include "third_party/mbedtls/base64.h"
#include "third_party/mbedtls/ecp.h"
#include "third_party/mbedtls/rsa.h"


/* Needed only for test case data under #if defined(MBEDTLS_USE_PSA_CRYPTO),
 * but the test code generator requires test case data to be valid C code
 * unconditionally (https://github.com/ARMmbed/mbedtls/issues/2023). */

#define RSA_KEY_SIZE 512
#define RSA_KEY_LEN   64

/** Generate a key of the desired type.
 *
 * \param pk        The PK object to fill. It must have been initialized
 *                  with mbedtls_pk_setup().
 * \param parameter - For RSA keys, the key size in bits.
 *                  - For EC keys, the curve (\c MBEDTLS_ECP_DP_xxx).
 *
 * \return          The status from the underlying type-specific key
 *                  generation function.
 * \return          -1 if the key type is not recognized.
 */
static int pk_genkey( mbedtls_pk_context *pk, int parameter )
{
    ((void) pk);
    (void) parameter;

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_GENPRIME)
    if( mbedtls_pk_get_type( pk ) == MBEDTLS_PK_RSA )
        return mbedtls_rsa_gen_key( mbedtls_pk_rsa( *pk ),
                                    mbedtls_test_rnd_std_rand, NULL,
                                    parameter, 3 );
#endif
#if defined(MBEDTLS_ECP_C)
    if( mbedtls_pk_get_type( pk ) == MBEDTLS_PK_ECKEY ||
        mbedtls_pk_get_type( pk ) == MBEDTLS_PK_ECKEY_DH ||
        mbedtls_pk_get_type( pk ) == MBEDTLS_PK_ECDSA )
    {
        int ret;
        if( ( ret = mbedtls_ecp_group_load( &mbedtls_pk_ec( *pk )->grp,
                                            parameter ) ) != 0 )
            return( ret );

        return mbedtls_ecp_gen_keypair( &mbedtls_pk_ec( *pk )->grp,
                                        &mbedtls_pk_ec( *pk )->d,
                                        &mbedtls_pk_ec( *pk )->Q,
                                        mbedtls_test_rnd_std_rand, NULL );
    }
#endif
    return( -1 );
}

#if defined(MBEDTLS_RSA_C)
int mbedtls_rsa_decrypt_func( void *ctx, int mode, size_t *olen,
                       const unsigned char *input, unsigned char *output,
                       size_t output_max_len )
{
    return( mbedtls_rsa_pkcs1_decrypt( (mbedtls_rsa_context *) ctx,
                                       mbedtls_test_rnd_std_rand, NULL, mode,
                                       olen, input, output, output_max_len ) );
}
int mbedtls_rsa_sign_func( void *ctx,
                   int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                   int mode, mbedtls_md_type_t md_alg, unsigned int hashlen,
                   const unsigned char *hash, unsigned char *sig )
{
    ((void) f_rng);
    ((void) p_rng);
    return( mbedtls_rsa_pkcs1_sign( (mbedtls_rsa_context *) ctx,
                                    mbedtls_test_rnd_std_rand, NULL, mode,
                                    md_alg, hashlen, hash, sig ) );
}
size_t mbedtls_rsa_key_len_func( void *ctx )
{
    return( ((const mbedtls_rsa_context *) ctx)->len );
}
#endif /* MBEDTLS_RSA_C */

#if defined(MBEDTLS_USE_PSA_CRYPTO)

/*
 * Generate a key using PSA and return the key identifier of that key,
 * or 0 if the key generation failed.
 * The key uses NIST P-256 and is usable for signing with SHA-256.
 */
mbedtls_svc_key_id_t pk_psa_genkey( void )
{
    mbedtls_svc_key_id_t key;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    const psa_key_type_t type =
        PSA_KEY_TYPE_ECC_KEY_PAIR( PSA_ECC_FAMILY_SECP_R1 );
    const size_t bits = 256;

    psa_set_key_usage_flags( &attributes, PSA_KEY_USAGE_SIGN_HASH );
    psa_set_key_algorithm( &attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256) );
    psa_set_key_type( &attributes, type );
    psa_set_key_bits( &attributes, bits );
    PSA_ASSERT( psa_generate_key( &attributes, &key ) );

exit:
    return( key );
}
#endif /* MBEDTLS_USE_PSA_CRYPTO */
#if defined(MBEDTLS_USE_PSA_CRYPTO)
#if defined(MBEDTLS_ECDSA_C)
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
void test_pk_psa_utils(  )
{
    mbedtls_pk_context pk, pk2;
    mbedtls_svc_key_id_t key;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;

    const char * const name = "Opaque";
    const size_t bitlen = 256; /* harcoded in genkey() */

    mbedtls_md_type_t md_alg = MBEDTLS_MD_NONE;
    unsigned char b1[1], b2[1];
    size_t len;
    mbedtls_pk_debug_item dbg;

    PSA_ASSERT( psa_crypto_init( ) );

    mbedtls_pk_init( &pk );
    mbedtls_pk_init( &pk2 );

    TEST_ASSERT( psa_crypto_init( ) == PSA_SUCCESS );

    TEST_ASSERT( mbedtls_pk_setup_opaque( &pk, MBEDTLS_SVC_KEY_ID_INIT ) ==
                 MBEDTLS_ERR_PK_BAD_INPUT_DATA );

    mbedtls_pk_free( &pk );
    mbedtls_pk_init( &pk );

    key = pk_psa_genkey();
    if( mbedtls_svc_key_id_is_null( key ) )
        goto exit;

    TEST_ASSERT( mbedtls_pk_setup_opaque( &pk, key ) == 0 );

    TEST_ASSERT( mbedtls_pk_get_type( &pk ) == MBEDTLS_PK_OPAQUE );
    TEST_ASSERT( strcmp( mbedtls_pk_get_name( &pk), name ) == 0 );

    TEST_ASSERT( mbedtls_pk_get_bitlen( &pk ) == bitlen );
    TEST_ASSERT( mbedtls_pk_get_len( &pk ) == bitlen / 8 );

    TEST_ASSERT( mbedtls_pk_can_do( &pk, MBEDTLS_PK_ECKEY ) == 1 );
    TEST_ASSERT( mbedtls_pk_can_do( &pk, MBEDTLS_PK_ECDSA ) == 1 );
    TEST_ASSERT( mbedtls_pk_can_do( &pk, MBEDTLS_PK_RSA ) == 0 );

    /* unsupported operations: verify, decrypt, encrypt */
    TEST_ASSERT( mbedtls_pk_verify( &pk, md_alg,
                                    b1, sizeof( b1), b2, sizeof( b2 ) )
                 == MBEDTLS_ERR_PK_TYPE_MISMATCH );
    TEST_ASSERT( mbedtls_pk_decrypt( &pk, b1, sizeof( b1 ),
                                     b2, &len, sizeof( b2 ),
                                     NULL, NULL )
                 == MBEDTLS_ERR_PK_TYPE_MISMATCH );
    TEST_ASSERT( mbedtls_pk_encrypt( &pk, b1, sizeof( b1 ),
                                     b2, &len, sizeof( b2 ),
                                     NULL, NULL )
                 == MBEDTLS_ERR_PK_TYPE_MISMATCH );

    /* unsupported functions: check_pair, debug */
    TEST_ASSERT( mbedtls_pk_setup( &pk2,
                 mbedtls_pk_info_from_type( MBEDTLS_PK_ECKEY ) ) == 0 );
    TEST_ASSERT( mbedtls_pk_check_pair( &pk, &pk2 )
                 == MBEDTLS_ERR_PK_TYPE_MISMATCH );
    TEST_ASSERT( mbedtls_pk_debug( &pk, &dbg )
                 == MBEDTLS_ERR_PK_TYPE_MISMATCH );

    /* test that freeing the context does not destroy the key */
    mbedtls_pk_free( &pk );
    TEST_ASSERT( PSA_SUCCESS == psa_get_key_attributes( key, &attributes ) );
    TEST_ASSERT( PSA_SUCCESS == psa_destroy_key( key ) );

exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes( &attributes );

    mbedtls_pk_free( &pk ); /* redundant except upon error */
    mbedtls_pk_free( &pk2 );
    USE_PSA_DONE( );
}

void test_pk_psa_utils_wrapper( void ** params )
{
    (void)params;

    test_pk_psa_utils(  );
}
#endif /* MBEDTLS_ECP_DP_SECP256R1_ENABLED */
#endif /* MBEDTLS_ECDSA_C */
#endif /* MBEDTLS_USE_PSA_CRYPTO */
void test_valid_parameters( )
{
    mbedtls_pk_context pk;
    unsigned char buf[1];
    size_t len;
    void *options = NULL;

    mbedtls_pk_init( &pk );

    TEST_VALID_PARAM( mbedtls_pk_free( NULL ) );

#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    TEST_VALID_PARAM( mbedtls_pk_restart_free( NULL ) );
#endif

    TEST_ASSERT( mbedtls_pk_setup( &pk, NULL ) ==
                 MBEDTLS_ERR_PK_BAD_INPUT_DATA );

    /* In informational functions, we accept NULL where a context pointer
     * is expected because that's what the library has done forever.
     * We do not document that NULL is accepted, so we may wish to change
     * the behavior in a future version. */
    TEST_ASSERT( mbedtls_pk_get_bitlen( NULL ) == 0 );
    TEST_ASSERT( mbedtls_pk_get_len( NULL ) == 0 );
    TEST_ASSERT( mbedtls_pk_can_do( NULL, MBEDTLS_PK_NONE ) == 0 );

    TEST_ASSERT( mbedtls_pk_sign_restartable( &pk,
                                              MBEDTLS_MD_NONE,
                                              NULL, 0,
                                              buf, &len,
                                              mbedtls_test_rnd_std_rand, NULL,
                                              NULL ) ==
                 MBEDTLS_ERR_PK_BAD_INPUT_DATA );

    TEST_ASSERT( mbedtls_pk_sign_restartable( &pk,
                                              MBEDTLS_MD_NONE,
                                              NULL, 0,
                                              buf, &len,
                                              mbedtls_test_rnd_std_rand, NULL,
                                              NULL ) ==
                 MBEDTLS_ERR_PK_BAD_INPUT_DATA );

    TEST_ASSERT( mbedtls_pk_sign( &pk,
                                  MBEDTLS_MD_NONE,
                                  NULL, 0,
                                  buf, &len,
                                  mbedtls_test_rnd_std_rand, NULL ) ==
                 MBEDTLS_ERR_PK_BAD_INPUT_DATA );

    TEST_ASSERT( mbedtls_pk_verify_restartable( &pk,
                                                MBEDTLS_MD_NONE,
                                                NULL, 0,
                                                buf, sizeof( buf ),
                                                NULL ) ==
                 MBEDTLS_ERR_PK_BAD_INPUT_DATA );

    TEST_ASSERT( mbedtls_pk_verify( &pk,
                                    MBEDTLS_MD_NONE,
                                    NULL, 0,
                                    buf, sizeof( buf ) ) ==
                 MBEDTLS_ERR_PK_BAD_INPUT_DATA );

    TEST_ASSERT( mbedtls_pk_verify_ext( MBEDTLS_PK_NONE, options,
                                        &pk,
                                        MBEDTLS_MD_NONE,
                                        NULL, 0,
                                        buf, sizeof( buf ) ) ==
                 MBEDTLS_ERR_PK_BAD_INPUT_DATA );

    TEST_ASSERT( mbedtls_pk_encrypt( &pk,
                                     NULL, 0,
                                     NULL, &len, 0,
                                     mbedtls_test_rnd_std_rand, NULL ) ==
                 MBEDTLS_ERR_PK_BAD_INPUT_DATA );

    TEST_ASSERT( mbedtls_pk_decrypt( &pk,
                                     NULL, 0,
                                     NULL, &len, 0,
                                     mbedtls_test_rnd_std_rand, NULL ) ==
                 MBEDTLS_ERR_PK_BAD_INPUT_DATA );

#if defined(MBEDTLS_PK_PARSE_C)
    TEST_ASSERT( mbedtls_pk_parse_key( &pk, NULL, 0, NULL, 1 ) ==
                 MBEDTLS_ERR_PK_KEY_INVALID_FORMAT );

    TEST_ASSERT( mbedtls_pk_parse_public_key( &pk, NULL, 0 ) ==
                 MBEDTLS_ERR_PK_KEY_INVALID_FORMAT );
#endif /* MBEDTLS_PK_PARSE_C */
exit:
    ;
}

void test_valid_parameters_wrapper( void ** params )
{
    (void)params;

    test_valid_parameters(  );
}
#if defined(MBEDTLS_PK_WRITE_C)
void test_valid_parameters_pkwrite( data_t *key_data )
{
    mbedtls_pk_context pk;

    /* For the write tests to be effective, we need a valid key pair. */
    mbedtls_pk_init( &pk );
    TEST_ASSERT( mbedtls_pk_parse_key( &pk,
                                       key_data->x, key_data->len,
                                       NULL, 0 ) == 0 );

    TEST_ASSERT( mbedtls_pk_write_key_der( &pk, NULL, 0 ) ==
                 MBEDTLS_ERR_ASN1_BUF_TOO_SMALL );

    TEST_ASSERT( mbedtls_pk_write_pubkey_der( &pk, NULL, 0 ) ==
                 MBEDTLS_ERR_ASN1_BUF_TOO_SMALL );

#if defined(MBEDTLS_PEM_WRITE_C)
    TEST_ASSERT( mbedtls_pk_write_key_pem( &pk, NULL, 0 ) ==
                 MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL );

    TEST_ASSERT( mbedtls_pk_write_pubkey_pem( &pk, NULL, 0 ) ==
                 MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL );
#endif /* MBEDTLS_PEM_WRITE_C */

exit:
    mbedtls_pk_free( &pk );
}

void test_valid_parameters_pkwrite_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_valid_parameters_pkwrite( &data0 );
}
#endif /* MBEDTLS_PK_WRITE_C */
#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_invalid_parameters( )
{
    size_t len;
    unsigned char *null_buf = NULL;
    unsigned char buf[1];
    unsigned char *p = buf;
    char str[1] = {0};
    mbedtls_pk_context pk;
    mbedtls_md_type_t valid_md = MBEDTLS_MD_SHA256;
    void *options = buf;

    (void) null_buf;
    (void) p;
    (void) str;

    mbedtls_pk_init( &pk );

    TEST_INVALID_PARAM( mbedtls_pk_init( NULL ) );

#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    TEST_INVALID_PARAM( mbedtls_pk_restart_init( NULL ) );
#endif

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_setup( NULL, NULL ) );

#if defined(MBEDTLS_PK_RSA_ALT_SUPPORT)
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_setup_rsa_alt( NULL, buf,
                                                      NULL, NULL, NULL ) );
#endif

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_verify_restartable( NULL,
                                                           MBEDTLS_MD_NONE,
                                                           buf, sizeof( buf ),
                                                           buf, sizeof( buf ),
                                                           NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_verify_restartable( &pk,
                                                           MBEDTLS_MD_NONE,
                                                           NULL, sizeof( buf ),
                                                           buf, sizeof( buf ),
                                                           NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_verify_restartable( &pk,
                                                           valid_md,
                                                           NULL, 0,
                                                           buf, sizeof( buf ),
                                                           NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_verify_restartable( &pk,
                                                           MBEDTLS_MD_NONE,
                                                           buf, sizeof( buf ),
                                                           NULL, sizeof( buf ),
                                                           NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_verify( NULL,
                                               MBEDTLS_MD_NONE,
                                               buf, sizeof( buf ),
                                               buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_verify( &pk,
                                               MBEDTLS_MD_NONE,
                                               NULL, sizeof( buf ),
                                               buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_verify( &pk,
                                               valid_md,
                                               NULL, 0,
                                               buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_verify( &pk,
                                               MBEDTLS_MD_NONE,
                                               buf, sizeof( buf ),
                                               NULL, sizeof( buf ) ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_verify_ext( MBEDTLS_PK_NONE, options,
                                                   NULL,
                                                   MBEDTLS_MD_NONE,
                                                   buf, sizeof( buf ),
                                                   buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_verify_ext( MBEDTLS_PK_NONE, options,
                                                   &pk,
                                                   MBEDTLS_MD_NONE,
                                                   NULL, sizeof( buf ),
                                                   buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_verify_ext( MBEDTLS_PK_NONE, options,
                                                   &pk,
                                                   valid_md,
                                                   NULL, 0,
                                                   buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_verify_ext( MBEDTLS_PK_NONE, options,
                                                   &pk,
                                                   MBEDTLS_MD_NONE,
                                                   buf, sizeof( buf ),
                                                   NULL, sizeof( buf ) ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
        mbedtls_pk_sign_restartable( NULL, MBEDTLS_MD_NONE, buf, sizeof( buf ),
                                     buf, &len, mbedtls_test_rnd_std_rand,
                                     NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
        mbedtls_pk_sign_restartable( &pk, MBEDTLS_MD_NONE, NULL, sizeof( buf ),
                                     buf, &len, mbedtls_test_rnd_std_rand,
                                     NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
        mbedtls_pk_sign_restartable( &pk, valid_md, NULL, 0, buf, &len,
                                     mbedtls_test_rnd_std_rand, NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
        mbedtls_pk_sign_restartable( &pk, MBEDTLS_MD_NONE, buf, sizeof( buf ),
                                     NULL, &len, mbedtls_test_rnd_std_rand,
                                     NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
        mbedtls_pk_sign( NULL, MBEDTLS_MD_NONE, buf, sizeof( buf ),
                         buf, &len, mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
        mbedtls_pk_sign( &pk, MBEDTLS_MD_NONE, NULL, sizeof( buf ),
                         buf, &len, mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
        mbedtls_pk_sign( &pk, valid_md, NULL, 0, buf, &len,
                         mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
        mbedtls_pk_sign( &pk, MBEDTLS_MD_NONE, buf, sizeof( buf ), NULL, &len,
                         mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
        mbedtls_pk_decrypt( NULL, buf, sizeof( buf ), buf, &len, sizeof( buf ),
                            mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
        mbedtls_pk_decrypt( &pk, NULL, sizeof( buf ), buf, &len, sizeof( buf ),
                            mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
        mbedtls_pk_decrypt( &pk, buf, sizeof( buf ), NULL, &len, sizeof( buf ),
                            mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
        mbedtls_pk_decrypt( &pk, buf, sizeof( buf ), buf, NULL, sizeof( buf ),
                            mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
        mbedtls_pk_encrypt( NULL, buf, sizeof( buf ), buf, &len, sizeof( buf ),
                            mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
        mbedtls_pk_encrypt( &pk, NULL, sizeof( buf ), buf, &len, sizeof( buf ),
                            mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
        mbedtls_pk_encrypt( &pk, buf, sizeof( buf ), NULL, &len, sizeof( buf ),
                            mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
        mbedtls_pk_encrypt( &pk, buf, sizeof( buf ), buf, NULL, sizeof( buf ),
                            mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_check_pair( NULL, &pk ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_check_pair( &pk, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_debug( NULL, NULL ) );

#if defined(MBEDTLS_PK_PARSE_C)
#if defined(MBEDTLS_FS_IO)
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_load_file( NULL, &p, &len ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_load_file( str, NULL, &len ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_load_file( str, &p, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_parse_keyfile( NULL, str, NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_parse_keyfile( &pk, NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_parse_public_keyfile( NULL, str ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_parse_public_keyfile( &pk, NULL ) );
#endif

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_parse_subpubkey( NULL, buf, &pk ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_parse_subpubkey( &null_buf, buf, &pk ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_parse_subpubkey( &p, NULL, &pk ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_parse_subpubkey( &p, buf, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_parse_key( NULL,
                                                  buf, sizeof( buf ),
                                                  buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_parse_key( &pk,
                                                  NULL, sizeof( buf ),
                                                  buf, sizeof( buf ) ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_parse_public_key( NULL,
                                                         buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_parse_public_key( &pk,
                                                         NULL, sizeof( buf ) ) );
#endif /* MBEDTLS_PK_PARSE_C */

#if defined(MBEDTLS_PK_WRITE_C)
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_write_pubkey( NULL, p, &pk ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_write_pubkey( &null_buf, p, &pk ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_write_pubkey( &p, NULL, &pk ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_write_pubkey( &p, p, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_write_pubkey_der( NULL,
                                                         buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_write_pubkey_der( &pk,
                                                         NULL, sizeof( buf ) ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_write_key_der( NULL,
                                                      buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_write_key_der( &pk,
                                                      NULL, sizeof( buf ) ) );

#if defined(MBEDTLS_PEM_WRITE_C)
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_write_pubkey_pem( NULL,
                                                         buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_write_pubkey_pem( &pk,
                                                         NULL, sizeof( buf ) ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_write_key_pem( NULL,
                                                      buf, sizeof( buf ) ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_PK_BAD_INPUT_DATA,
                            mbedtls_pk_write_key_pem( &pk,
                                                      NULL, sizeof( buf ) ) );
#endif /* MBEDTLS_PEM_WRITE_C */

#endif /* MBEDTLS_PK_WRITE_C */
exit:
    ;
}

void test_invalid_parameters_wrapper( void ** params )
{
    (void)params;

    test_invalid_parameters(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
void test_pk_utils( int type, int parameter, int bitlen, int len, char * name )
{
    mbedtls_pk_context pk;

    mbedtls_pk_init( &pk );

    TEST_ASSERT( mbedtls_pk_setup( &pk, mbedtls_pk_info_from_type( type ) ) == 0 );
    TEST_ASSERT( pk_genkey( &pk, parameter ) == 0 );

    TEST_ASSERT( (int) mbedtls_pk_get_type( &pk ) == type );
    TEST_ASSERT( mbedtls_pk_can_do( &pk, type ) );
    TEST_ASSERT( mbedtls_pk_get_bitlen( &pk ) == (unsigned) bitlen );
    TEST_ASSERT( mbedtls_pk_get_len( &pk ) == (unsigned) len );
    TEST_ASSERT( strcmp( mbedtls_pk_get_name( &pk), name ) == 0 );

exit:
    mbedtls_pk_free( &pk );
}

void test_pk_utils_wrapper( void ** params )
{

    test_pk_utils( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4] );
}
#if defined(MBEDTLS_PK_PARSE_C)
#if defined(MBEDTLS_FS_IO)
void test_mbedtls_pk_check_pair( char * pub_file, char * prv_file, int ret )
{
    mbedtls_pk_context pub, prv, alt;

    mbedtls_pk_init( &pub );
    mbedtls_pk_init( &prv );
    mbedtls_pk_init( &alt );

    TEST_ASSERT( mbedtls_pk_parse_public_keyfile( &pub, pub_file ) == 0 );
    TEST_ASSERT( mbedtls_pk_parse_keyfile( &prv, prv_file, NULL ) == 0 );

    TEST_ASSERT( mbedtls_pk_check_pair( &pub, &prv ) == ret );

#if defined(MBEDTLS_RSA_C) && defined(MBEDTLS_PK_RSA_ALT_SUPPORT)
    if( mbedtls_pk_get_type( &prv ) == MBEDTLS_PK_RSA )
    {
        TEST_ASSERT( mbedtls_pk_setup_rsa_alt( &alt, mbedtls_pk_rsa( prv ),
                     mbedtls_rsa_decrypt_func, mbedtls_rsa_sign_func,
                     mbedtls_rsa_key_len_func ) == 0 );
        TEST_ASSERT( mbedtls_pk_check_pair( &pub, &alt ) == ret );
    }
#endif

    mbedtls_pk_free( &pub );
    mbedtls_pk_free( &prv );
    mbedtls_pk_free( &alt );
exit:
    ;
}

void test_mbedtls_pk_check_pair_wrapper( void ** params )
{

    test_mbedtls_pk_check_pair( (char *) params[0], (char *) params[1], *( (int *) params[2] ) );
}
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_PK_PARSE_C */
#if defined(MBEDTLS_RSA_C)
void test_pk_rsa_verify_test_vec( data_t * message_str, int digest, int mod,
                             int radix_N, char * input_N, int radix_E,
                             char * input_E, data_t * result_str,
                             int result )
{
    unsigned char hash_result[MBEDTLS_MD_MAX_SIZE];
    mbedtls_rsa_context *rsa;
    mbedtls_pk_context pk;
    mbedtls_pk_restart_ctx *rs_ctx = NULL;
#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    mbedtls_pk_restart_ctx ctx;

    rs_ctx = &ctx;
    mbedtls_pk_restart_init( rs_ctx );
    // this setting would ensure restart would happen if ECC was used
    mbedtls_ecp_set_max_ops( 1 );
#endif

    mbedtls_pk_init( &pk );

    memset( hash_result, 0x00, MBEDTLS_MD_MAX_SIZE );

    TEST_ASSERT( mbedtls_pk_setup( &pk, mbedtls_pk_info_from_type( MBEDTLS_PK_RSA ) ) == 0 );
    rsa = mbedtls_pk_rsa( pk );

    rsa->len = mod / 8;
    TEST_ASSERT( mbedtls_mpi_read_string( &rsa->N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &rsa->E, radix_E, input_E ) == 0 );


    if( mbedtls_md_info_from_type( digest ) != NULL )
        TEST_ASSERT( mbedtls_md( mbedtls_md_info_from_type( digest ), message_str->x, message_str->len, hash_result ) == 0 );

    TEST_ASSERT( mbedtls_pk_verify( &pk, digest, hash_result, 0,
                            result_str->x, mbedtls_pk_get_len( &pk ) ) == result );

    TEST_ASSERT( mbedtls_pk_verify_restartable( &pk, digest, hash_result, 0,
                    result_str->x, mbedtls_pk_get_len( &pk ), rs_ctx ) == result );

exit:
#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    mbedtls_pk_restart_free( rs_ctx );
#endif
    mbedtls_pk_free( &pk );
}

void test_pk_rsa_verify_test_vec_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_pk_rsa_verify_test_vec( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ), (char *) params[7], &data8, *( (int *) params[10] ) );
}
#endif /* MBEDTLS_RSA_C */
#if defined(MBEDTLS_RSA_C)
void test_pk_rsa_verify_ext_test_vec( data_t * message_str, int digest,
                                 int mod, int radix_N, char * input_N,
                                 int radix_E, char * input_E,
                                 data_t * result_str, int pk_type,
                                 int mgf1_hash_id, int salt_len, int result )
{
    unsigned char hash_result[MBEDTLS_MD_MAX_SIZE];
    mbedtls_rsa_context *rsa;
    mbedtls_pk_context pk;
    mbedtls_pk_rsassa_pss_options pss_opts;
    void *options;
    size_t hash_len;

    mbedtls_pk_init( &pk );

    memset( hash_result, 0x00, sizeof( hash_result ) );

    TEST_ASSERT( mbedtls_pk_setup( &pk, mbedtls_pk_info_from_type( MBEDTLS_PK_RSA ) ) == 0 );
    rsa = mbedtls_pk_rsa( pk );

    rsa->len = mod / 8;
    TEST_ASSERT( mbedtls_mpi_read_string( &rsa->N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &rsa->E, radix_E, input_E ) == 0 );


    if( digest != MBEDTLS_MD_NONE )
    {
        TEST_ASSERT( mbedtls_md( mbedtls_md_info_from_type( digest ),
                     message_str->x, message_str->len, hash_result ) == 0 );
        hash_len = 0;
    }
    else
    {
        memcpy( hash_result, message_str->x, message_str->len );
        hash_len = message_str->len;
    }

    if( mgf1_hash_id < 0 )
    {
        options = NULL;
    }
    else
    {
        options = &pss_opts;

        pss_opts.mgf1_hash_id = mgf1_hash_id;
        pss_opts.expected_salt_len = salt_len;
    }

    TEST_ASSERT( mbedtls_pk_verify_ext( pk_type, options, &pk,
                                digest, hash_result, hash_len,
                                result_str->x, mbedtls_pk_get_len( &pk ) ) == result );

exit:
    mbedtls_pk_free( &pk );
}

void test_pk_rsa_verify_ext_test_vec_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_pk_rsa_verify_ext_test_vec( &data0, *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), (char *) params[5], *( (int *) params[6] ), (char *) params[7], &data8, *( (int *) params[10] ), *( (int *) params[11] ), *( (int *) params[12] ), *( (int *) params[13] ) );
}
#endif /* MBEDTLS_RSA_C */
#if defined(MBEDTLS_ECDSA_C)
void test_pk_ec_test_vec( int type, int id, data_t * key, data_t * hash,
                     data_t * sig, int ret )
{
    mbedtls_pk_context pk;
    mbedtls_ecp_keypair *eckey;

    mbedtls_pk_init( &pk );
    USE_PSA_INIT( );

    TEST_ASSERT( mbedtls_pk_setup( &pk, mbedtls_pk_info_from_type( type ) ) == 0 );

    TEST_ASSERT( mbedtls_pk_can_do( &pk, MBEDTLS_PK_ECDSA ) );
    eckey = mbedtls_pk_ec( pk );

    TEST_ASSERT( mbedtls_ecp_group_load( &eckey->grp, id ) == 0 );
    TEST_ASSERT( mbedtls_ecp_point_read_binary( &eckey->grp, &eckey->Q,
                                        key->x, key->len ) == 0 );

    // MBEDTLS_MD_NONE is used since it will be ignored.
    TEST_ASSERT( mbedtls_pk_verify( &pk, MBEDTLS_MD_NONE,
                            hash->x, hash->len, sig->x, sig->len ) == ret );

exit:
    mbedtls_pk_free( &pk );
    USE_PSA_DONE( );
}

void test_pk_ec_test_vec_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_pk_ec_test_vec( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, *( (int *) params[8] ) );
}
#endif /* MBEDTLS_ECDSA_C */
#if defined(MBEDTLS_ECP_RESTARTABLE)
#if defined(MBEDTLS_ECDSA_C)
#if defined(MBEDTLS_ECDSA_DETERMINISTIC)
void test_pk_sign_verify_restart( int pk_type, int grp_id, char *d_str,
                              char *QX_str, char *QY_str,
                              int md_alg, char *msg, data_t *sig_check,
                              int max_ops, int min_restart, int max_restart )
{
    int ret, cnt_restart;
    mbedtls_pk_restart_ctx rs_ctx;
    mbedtls_pk_context prv, pub;
    unsigned char hash[MBEDTLS_MD_MAX_SIZE];
    unsigned char sig[MBEDTLS_ECDSA_MAX_LEN];
    size_t hlen, slen;
    const mbedtls_md_info_t *md_info;

    mbedtls_pk_restart_init( &rs_ctx );
    mbedtls_pk_init( &prv );
    mbedtls_pk_init( &pub );
    memset( hash, 0, sizeof( hash ) );
    memset( sig, 0, sizeof( sig ) );

    TEST_ASSERT( mbedtls_pk_setup( &prv, mbedtls_pk_info_from_type( pk_type ) ) == 0 );
    TEST_ASSERT( mbedtls_ecp_group_load( &mbedtls_pk_ec( prv )->grp, grp_id ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &mbedtls_pk_ec( prv )->d, 16, d_str ) == 0 );

    TEST_ASSERT( mbedtls_pk_setup( &pub, mbedtls_pk_info_from_type( pk_type ) ) == 0 );
    TEST_ASSERT( mbedtls_ecp_group_load( &mbedtls_pk_ec( pub )->grp, grp_id ) == 0 );
    TEST_ASSERT( mbedtls_ecp_point_read_string( &mbedtls_pk_ec( pub )->Q, 16, QX_str, QY_str ) == 0 );

    md_info = mbedtls_md_info_from_type( md_alg );
    TEST_ASSERT( md_info != NULL );

    hlen = mbedtls_md_get_size( md_info );
    TEST_ASSERT( mbedtls_md( md_info,
                             (const unsigned char *) msg, strlen( msg ),
                             hash ) == 0 );

    mbedtls_ecp_set_max_ops( max_ops );

    slen = sizeof( sig );
    cnt_restart = 0;
    do {
        ret = mbedtls_pk_sign_restartable( &prv, md_alg, hash, hlen,
                                            sig, &slen, NULL, NULL, &rs_ctx );
    } while( ret == MBEDTLS_ERR_ECP_IN_PROGRESS && ++cnt_restart );

    TEST_ASSERT( ret == 0 );
    TEST_ASSERT( slen == sig_check->len );
    TEST_ASSERT( memcmp( sig, sig_check->x, slen ) == 0 );

    TEST_ASSERT( cnt_restart >= min_restart );
    TEST_ASSERT( cnt_restart <= max_restart );

    cnt_restart = 0;
    do {
        ret = mbedtls_pk_verify_restartable( &pub, md_alg,
                                 hash, hlen, sig, slen, &rs_ctx );
    } while( ret == MBEDTLS_ERR_ECP_IN_PROGRESS && ++cnt_restart );

    TEST_ASSERT( ret == 0 );
    TEST_ASSERT( cnt_restart >= min_restart );
    TEST_ASSERT( cnt_restart <= max_restart );

    hash[0]++;
    do {
        ret = mbedtls_pk_verify_restartable( &pub, md_alg,
                                 hash, hlen, sig, slen, &rs_ctx );
    } while( ret == MBEDTLS_ERR_ECP_IN_PROGRESS );
    TEST_ASSERT( ret != 0 );
    hash[0]--;

    sig[0]++;
    do {
        ret = mbedtls_pk_verify_restartable( &pub, md_alg,
                                 hash, hlen, sig, slen, &rs_ctx );
    } while( ret == MBEDTLS_ERR_ECP_IN_PROGRESS );
    TEST_ASSERT( ret != 0 );
    sig[0]--;

    /* Do we leak memory when aborting? try verify then sign
     * This test only makes sense when we actually restart */
    if( min_restart > 0 )
    {
        ret = mbedtls_pk_verify_restartable( &pub, md_alg,
                                 hash, hlen, sig, slen, &rs_ctx );
        TEST_ASSERT( ret == MBEDTLS_ERR_ECP_IN_PROGRESS );
        mbedtls_pk_restart_free( &rs_ctx );

        slen = sizeof( sig );
        ret = mbedtls_pk_sign_restartable( &prv, md_alg, hash, hlen,
                                            sig, &slen, NULL, NULL, &rs_ctx );
        TEST_ASSERT( ret == MBEDTLS_ERR_ECP_IN_PROGRESS );
    }

exit:
    mbedtls_pk_restart_free( &rs_ctx );
    mbedtls_pk_free( &prv );
    mbedtls_pk_free( &pub );
}

void test_pk_sign_verify_restart_wrapper( void ** params )
{
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};

    test_pk_sign_verify_restart( *( (int *) params[0] ), *( (int *) params[1] ), (char *) params[2], (char *) params[3], (char *) params[4], *( (int *) params[5] ), (char *) params[6], &data7, *( (int *) params[9] ), *( (int *) params[10] ), *( (int *) params[11] ) );
}
#endif /* MBEDTLS_ECDSA_DETERMINISTIC */
#endif /* MBEDTLS_ECDSA_C */
#endif /* MBEDTLS_ECP_RESTARTABLE */
#if defined(MBEDTLS_SHA256_C)
void test_pk_sign_verify( int type, int parameter, int sign_ret, int verify_ret )
{
    mbedtls_pk_context pk;
    size_t sig_len;
    unsigned char hash[MBEDTLS_MD_MAX_SIZE];
    unsigned char sig[MBEDTLS_PK_SIGNATURE_MAX_SIZE];
    void *rs_ctx = NULL;
#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    mbedtls_pk_restart_ctx ctx;

    rs_ctx = &ctx;
    mbedtls_pk_restart_init( rs_ctx );
    /* This value is large enough that the operation will complete in one run.
     * See comments at the top of ecp_test_vect_restart in
     * test_suite_ecp.function for estimates of operation counts. */
    mbedtls_ecp_set_max_ops( 42000 );
#endif

    mbedtls_pk_init( &pk );
    USE_PSA_INIT( );

    memset( hash, 0x2a, sizeof hash );
    memset( sig, 0, sizeof sig );

    TEST_ASSERT( mbedtls_pk_setup( &pk, mbedtls_pk_info_from_type( type ) ) == 0 );
    TEST_ASSERT( pk_genkey( &pk, parameter ) == 0 );

    TEST_ASSERT( mbedtls_pk_sign_restartable( &pk, MBEDTLS_MD_SHA256,
                 hash, sizeof hash, sig, &sig_len,
                 mbedtls_test_rnd_std_rand, NULL, rs_ctx ) == sign_ret );
    if( sign_ret == 0 )
        TEST_ASSERT( sig_len <= MBEDTLS_PK_SIGNATURE_MAX_SIZE );
    else
        sig_len = MBEDTLS_PK_SIGNATURE_MAX_SIZE;

    TEST_ASSERT( mbedtls_pk_verify( &pk, MBEDTLS_MD_SHA256,
                            hash, sizeof hash, sig, sig_len ) == verify_ret );

    if( verify_ret == 0 )
    {
        hash[0]++;
        TEST_ASSERT( mbedtls_pk_verify( &pk, MBEDTLS_MD_SHA256,
                                hash, sizeof hash, sig, sig_len ) != 0 );
        hash[0]--;

        sig[0]++;
        TEST_ASSERT( mbedtls_pk_verify( &pk, MBEDTLS_MD_SHA256,
                                hash, sizeof hash, sig, sig_len ) != 0 );
        sig[0]--;
    }

    TEST_ASSERT( mbedtls_pk_sign( &pk, MBEDTLS_MD_SHA256, hash, sizeof hash,
                                  sig, &sig_len,
                                  mbedtls_test_rnd_std_rand,
                                  NULL ) == sign_ret );
    if( sign_ret == 0 )
        TEST_ASSERT( sig_len <= MBEDTLS_PK_SIGNATURE_MAX_SIZE );
    else
        sig_len = MBEDTLS_PK_SIGNATURE_MAX_SIZE;

    TEST_ASSERT( mbedtls_pk_verify_restartable( &pk, MBEDTLS_MD_SHA256,
                 hash, sizeof hash, sig, sig_len, rs_ctx ) == verify_ret );

    if( verify_ret == 0 )
    {
        hash[0]++;
        TEST_ASSERT( mbedtls_pk_verify_restartable( &pk, MBEDTLS_MD_SHA256,
                     hash, sizeof hash, sig, sig_len, rs_ctx ) != 0 );
        hash[0]--;

        sig[0]++;
        TEST_ASSERT( mbedtls_pk_verify_restartable( &pk, MBEDTLS_MD_SHA256,
                     hash, sizeof hash, sig, sig_len, rs_ctx ) != 0 );
        sig[0]--;
    }

exit:
#if defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_RESTARTABLE)
    mbedtls_pk_restart_free( rs_ctx );
#endif
    mbedtls_pk_free( &pk );
    USE_PSA_DONE( );
}

void test_pk_sign_verify_wrapper( void ** params )
{

    test_pk_sign_verify( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ) );
}
#endif /* MBEDTLS_SHA256_C */
#if defined(MBEDTLS_RSA_C)
void test_pk_rsa_encrypt_test_vec( data_t * message, int mod, int radix_N,
                              char * input_N, int radix_E, char * input_E,
                              data_t * result, int ret )
{
    unsigned char output[300];
    mbedtls_test_rnd_pseudo_info rnd_info;
    mbedtls_rsa_context *rsa;
    mbedtls_pk_context pk;
    size_t olen;

    memset( &rnd_info,  0, sizeof( mbedtls_test_rnd_pseudo_info ) );
    memset( output,     0, sizeof( output ) );


    mbedtls_pk_init( &pk );
    TEST_ASSERT( mbedtls_pk_setup( &pk, mbedtls_pk_info_from_type( MBEDTLS_PK_RSA ) ) == 0 );
    rsa = mbedtls_pk_rsa( pk );

    rsa->len = mod / 8;
    TEST_ASSERT( mbedtls_mpi_read_string( &rsa->N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &rsa->E, radix_E, input_E ) == 0 );

    TEST_ASSERT( mbedtls_pk_encrypt( &pk, message->x, message->len,
                            output, &olen, sizeof( output ),
                            mbedtls_test_rnd_pseudo_rand, &rnd_info ) == ret );
    TEST_ASSERT( olen == result->len );
    TEST_ASSERT( memcmp( output, result->x, olen ) == 0 );

exit:
    mbedtls_pk_free( &pk );
}

void test_pk_rsa_encrypt_test_vec_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};

    test_pk_rsa_encrypt_test_vec( &data0, *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ), (char *) params[6], &data7, *( (int *) params[9] ) );
}
#endif /* MBEDTLS_RSA_C */
#if defined(MBEDTLS_RSA_C)
void test_pk_rsa_decrypt_test_vec( data_t * cipher, int mod, int radix_P,
                              char * input_P, int radix_Q, char * input_Q,
                              int radix_N, char * input_N, int radix_E,
                              char * input_E, data_t * clear, int ret )
{
    unsigned char output[256];
    mbedtls_test_rnd_pseudo_info rnd_info;
    mbedtls_mpi N, P, Q, E;
    mbedtls_rsa_context *rsa;
    mbedtls_pk_context pk;
    size_t olen;

    mbedtls_pk_init( &pk );
    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &P );
    mbedtls_mpi_init( &Q ); mbedtls_mpi_init( &E );

    memset( &rnd_info,  0, sizeof( mbedtls_test_rnd_pseudo_info ) );


    /* init pk-rsa context */
    TEST_ASSERT( mbedtls_pk_setup( &pk, mbedtls_pk_info_from_type( MBEDTLS_PK_RSA ) ) == 0 );
    rsa = mbedtls_pk_rsa( pk );

    /* load public key */
    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    /* load private key */
    TEST_ASSERT( mbedtls_mpi_read_string( &P, radix_P, input_P ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Q, radix_Q, input_Q ) == 0 );
    TEST_ASSERT( mbedtls_rsa_import( rsa, &N, &P, &Q, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( rsa ) == (size_t) ( mod / 8 ) );
    TEST_ASSERT( mbedtls_rsa_complete( rsa ) == 0 );

    /* decryption test */
    memset( output, 0, sizeof( output ) );
    olen = 0;
    TEST_ASSERT( mbedtls_pk_decrypt( &pk, cipher->x, cipher->len,
                            output, &olen, sizeof( output ),
                            mbedtls_test_rnd_pseudo_rand, &rnd_info ) == ret );
    if( ret == 0 )
    {
        TEST_ASSERT( olen == clear->len );
        TEST_ASSERT( memcmp( output, clear->x, olen ) == 0 );
    }

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &P );
    mbedtls_mpi_free( &Q ); mbedtls_mpi_free( &E );
    mbedtls_pk_free( &pk );
}

void test_pk_rsa_decrypt_test_vec_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data11 = {(uint8_t *) params[11], *( (uint32_t *) params[12] )};

    test_pk_rsa_decrypt_test_vec( &data0, *( (int *) params[2] ), *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ), (char *) params[6], *( (int *) params[7] ), (char *) params[8], *( (int *) params[9] ), (char *) params[10], &data11, *( (int *) params[13] ) );
}
#endif /* MBEDTLS_RSA_C */
void test_pk_ec_nocrypt( int type )
{
    mbedtls_pk_context pk;
    unsigned char output[100];
    unsigned char input[100];
    mbedtls_test_rnd_pseudo_info rnd_info;
    size_t olen = 0;
    int ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;

    mbedtls_pk_init( &pk );

    memset( &rnd_info,  0, sizeof( mbedtls_test_rnd_pseudo_info ) );
    memset( output,     0, sizeof( output ) );
    memset( input,      0, sizeof( input ) );

    TEST_ASSERT( mbedtls_pk_setup( &pk, mbedtls_pk_info_from_type( type ) ) == 0 );

    TEST_ASSERT( mbedtls_pk_encrypt( &pk, input, sizeof( input ),
                            output, &olen, sizeof( output ),
                            mbedtls_test_rnd_pseudo_rand, &rnd_info ) == ret );

    TEST_ASSERT( mbedtls_pk_decrypt( &pk, input, sizeof( input ),
                            output, &olen, sizeof( output ),
                            mbedtls_test_rnd_pseudo_rand, &rnd_info ) == ret );

exit:
    mbedtls_pk_free( &pk );
}

void test_pk_ec_nocrypt_wrapper( void ** params )
{

    test_pk_ec_nocrypt( *( (int *) params[0] ) );
}
#if defined(MBEDTLS_RSA_C)
void test_pk_rsa_overflow( )
{
    mbedtls_pk_context pk;
    size_t hash_len = SIZE_MAX, sig_len = SIZE_MAX;
    unsigned char hash[50], sig[100];

    if( SIZE_MAX <= UINT_MAX )
        return;

    memset( hash, 0x2a, sizeof hash );
    memset( sig, 0, sizeof sig );

    mbedtls_pk_init( &pk );

    TEST_ASSERT( mbedtls_pk_setup( &pk,
                 mbedtls_pk_info_from_type( MBEDTLS_PK_RSA ) ) == 0 );

#if defined(MBEDTLS_PKCS1_V21)
    TEST_ASSERT( mbedtls_pk_verify_ext( MBEDTLS_PK_RSASSA_PSS, NULL, &pk,
                    MBEDTLS_MD_NONE, hash, hash_len, sig, sig_len ) ==
                 MBEDTLS_ERR_PK_BAD_INPUT_DATA );
#endif /* MBEDTLS_PKCS1_V21 */

    TEST_ASSERT( mbedtls_pk_verify( &pk, MBEDTLS_MD_NONE, hash, hash_len,
                    sig, sig_len ) == MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

    TEST_ASSERT( mbedtls_pk_sign( &pk, MBEDTLS_MD_NONE, hash, hash_len, sig,
                                  &sig_len, mbedtls_test_rnd_std_rand, NULL )
                 == MBEDTLS_ERR_RSA_BAD_INPUT_DATA );

exit:
    mbedtls_pk_free( &pk );
}

void test_pk_rsa_overflow_wrapper( void ** params )
{
    (void)params;

    test_pk_rsa_overflow(  );
}
#endif /* MBEDTLS_RSA_C */
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_PK_RSA_ALT_SUPPORT)
void test_pk_rsa_alt(  )
{
    /*
     * An rsa_alt context can only do private operations (decrypt, sign).
     * Test it against the public operations (encrypt, verify) of a
     * corresponding rsa context.
     */
    mbedtls_rsa_context raw;
    mbedtls_pk_context rsa, alt;
    mbedtls_pk_debug_item dbg_items[10];
    unsigned char hash[50], sig[64];
    unsigned char msg[50], ciph[64], test[50];
    size_t sig_len, ciph_len, test_len;
    int ret = MBEDTLS_ERR_PK_TYPE_MISMATCH;

    mbedtls_rsa_init( &raw, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE );
    mbedtls_pk_init( &rsa ); mbedtls_pk_init( &alt );

    memset( hash, 0x2a, sizeof hash );
    memset( sig, 0, sizeof sig );
    memset( msg, 0x2a, sizeof msg );
    memset( ciph, 0, sizeof ciph );
    memset( test, 0, sizeof test );

    /* Initiliaze PK RSA context with random key */
    TEST_ASSERT( mbedtls_pk_setup( &rsa,
                              mbedtls_pk_info_from_type( MBEDTLS_PK_RSA ) ) == 0 );
    TEST_ASSERT( pk_genkey( &rsa, RSA_KEY_SIZE ) == 0 );

    /* Extract key to the raw rsa context */
    TEST_ASSERT( mbedtls_rsa_copy( &raw, mbedtls_pk_rsa( rsa ) ) == 0 );

    /* Initialize PK RSA_ALT context */
    TEST_ASSERT( mbedtls_pk_setup_rsa_alt( &alt, (void *) &raw,
                 mbedtls_rsa_decrypt_func, mbedtls_rsa_sign_func, mbedtls_rsa_key_len_func ) == 0 );

    /* Test administrative functions */
    TEST_ASSERT( mbedtls_pk_can_do( &alt, MBEDTLS_PK_RSA ) );
    TEST_ASSERT( mbedtls_pk_get_bitlen( &alt ) == RSA_KEY_SIZE );
    TEST_ASSERT( mbedtls_pk_get_len( &alt ) == RSA_KEY_LEN );
    TEST_ASSERT( mbedtls_pk_get_type( &alt ) == MBEDTLS_PK_RSA_ALT );
    TEST_ASSERT( strcmp( mbedtls_pk_get_name( &alt ), "RSA-alt" ) == 0 );

    /* Test signature */
#if SIZE_MAX > UINT_MAX
    TEST_ASSERT( mbedtls_pk_sign( &alt, MBEDTLS_MD_NONE, hash, SIZE_MAX, sig,
                                  &sig_len, mbedtls_test_rnd_std_rand, NULL )
                 == MBEDTLS_ERR_PK_BAD_INPUT_DATA );
#endif /* SIZE_MAX > UINT_MAX */
    TEST_ASSERT( mbedtls_pk_sign( &alt, MBEDTLS_MD_NONE, hash, sizeof hash, sig,
                                  &sig_len, mbedtls_test_rnd_std_rand, NULL )
                 == 0 );
    TEST_ASSERT( sig_len == RSA_KEY_LEN );
    TEST_ASSERT( mbedtls_pk_verify( &rsa, MBEDTLS_MD_NONE,
                            hash, sizeof hash, sig, sig_len ) == 0 );

    /* Test decrypt */
    TEST_ASSERT( mbedtls_pk_encrypt( &rsa, msg, sizeof msg,
                             ciph, &ciph_len, sizeof ciph,
                             mbedtls_test_rnd_std_rand, NULL ) == 0 );
    TEST_ASSERT( mbedtls_pk_decrypt( &alt, ciph, ciph_len,
                             test, &test_len, sizeof test,
                             mbedtls_test_rnd_std_rand, NULL ) == 0 );
    TEST_ASSERT( test_len == sizeof msg );
    TEST_ASSERT( memcmp( test, msg, test_len ) == 0 );

    /* Test forbidden operations */
    TEST_ASSERT( mbedtls_pk_encrypt( &alt, msg, sizeof msg,
                             ciph, &ciph_len, sizeof ciph,
                             mbedtls_test_rnd_std_rand, NULL ) == ret );
    TEST_ASSERT( mbedtls_pk_verify( &alt, MBEDTLS_MD_NONE,
                            hash, sizeof hash, sig, sig_len ) == ret );
    TEST_ASSERT( mbedtls_pk_debug( &alt, dbg_items ) == ret );

exit:
    mbedtls_rsa_free( &raw );
    mbedtls_pk_free( &rsa ); mbedtls_pk_free( &alt );
}

void test_pk_rsa_alt_wrapper( void ** params )
{
    (void)params;

    test_pk_rsa_alt(  );
}
#endif /* MBEDTLS_PK_RSA_ALT_SUPPORT */
#endif /* MBEDTLS_RSA_C */
#if defined(MBEDTLS_SHA256_C)
#if defined(MBEDTLS_USE_PSA_CRYPTO)
#if defined(MBEDTLS_ECDSA_C)
void test_pk_psa_sign( int grpid_arg,
                  int psa_curve_arg, int expected_bits_arg )
{
    mbedtls_ecp_group_id grpid = grpid_arg;
    mbedtls_pk_context pk;
    unsigned char hash[32];
    unsigned char sig[MBEDTLS_ECDSA_MAX_LEN];
    unsigned char pkey_legacy[200];
    unsigned char pkey_psa[200];
    unsigned char *pkey_legacy_start, *pkey_psa_start;
    size_t sig_len, klen_legacy, klen_psa;
    int ret;
    mbedtls_svc_key_id_t key_id;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_type_t expected_type = PSA_KEY_TYPE_ECC_KEY_PAIR( psa_curve_arg );
    size_t expected_bits = expected_bits_arg;

    /*
     * This tests making signatures with a wrapped PSA key:
     * - generate a fresh ECP legacy PK context
     * - wrap it in a PK context and make a signature this way
     * - extract the public key
     * - parse it to a PK context and verify the signature this way
     */

    PSA_ASSERT( psa_crypto_init( ) );

    /* Create legacy EC public/private key in PK context. */
    mbedtls_pk_init( &pk );
    TEST_ASSERT( mbedtls_pk_setup( &pk,
                      mbedtls_pk_info_from_type( MBEDTLS_PK_ECKEY ) ) == 0 );
    TEST_ASSERT( mbedtls_ecp_gen_key( grpid,
                                      (mbedtls_ecp_keypair*) pk.pk_ctx,
                                      mbedtls_test_rnd_std_rand, NULL ) == 0 );

    /* Export underlying public key for re-importing in a legacy context. */
    ret = mbedtls_pk_write_pubkey_der( &pk, pkey_legacy,
                                       sizeof( pkey_legacy ) );
    TEST_ASSERT( ret >= 0 );
    klen_legacy = (size_t) ret;
    /* mbedtls_pk_write_pubkey_der() writes backwards in the data buffer. */
    pkey_legacy_start = pkey_legacy + sizeof( pkey_legacy ) - klen_legacy;

    /* Turn PK context into an opaque one. */
    TEST_ASSERT( mbedtls_pk_wrap_as_opaque( &pk, &key_id,
                                            PSA_ALG_SHA_256 ) == 0 );

    PSA_ASSERT( psa_get_key_attributes( key_id, &attributes ) );
    TEST_EQUAL( psa_get_key_type( &attributes ), expected_type );
    TEST_EQUAL( psa_get_key_bits( &attributes ), expected_bits );
    TEST_EQUAL( psa_get_key_lifetime( &attributes ),
                PSA_KEY_LIFETIME_VOLATILE );

    memset( hash, 0x2a, sizeof hash );
    memset( sig, 0, sizeof sig );

    TEST_ASSERT( mbedtls_pk_sign( &pk, MBEDTLS_MD_SHA256,
                 hash, sizeof hash, sig, &sig_len,
                 NULL, NULL ) == 0 );

    /* Export underlying public key for re-importing in a psa context. */
    ret = mbedtls_pk_write_pubkey_der( &pk, pkey_psa,
                                       sizeof( pkey_psa ) );
    TEST_ASSERT( ret >= 0 );
    klen_psa = (size_t) ret;
    /* mbedtls_pk_write_pubkey_der() writes backwards in the data buffer. */
    pkey_psa_start = pkey_psa + sizeof( pkey_psa ) - klen_psa;

    TEST_ASSERT( klen_psa == klen_legacy );
    TEST_ASSERT( memcmp( pkey_psa_start, pkey_legacy_start, klen_psa ) == 0 );

    mbedtls_pk_free( &pk );
    TEST_ASSERT( PSA_SUCCESS == psa_destroy_key( key_id ) );

    mbedtls_pk_init( &pk );
    TEST_ASSERT( mbedtls_pk_parse_public_key( &pk, pkey_legacy_start,
                                              klen_legacy ) == 0 );
    TEST_ASSERT( mbedtls_pk_verify( &pk, MBEDTLS_MD_SHA256,
                            hash, sizeof hash, sig, sig_len ) == 0 );

exit:
    /*
     * Key attributes may have been returned by psa_get_key_attributes()
     * thus reset them as required.
     */
    psa_reset_key_attributes( &attributes );

    mbedtls_pk_free( &pk );
    USE_PSA_DONE( );
}

void test_pk_psa_sign_wrapper( void ** params )
{

    test_pk_psa_sign( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ) );
}
#endif /* MBEDTLS_ECDSA_C */
#endif /* MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_SHA256_C */
#endif /* MBEDTLS_PK_C */

/*----------------------------------------------------------------------------*/
/* Test dispatch code */


/**
 * \brief       Evaluates an expression/macro into its literal integer value.
 *              For optimizing space for embedded targets each expression/macro
 *              is identified by a unique identifier instead of string literals.
 *              Identifiers and evaluation code is generated by script:
 *              generate_test_code.py and then mbedtls_test_suite.sh and then mbedtls_test_suite.sh
 *
 * \param exp_id    Expression identifier.
 * \param out_value Pointer to int to hold the integer.
 *
 * \return       0 if exp_id is found. 1 otherwise.
 */
int get_expression( int32_t exp_id, int32_t * out_value )
{
    int ret = KEY_VALUE_MAPPING_FOUND;

    (void) exp_id;
    (void) out_value;

    switch( exp_id )
    {

#if defined(MBEDTLS_PK_C)

        case 0:
            {
                *out_value = MBEDTLS_PK_RSA;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_PK_ECKEY;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ECP_DP_SECP192R1;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_PK_ECKEY_DH;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_ECP_DP_CURVE25519;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_ECP_DP_CURVE448;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_PK_ECDSA;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_ECP_DP_SECP256R1;
            }
            break;
        case 8:
            {
                *out_value = MBEDTLS_ECP_DP_SECP384R1;
            }
            break;
        case 9:
            {
                *out_value = MBEDTLS_ECP_DP_SECP521R1;
            }
            break;
        case 10:
            {
                *out_value = MBEDTLS_MD_SHA1;
            }
            break;
        case 11:
            {
                *out_value = MBEDTLS_ERR_RSA_VERIFY_FAILED;
            }
            break;
        case 12:
            {
                *out_value = MBEDTLS_ERR_ECP_VERIFY_FAILED;
            }
            break;
        case 13:
            {
                *out_value = MBEDTLS_ECP_DP_BP256R1;
            }
            break;
        case 14:
            {
                *out_value = MBEDTLS_ECP_DP_BP512R1;
            }
            break;
        case 15:
            {
                *out_value = MBEDTLS_ERR_PK_TYPE_MISMATCH;
            }
            break;
        case 16:
            {
                *out_value = MBEDTLS_ERR_RSA_INVALID_PADDING;
            }
            break;
        case 17:
            {
                *out_value = MBEDTLS_MD_SHA256;
            }
            break;
        case 18:
            {
                *out_value = MBEDTLS_PK_RSASSA_PSS;
            }
            break;
        case 19:
            {
                *out_value = MBEDTLS_RSA_SALT_LEN_ANY;
            }
            break;
        case 20:
            {
                *out_value = MBEDTLS_MD_NONE;
            }
            break;
        case 21:
            {
                *out_value = -1;
            }
            break;
        case 22:
            {
                *out_value = MBEDTLS_ERR_PK_BAD_INPUT_DATA;
            }
            break;
        case 23:
            {
                *out_value = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
            }
            break;
        case 24:
            {
                *out_value = MBEDTLS_ERR_RSA_KEY_CHECK_FAILED;
            }
            break;
        case 25:
            {
                *out_value = PSA_ECC_FAMILY_SECP_R1;
            }
            break;
        case 26:
            {
                *out_value = MBEDTLS_ECP_DP_SECP192K1;
            }
            break;
        case 27:
            {
                *out_value = PSA_ECC_FAMILY_SECP_K1;
            }
            break;
        case 28:
            {
                *out_value = MBEDTLS_ECP_DP_SECP256K1;
            }
            break;
        case 29:
            {
                *out_value = PSA_ECC_FAMILY_BRAINPOOL_P_R1;
            }
            break;
        case 30:
            {
                *out_value = MBEDTLS_ECP_DP_BP384R1;
            }
            break;
#endif

        default:
           {
                ret = KEY_VALUE_MAPPING_NOT_FOUND;
           }
           break;
    }
    return( ret );
}


/**
 * \brief       Checks if the dependency i.e. the compile flag is set.
 *              For optimizing space for embedded targets each dependency
 *              is identified by a unique identifier instead of string literals.
 *              Identifiers and check code is generated by script:
 *              generate_test_code.py and then mbedtls_test_suite.sh and then mbedtls_test_suite.sh
 *
 * \param dep_id    Dependency identifier.
 *
 * \return       DEPENDENCY_SUPPORTED if set else DEPENDENCY_NOT_SUPPORTED
 */
int dep_check( int dep_id )
{
    int ret = DEPENDENCY_NOT_SUPPORTED;

    (void) dep_id;

    switch( dep_id )
    {

#if defined(MBEDTLS_PK_C)

        case 0:
            {
#if defined(MBEDTLS_RSA_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_GENPRIME)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_ECP_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if defined(MBEDTLS_ECP_DP_SECP192R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if defined(MBEDTLS_ECP_DP_CURVE25519_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 5:
            {
#if defined(MBEDTLS_ECP_DP_CURVE448_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 6:
            {
#if defined(MBEDTLS_ECDSA_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 7:
            {
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 8:
            {
#if defined(MBEDTLS_ECP_DP_SECP384R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 9:
            {
#if defined(MBEDTLS_ECP_DP_SECP521R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 10:
            {
#if defined(MBEDTLS_SHA1_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 11:
            {
#if defined(MBEDTLS_PKCS1_V15)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 12:
            {
#if defined(MBEDTLS_ECP_DP_BP256R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 13:
            {
#if defined(MBEDTLS_ECP_DP_BP512R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 14:
            {
#if defined(MBEDTLS_PKCS1_V21)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 15:
            {
#if defined(MBEDTLS_SHA256_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 16:
            {
#if defined(MBEDTLS_HAVE_INT64)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 17:
            {
#if defined(MBEDTLS_ECP_DP_SECP192K1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 18:
            {
#if defined(MBEDTLS_ECP_DP_SECP256K1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 19:
            {
#if defined(MBEDTLS_ECP_DP_BP384R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
#endif

        default:
            break;
    }
    return( ret );
}


/**
 * \brief       Function pointer type for test function wrappers.
 *
 * A test function wrapper decodes the parameters and passes them to the
 * underlying test function. Both the wrapper and the underlying function
 * return void. Test wrappers assume that they are passed a suitable
 * parameter array and do not perform any error detection.
 *
 * \param param_array   The array of parameters. Each element is a `void *`
 *                      which the wrapper casts to the correct type and
 *                      dereferences. Each wrapper function hard-codes the
 *                      number and types of the parameters.
 */
typedef void (*TestWrapper_t)( void **param_array );


/**
 * \brief       Table of test function wrappers. Used by dispatch_test().
 *              This table is populated by script:
 *              generate_test_code.py and then mbedtls_test_suite.sh and then mbedtls_test_suite.sh
 *
 */
TestWrapper_t test_funcs[] =
{
/* Function Id: 0 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
    test_pk_psa_utils_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_PK_C)
    test_valid_parameters_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_PK_WRITE_C)
    test_valid_parameters_pkwrite_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_invalid_parameters_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_PK_C)
    test_pk_utils_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_FS_IO)
    test_mbedtls_pk_check_pair_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_RSA_C)
    test_pk_rsa_verify_test_vec_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_RSA_C)
    test_pk_rsa_verify_ext_test_vec_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_ECDSA_C)
    test_pk_ec_test_vec_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_ECP_RESTARTABLE) && defined(MBEDTLS_ECDSA_C) && defined(MBEDTLS_ECDSA_DETERMINISTIC)
    test_pk_sign_verify_restart_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_SHA256_C)
    test_pk_sign_verify_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_RSA_C)
    test_pk_rsa_encrypt_test_vec_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_RSA_C)
    test_pk_rsa_decrypt_test_vec_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_PK_C)
    test_pk_ec_nocrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 14 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_RSA_C)
    test_pk_rsa_overflow_wrapper,
#else
    NULL,
#endif
/* Function Id: 15 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_PK_RSA_ALT_SUPPORT)
    test_pk_rsa_alt_wrapper,
#else
    NULL,
#endif
/* Function Id: 16 */

#if defined(MBEDTLS_PK_C) && defined(MBEDTLS_SHA256_C) && defined(MBEDTLS_USE_PSA_CRYPTO) && defined(MBEDTLS_ECDSA_C)
    test_pk_psa_sign_wrapper,
#else
    NULL,
#endif

};

/**
 * \brief        Execute the test function.
 *
 *               This is a wrapper function around the test function execution
 *               to allow the setjmp() call used to catch any calls to the
 *               parameter failure callback, to be used. Calls to setjmp()
 *               can invalidate the state of any local auto variables.
 *
 * \param fp     Function pointer to the test function.
 * \param params Parameters to pass to the #TestWrapper_t wrapper function.
 *
 */
void execute_function_ptr(TestWrapper_t fp, void **params)
{
#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
    mbedtls_test_enable_insecure_external_rng( );
#endif

#if defined(MBEDTLS_CHECK_PARAMS)
    mbedtls_test_param_failed_location_record_t location_record;

    if ( setjmp( mbedtls_test_param_failed_get_state_buf( ) ) == 0 )
    {
        fp( params );
    }
    else
    {
        /* Unexpected parameter validation error */
        mbedtls_test_param_failed_get_location_record( &location_record );
        mbedtls_test_fail( location_record.failure_condition,
                           location_record.line,
                           location_record.file );
    }

    mbedtls_test_param_failed_reset_state( );
#else
    fp( params );
#endif

#if defined(MBEDTLS_TEST_MUTEX_USAGE)
    mbedtls_test_mutex_usage_check( );
#endif /* MBEDTLS_TEST_MUTEX_USAGE */
}

/**
 * \brief        Dispatches test functions based on function index.
 *
 * \param func_idx    Test function index.
 * \param params      The array of parameters to pass to the test function.
 *                    It will be decoded by the #TestWrapper_t wrapper function.
 *
 * \return       DISPATCH_TEST_SUCCESS if found
 *               DISPATCH_TEST_FN_NOT_FOUND if not found
 *               DISPATCH_UNSUPPORTED_SUITE if not compile time enabled.
 */
int dispatch_test( size_t func_idx, void ** params )
{
    int ret = DISPATCH_TEST_SUCCESS;
    TestWrapper_t fp = NULL;

    if ( func_idx < (int)( sizeof( test_funcs ) / sizeof( TestWrapper_t ) ) )
    {
        fp = test_funcs[func_idx];
        if ( fp )
            execute_function_ptr(fp, params);
        else
            ret = DISPATCH_UNSUPPORTED_SUITE;
    }
    else
    {
        ret = DISPATCH_TEST_FN_NOT_FOUND;
    }

    return( ret );
}


/**
 * \brief       Checks if test function is supported in this build-time
 *              configuration.
 *
 * \param func_idx    Test function index.
 *
 * \return       DISPATCH_TEST_SUCCESS if found
 *               DISPATCH_TEST_FN_NOT_FOUND if not found
 *               DISPATCH_UNSUPPORTED_SUITE if not compile time enabled.
 */
int check_test( size_t func_idx )
{
    int ret = DISPATCH_TEST_SUCCESS;
    TestWrapper_t fp = NULL;

    if ( func_idx < (int)( sizeof(test_funcs)/sizeof( TestWrapper_t ) ) )
    {
        fp = test_funcs[func_idx];
        if ( fp == NULL )
            ret = DISPATCH_UNSUPPORTED_SUITE;
    }
    else
    {
        ret = DISPATCH_TEST_FN_NOT_FOUND;
    }

    return( ret );
}

int main( int argc, const char *argv[] )
{
    int ret;
    mbedtls_test_platform_setup();
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_pk.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
