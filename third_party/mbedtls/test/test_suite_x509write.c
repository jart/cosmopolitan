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
 * Test file      : ./test_suite_x509write.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_x509write.function
 *      Test suite data     : suites/test_suite_x509write.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_BIGNUM_C)
#if defined(MBEDTLS_FS_IO)
#if defined(MBEDTLS_PK_PARSE_C)
#include "third_party/mbedtls/bignum.h"
#include "third_party/mbedtls/x509_crt.h"
#include "third_party/mbedtls/x509_csr.h"
#include "third_party/mbedtls/pem.h"
#include "third_party/mbedtls/oid.h"
#include "third_party/mbedtls/rsa.h"

#if defined(MBEDTLS_RSA_C)
int mbedtls_rsa_decrypt_func( void *ctx, int mode, size_t *olen,
                       const unsigned char *input, unsigned char *output,
                       size_t output_max_len )
{
    return( mbedtls_rsa_pkcs1_decrypt( (mbedtls_rsa_context *) ctx, NULL, NULL, mode, olen,
                               input, output, output_max_len ) );
}
int mbedtls_rsa_sign_func( void *ctx,
                   int (*f_rng)(void *, unsigned char *, size_t), void *p_rng,
                   int mode, mbedtls_md_type_t md_alg, unsigned int hashlen,
                   const unsigned char *hash, unsigned char *sig )
{
    return( mbedtls_rsa_pkcs1_sign( (mbedtls_rsa_context *) ctx, f_rng, p_rng, mode,
                            md_alg, hashlen, hash, sig ) );
}
size_t mbedtls_rsa_key_len_func( void *ctx )
{
    return( ((const mbedtls_rsa_context *) ctx)->len );
}
#endif /* MBEDTLS_RSA_C */

#if defined(MBEDTLS_USE_PSA_CRYPTO) && \
    defined(MBEDTLS_PEM_WRITE_C) && defined(MBEDTLS_X509_CSR_WRITE_C)
static int x509_crt_verifycsr( const unsigned char *buf, size_t buflen )
{
    unsigned char hash[MBEDTLS_MD_MAX_SIZE];
    const mbedtls_md_info_t *md_info;
    mbedtls_x509_csr csr;
    int ret = 0;

    mbedtls_x509_csr_init( &csr );

    if( mbedtls_x509_csr_parse( &csr, buf, buflen ) != 0 )
    {
        ret = MBEDTLS_ERR_X509_BAD_INPUT_DATA;
        goto cleanup;
    }

    md_info = mbedtls_md_info_from_type( csr.sig_md );
    if( mbedtls_md( md_info, csr.cri.p, csr.cri.len, hash ) != 0 )
    {
        /* Note: this can't happen except after an internal error */
        ret = MBEDTLS_ERR_X509_BAD_INPUT_DATA;
        goto cleanup;
    }

    if( mbedtls_pk_verify_ext( csr.sig_pk, csr.sig_opts, &csr.pk,
                       csr.sig_md, hash, mbedtls_md_get_size( md_info ),
                       csr.sig.p, csr.sig.len ) != 0 )
    {
        ret = MBEDTLS_ERR_X509_CERT_VERIFY_FAILED;
        goto cleanup;
    }

cleanup:

    mbedtls_x509_csr_free( &csr );
    return( ret );
}
#endif /* MBEDTLS_USE_PSA_CRYPTO && MBEDTLS_PEM_WRITE_C && MBEDTLS_X509_CSR_WRITE_C */

#if defined(MBEDTLS_PEM_WRITE_C)
#if defined(MBEDTLS_X509_CSR_WRITE_C)
void test_x509_csr_check( char * key_file, char * cert_req_check_file, int md_type,
                     int key_usage, int set_key_usage, int cert_type,
                     int set_cert_type )
{
    mbedtls_pk_context key;
    mbedtls_x509write_csr req;
    unsigned char buf[4096];
    unsigned char check_buf[4000];
    int ret;
    size_t olen = 0, pem_len = 0, buf_index;
    int der_len = -1;
    FILE *f;
    const char *subject_name = "C=NL,O=PolarSSL,CN=PolarSSL Server 1";
    mbedtls_test_rnd_pseudo_info rnd_info;

    memset( &rnd_info, 0x2a, sizeof( mbedtls_test_rnd_pseudo_info ) );

    mbedtls_pk_init( &key );
    TEST_ASSERT( mbedtls_pk_parse_keyfile( &key, key_file, NULL ) == 0 );

    mbedtls_x509write_csr_init( &req );
    mbedtls_x509write_csr_set_md_alg( &req, md_type );
    mbedtls_x509write_csr_set_key( &req, &key );
    TEST_ASSERT( mbedtls_x509write_csr_set_subject_name( &req, subject_name ) == 0 );
    if( set_key_usage != 0 )
        TEST_ASSERT( mbedtls_x509write_csr_set_key_usage( &req, key_usage ) == 0 );
    if( set_cert_type != 0 )
        TEST_ASSERT( mbedtls_x509write_csr_set_ns_cert_type( &req, cert_type ) == 0 );

    ret = mbedtls_x509write_csr_pem( &req, buf, sizeof( buf ),
                                     mbedtls_test_rnd_pseudo_rand, &rnd_info );
    TEST_ASSERT( ret == 0 );

    pem_len = strlen( (char *) buf );

    for( buf_index = pem_len; buf_index < sizeof( buf ); ++buf_index )
    {
        TEST_ASSERT( buf[buf_index] == 0 );
    }

    f = fopen( cert_req_check_file, "r" );
    TEST_ASSERT( f != NULL );
    olen = fread( check_buf, 1, sizeof( check_buf ), f );
    fclose( f );

    TEST_ASSERT( olen >= pem_len - 1 );
    TEST_ASSERT( memcmp( buf, check_buf, pem_len - 1 ) == 0 );

    der_len = mbedtls_x509write_csr_der( &req, buf, sizeof( buf ),
                                         mbedtls_test_rnd_pseudo_rand,
                                         &rnd_info );
    TEST_ASSERT( der_len >= 0 );

    if( der_len == 0 )
        goto exit;

    ret = mbedtls_x509write_csr_der( &req, buf, (size_t)( der_len - 1 ),
                                     mbedtls_test_rnd_pseudo_rand, &rnd_info );
    TEST_ASSERT( ret == MBEDTLS_ERR_ASN1_BUF_TOO_SMALL );

exit:
    mbedtls_x509write_csr_free( &req );
    mbedtls_pk_free( &key );
}

void test_x509_csr_check_wrapper( void ** params )
{

    test_x509_csr_check( (char *) params[0], (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ), *( (int *) params[4] ), *( (int *) params[5] ), *( (int *) params[6] ) );
}
#endif /* MBEDTLS_X509_CSR_WRITE_C */
#endif /* MBEDTLS_PEM_WRITE_C */
#if defined(MBEDTLS_PEM_WRITE_C)
#if defined(MBEDTLS_X509_CSR_WRITE_C)
#if defined(MBEDTLS_USE_PSA_CRYPTO)
void test_x509_csr_check_opaque( char *key_file, int md_type, int key_usage,
                                 int cert_type )
{
    mbedtls_pk_context key;
    mbedtls_svc_key_id_t key_id = MBEDTLS_SVC_KEY_ID_INIT;
    psa_algorithm_t md_alg_psa;
    mbedtls_x509write_csr req;
    unsigned char buf[4096];
    int ret;
    size_t pem_len = 0;
    const char *subject_name = "C=NL,O=PolarSSL,CN=PolarSSL Server 1";
    mbedtls_test_rnd_pseudo_info rnd_info;

    PSA_INIT( );
    memset( &rnd_info, 0x2a, sizeof( mbedtls_test_rnd_pseudo_info ) );

    md_alg_psa = mbedtls_psa_translate_md( (mbedtls_md_type_t) md_type );
    TEST_ASSERT( md_alg_psa != MBEDTLS_MD_NONE );

    mbedtls_pk_init( &key );
    TEST_ASSERT( mbedtls_pk_parse_keyfile( &key, key_file, NULL ) == 0 );
    TEST_ASSERT( mbedtls_pk_wrap_as_opaque( &key, &key_id, md_alg_psa ) == 0 );

    mbedtls_x509write_csr_init( &req );
    mbedtls_x509write_csr_set_md_alg( &req, md_type );
    mbedtls_x509write_csr_set_key( &req, &key );
    TEST_ASSERT( mbedtls_x509write_csr_set_subject_name( &req, subject_name ) == 0 );
    if( key_usage != 0 )
        TEST_ASSERT( mbedtls_x509write_csr_set_key_usage( &req, key_usage ) == 0 );
    if( cert_type != 0 )
        TEST_ASSERT( mbedtls_x509write_csr_set_ns_cert_type( &req, cert_type ) == 0 );

    ret = mbedtls_x509write_csr_pem( &req, buf, sizeof( buf ) - 1,
                                     mbedtls_test_rnd_pseudo_rand, &rnd_info );

    TEST_ASSERT( ret == 0 );

    pem_len = strlen( (char *) buf );
    buf[pem_len] = '\0';
    TEST_ASSERT( x509_crt_verifycsr( buf, pem_len + 1 ) == 0 );


exit:
    mbedtls_x509write_csr_free( &req );
    mbedtls_pk_free( &key );
    psa_destroy_key( key_id );
    PSA_DONE( );
}

void test_x509_csr_check_opaque_wrapper( void ** params )
{

    test_x509_csr_check_opaque( (char *) params[0], *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ) );
}
#endif /* MBEDTLS_USE_PSA_CRYPTO */
#endif /* MBEDTLS_X509_CSR_WRITE_C */
#endif /* MBEDTLS_PEM_WRITE_C */
#if defined(MBEDTLS_PEM_WRITE_C)
#if defined(MBEDTLS_X509_CRT_WRITE_C)
#if defined(MBEDTLS_SHA1_C)
void test_x509_crt_check( char *subject_key_file, char *subject_pwd,
                     char *subject_name, char *issuer_key_file,
                     char *issuer_pwd, char *issuer_name,
                     char *serial_str, char *not_before, char *not_after,
                     int md_type, int key_usage, int set_key_usage,
                     int cert_type, int set_cert_type, int auth_ident,
                     int ver, char *cert_check_file, int rsa_alt, int is_ca )
{
    mbedtls_pk_context subject_key, issuer_key, issuer_key_alt;
    mbedtls_pk_context *key = &issuer_key;

    mbedtls_x509write_cert crt;
    unsigned char buf[4096];
    unsigned char check_buf[5000];
    mbedtls_mpi serial;
    int ret;
    size_t olen = 0, pem_len = 0, buf_index = 0;
    int der_len = -1;
    FILE *f;
    mbedtls_test_rnd_pseudo_info rnd_info;

    memset( &rnd_info, 0x2a, sizeof( mbedtls_test_rnd_pseudo_info ) );
    mbedtls_mpi_init( &serial );

    mbedtls_pk_init( &subject_key );
    mbedtls_pk_init( &issuer_key  );
    mbedtls_pk_init( &issuer_key_alt );

    mbedtls_x509write_crt_init( &crt );

    TEST_ASSERT( mbedtls_pk_parse_keyfile( &subject_key, subject_key_file,
                                         subject_pwd ) == 0 );

    TEST_ASSERT( mbedtls_pk_parse_keyfile( &issuer_key, issuer_key_file,
                                         issuer_pwd ) == 0 );

#if defined(MBEDTLS_RSA_C)
    /* For RSA PK contexts, create a copy as an alternative RSA context. */
    if( rsa_alt == 1 && mbedtls_pk_get_type( &issuer_key ) == MBEDTLS_PK_RSA )
    {
        TEST_ASSERT( mbedtls_pk_setup_rsa_alt( &issuer_key_alt,
                                            mbedtls_pk_rsa( issuer_key ),
                                            mbedtls_rsa_decrypt_func,
                                            mbedtls_rsa_sign_func,
                                            mbedtls_rsa_key_len_func ) == 0 );

        key = &issuer_key_alt;
    }
#else
    (void) rsa_alt;
#endif

    TEST_ASSERT( mbedtls_mpi_read_string( &serial, 10, serial_str ) == 0 );

    if( ver != -1 )
        mbedtls_x509write_crt_set_version( &crt, ver );

    TEST_ASSERT( mbedtls_x509write_crt_set_serial( &crt, &serial ) == 0 );
    TEST_ASSERT( mbedtls_x509write_crt_set_validity( &crt, not_before,
                                                     not_after ) == 0 );
    mbedtls_x509write_crt_set_md_alg( &crt, md_type );
    TEST_ASSERT( mbedtls_x509write_crt_set_issuer_name( &crt, issuer_name ) == 0 );
    TEST_ASSERT( mbedtls_x509write_crt_set_subject_name( &crt, subject_name ) == 0 );
    mbedtls_x509write_crt_set_subject_key( &crt, &subject_key );

    mbedtls_x509write_crt_set_issuer_key( &crt, key );

    if( crt.version >= MBEDTLS_X509_CRT_VERSION_3 )
    {
        /* For the CA case, a path length of -1 means unlimited. */
        TEST_ASSERT( mbedtls_x509write_crt_set_basic_constraints( &crt, is_ca,
                                                                  (is_ca ? -1 : 0) ) == 0 );
        TEST_ASSERT( mbedtls_x509write_crt_set_subject_key_identifier( &crt ) == 0 );
        if( auth_ident )
            TEST_ASSERT( mbedtls_x509write_crt_set_authority_key_identifier( &crt ) == 0 );
        if( set_key_usage != 0 )
            TEST_ASSERT( mbedtls_x509write_crt_set_key_usage( &crt, key_usage ) == 0 );
        if( set_cert_type != 0 )
            TEST_ASSERT( mbedtls_x509write_crt_set_ns_cert_type( &crt, cert_type ) == 0 );
    }

    ret = mbedtls_x509write_crt_pem( &crt, buf, sizeof( buf ),
                                     mbedtls_test_rnd_pseudo_rand, &rnd_info );
    TEST_ASSERT( ret == 0 );

    pem_len = strlen( (char *) buf );

    // check that the rest of the buffer remains clear
    for( buf_index = pem_len; buf_index < sizeof( buf ); ++buf_index )
    {
        TEST_ASSERT( buf[buf_index] == 0 );
    }

    f = fopen( cert_check_file, "r" );
    TEST_ASSERT( f != NULL );
    olen = fread( check_buf, 1, sizeof( check_buf ), f );
    fclose( f );
    TEST_ASSERT( olen < sizeof( check_buf ) );

    TEST_ASSERT( olen >= pem_len - 1 );
    TEST_ASSERT( memcmp( buf, check_buf, pem_len - 1 ) == 0 );

    der_len = mbedtls_x509write_crt_der( &crt, buf, sizeof( buf ),
                                         mbedtls_test_rnd_pseudo_rand,
                                         &rnd_info );
    TEST_ASSERT( der_len >= 0 );

    if( der_len == 0 )
        goto exit;

    ret = mbedtls_x509write_crt_der( &crt, buf, (size_t)( der_len - 1 ),
                                     mbedtls_test_rnd_pseudo_rand, &rnd_info );
    TEST_ASSERT( ret == MBEDTLS_ERR_ASN1_BUF_TOO_SMALL );

exit:
    mbedtls_x509write_crt_free( &crt );
    mbedtls_pk_free( &issuer_key_alt );
    mbedtls_pk_free( &subject_key );
    mbedtls_pk_free( &issuer_key );
    mbedtls_mpi_free( &serial );
}

void test_x509_crt_check_wrapper( void ** params )
{

    test_x509_crt_check( (char *) params[0], (char *) params[1], (char *) params[2], (char *) params[3], (char *) params[4], (char *) params[5], (char *) params[6], (char *) params[7], (char *) params[8], *( (int *) params[9] ), *( (int *) params[10] ), *( (int *) params[11] ), *( (int *) params[12] ), *( (int *) params[13] ), *( (int *) params[14] ), *( (int *) params[15] ), (char *) params[16], *( (int *) params[17] ), *( (int *) params[18] ) );
}
#endif /* MBEDTLS_SHA1_C */
#endif /* MBEDTLS_X509_CRT_WRITE_C */
#endif /* MBEDTLS_PEM_WRITE_C */
#if defined(MBEDTLS_X509_CREATE_C)
#if defined(MBEDTLS_X509_USE_C)
void test_mbedtls_x509_string_to_names( char * name, char * parsed_name, int result
                                   )
{
    int ret;
    size_t len = 0;
    mbedtls_asn1_named_data *names = NULL;
    mbedtls_x509_name parsed, *parsed_cur, *parsed_prv;
    unsigned char buf[1024], out[1024], *c;

    memset( &parsed, 0, sizeof( parsed ) );
    memset( out, 0, sizeof( out ) );
    memset( buf, 0, sizeof( buf ) );
    c = buf + sizeof( buf );

    ret = mbedtls_x509_string_to_names( &names, name );
    TEST_ASSERT( ret == result );

    if( ret != 0 )
        goto exit;

    ret = mbedtls_x509_write_names( &c, buf, names );
    TEST_ASSERT( ret > 0 );

    TEST_ASSERT( mbedtls_asn1_get_tag( &c, buf + sizeof( buf ), &len,
                        MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE ) == 0 );
    TEST_ASSERT( mbedtls_x509_get_name( &c, buf + sizeof( buf ), &parsed ) == 0 );

    ret = mbedtls_x509_dn_gets( (char *) out, sizeof( out ), &parsed );
    TEST_ASSERT( ret > 0 );

    TEST_ASSERT( strcmp( (char *) out, parsed_name ) == 0 );

exit:
    mbedtls_asn1_free_named_data_list( &names );

    parsed_cur = parsed.next;
    while( parsed_cur != 0 )
    {
        parsed_prv = parsed_cur;
        parsed_cur = parsed_cur->next;
        mbedtls_free( parsed_prv );
    }
}

void test_mbedtls_x509_string_to_names_wrapper( void ** params )
{

    test_mbedtls_x509_string_to_names( (char *) params[0], (char *) params[1], *( (int *) params[2] ) );
}
#endif /* MBEDTLS_X509_USE_C */
#endif /* MBEDTLS_X509_CREATE_C */
#endif /* MBEDTLS_PK_PARSE_C */
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_BIGNUM_C */

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

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_PK_PARSE_C)

        case 0:
            {
                *out_value = MBEDTLS_MD_SHA1;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_MD_SHA224;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_MD_SHA256;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_MD_SHA384;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_MD_SHA512;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_MD_MD4;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_MD_MD5;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_X509_KU_DIGITAL_SIGNATURE | MBEDTLS_X509_KU_NON_REPUDIATION | MBEDTLS_X509_KU_KEY_ENCIPHERMENT;
            }
            break;
        case 8:
            {
                *out_value = MBEDTLS_X509_NS_CERT_TYPE_SSL_SERVER;
            }
            break;
        case 9:
            {
                *out_value = MBEDTLS_X509_KU_DIGITAL_SIGNATURE | MBEDTLS_X509_KU_NON_REPUDIATION;
            }
            break;
        case 10:
            {
                *out_value = -1;
            }
            break;
        case 11:
            {
                *out_value = MBEDTLS_X509_CRT_VERSION_1;
            }
            break;
        case 12:
            {
                *out_value = MBEDTLS_ERR_X509_UNKNOWN_OID;
            }
            break;
        case 13:
            {
                *out_value = MBEDTLS_ERR_X509_INVALID_NAME;
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

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_PK_PARSE_C)

        case 0:
            {
#if defined(MBEDTLS_SHA1_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_RSA_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_PKCS1_V15)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if defined(MBEDTLS_SHA256_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if defined(MBEDTLS_SHA512_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 5:
            {
#if !defined(MBEDTLS_SHA512_NO_SHA384)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 6:
            {
#if defined(MBEDTLS_MD4_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 7:
            {
#if defined(MBEDTLS_MD5_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 8:
            {
#if defined(MBEDTLS_ECDSA_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 9:
            {
#if defined(MBEDTLS_ECDSA_DETERMINISTIC)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 10:
            {
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 11:
            {
#if defined(MBEDTLS_DES_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 12:
            {
#if defined(MBEDTLS_CIPHER_MODE_CBC)
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

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_PEM_WRITE_C) && defined(MBEDTLS_X509_CSR_WRITE_C)
    test_x509_csr_check_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_PEM_WRITE_C) && defined(MBEDTLS_X509_CSR_WRITE_C) && defined(MBEDTLS_USE_PSA_CRYPTO)
    test_x509_csr_check_opaque_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_PEM_WRITE_C) && defined(MBEDTLS_X509_CRT_WRITE_C) && defined(MBEDTLS_SHA1_C)
    test_x509_crt_check_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO) && defined(MBEDTLS_PK_PARSE_C) && defined(MBEDTLS_X509_CREATE_C) && defined(MBEDTLS_X509_USE_C)
    test_mbedtls_x509_string_to_names_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_x509write.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
