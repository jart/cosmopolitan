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
 * Test file      : ./test_suite_pkcs1_v21.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_pkcs1_v21.function
 *      Test suite data     : suites/test_suite_pkcs1_v21.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_PKCS1_V21)
#if defined(MBEDTLS_RSA_C)
#if defined(MBEDTLS_SHA1_C)
#include "third_party/mbedtls/rsa.h"
#include "third_party/mbedtls/md.h"
void test_pkcs1_rsaes_oaep_encrypt( int mod, int radix_N, char * input_N,
                               int radix_E, char * input_E, int hash,
                               data_t * message_str, data_t * rnd_buf,
                               data_t * result_str, int result )
{
    unsigned char output[256];
    mbedtls_rsa_context ctx;
    mbedtls_test_rnd_buf_info info;
    mbedtls_mpi N, E;

    info.buf = rnd_buf->x;
    info.length = rnd_buf->len;

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &E );
    mbedtls_rsa_init( &ctx, MBEDTLS_RSA_PKCS_V21, hash );
    memset( output, 0x00, sizeof( output ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, NULL, NULL, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( ( mod + 7 ) / 8 ) );
    TEST_ASSERT( mbedtls_rsa_check_pubkey( &ctx ) == 0 );

    if( message_str->len == 0 )
        message_str->x = NULL;
    TEST_ASSERT( mbedtls_rsa_pkcs1_encrypt( &ctx,
                                            &mbedtls_test_rnd_buffer_rand,
                                            &info, MBEDTLS_RSA_PUBLIC,
                                            message_str->len, message_str->x,
                                            output ) == result );
    if( result == 0 )
    {
        TEST_ASSERT( mbedtls_test_hexcmp( output, result_str->x,
                                          ctx.len, result_str->len ) == 0 );
    }

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &E );
    mbedtls_rsa_free( &ctx );
}

void test_pkcs1_rsaes_oaep_encrypt_wrapper( void ** params )
{
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};

    test_pkcs1_rsaes_oaep_encrypt( *( (int *) params[0] ), *( (int *) params[1] ), (char *) params[2], *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ), &data6, &data8, &data10, *( (int *) params[12] ) );
}
void test_pkcs1_rsaes_oaep_decrypt( int mod, int radix_P, char * input_P,
                               int radix_Q, char * input_Q, int radix_N,
                               char * input_N, int radix_E, char * input_E,
                               int hash, data_t * result_str,
                               char * seed, data_t * message_str,
                               int result )
{
    unsigned char output[64];
    mbedtls_rsa_context ctx;
    size_t output_len;
    mbedtls_test_rnd_pseudo_info rnd_info;
    mbedtls_mpi N, P, Q, E;
    ((void) seed);

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &P );
    mbedtls_mpi_init( &Q ); mbedtls_mpi_init( &E );

    mbedtls_rsa_init( &ctx, MBEDTLS_RSA_PKCS_V21, hash );

    memset( output, 0x00, sizeof( output ) );
    memset( &rnd_info, 0, sizeof( mbedtls_test_rnd_pseudo_info ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &P, radix_P, input_P ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Q, radix_Q, input_Q ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, &P, &Q, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( ( mod + 7 ) / 8 ) );
    TEST_ASSERT( mbedtls_rsa_complete( &ctx ) == 0 );
    TEST_ASSERT( mbedtls_rsa_check_privkey( &ctx ) == 0 );

    if( result_str->len == 0 )
    {
        TEST_ASSERT( mbedtls_rsa_pkcs1_decrypt( &ctx,
                                                &mbedtls_test_rnd_pseudo_rand,
                                                &rnd_info,
                                                MBEDTLS_RSA_PRIVATE,
                                                &output_len, message_str->x,
                                                NULL, 0 ) == result );
    }
    else
    {
        TEST_ASSERT( mbedtls_rsa_pkcs1_decrypt( &ctx,
                                                &mbedtls_test_rnd_pseudo_rand,
                                                &rnd_info,
                                                MBEDTLS_RSA_PRIVATE,
                                                &output_len, message_str->x,
                                                output,
                                                sizeof( output ) ) == result );
        if( result == 0 )
        {
            TEST_ASSERT( mbedtls_test_hexcmp( output, result_str->x,
                                              output_len,
                                              result_str->len ) == 0 );
        }
    }

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &P );
    mbedtls_mpi_free( &Q ); mbedtls_mpi_free( &E );
    mbedtls_rsa_free( &ctx );
}

void test_pkcs1_rsaes_oaep_decrypt_wrapper( void ** params )
{
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};
    data_t data13 = {(uint8_t *) params[13], *( (uint32_t *) params[14] )};

    test_pkcs1_rsaes_oaep_decrypt( *( (int *) params[0] ), *( (int *) params[1] ), (char *) params[2], *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ), (char *) params[6], *( (int *) params[7] ), (char *) params[8], *( (int *) params[9] ), &data10, (char *) params[12], &data13, *( (int *) params[15] ) );
}
void test_pkcs1_rsassa_pss_sign( int mod, int radix_P, char * input_P, int radix_Q,
                            char * input_Q, int radix_N, char * input_N,
                            int radix_E, char * input_E, int digest, int hash,
                            data_t * message_str, data_t * rnd_buf,
                            data_t * result_str, int result )
{
    unsigned char hash_result[MBEDTLS_MD_MAX_SIZE];
    unsigned char output[256];
    mbedtls_rsa_context ctx;
    mbedtls_test_rnd_buf_info info;
    mbedtls_mpi N, P, Q, E;

    info.buf = rnd_buf->x;
    info.length = rnd_buf->len;

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &P );
    mbedtls_mpi_init( &Q ); mbedtls_mpi_init( &E );
    mbedtls_rsa_init( &ctx, MBEDTLS_RSA_PKCS_V21, hash );

    memset( hash_result, 0x00, sizeof( hash_result ) );
    memset( output, 0x00, sizeof( output ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &P, radix_P, input_P ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &Q, radix_Q, input_Q ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, &P, &Q, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( ( mod + 7 ) / 8 ) );
    TEST_ASSERT( mbedtls_rsa_complete( &ctx ) == 0 );
    TEST_ASSERT( mbedtls_rsa_check_privkey( &ctx ) == 0 );


    if( mbedtls_md_info_from_type( digest ) != NULL )
        TEST_ASSERT( mbedtls_md( mbedtls_md_info_from_type( digest ), message_str->x, message_str->len, hash_result ) == 0 );

    TEST_ASSERT( mbedtls_rsa_pkcs1_sign( &ctx, &mbedtls_test_rnd_buffer_rand,
                                         &info, MBEDTLS_RSA_PRIVATE, digest, 0,
                                         hash_result, output ) == result );
    if( result == 0 )
    {

        TEST_ASSERT( mbedtls_test_hexcmp( output, result_str->x,
                                          ctx.len, result_str->len ) == 0 );
    }

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &P );
    mbedtls_mpi_free( &Q ); mbedtls_mpi_free( &E );
    mbedtls_rsa_free( &ctx );
}

void test_pkcs1_rsassa_pss_sign_wrapper( void ** params )
{
    data_t data11 = {(uint8_t *) params[11], *( (uint32_t *) params[12] )};
    data_t data13 = {(uint8_t *) params[13], *( (uint32_t *) params[14] )};
    data_t data15 = {(uint8_t *) params[15], *( (uint32_t *) params[16] )};

    test_pkcs1_rsassa_pss_sign( *( (int *) params[0] ), *( (int *) params[1] ), (char *) params[2], *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ), (char *) params[6], *( (int *) params[7] ), (char *) params[8], *( (int *) params[9] ), *( (int *) params[10] ), &data11, &data13, &data15, *( (int *) params[17] ) );
}
void test_pkcs1_rsassa_pss_verify( int mod, int radix_N, char * input_N,
                              int radix_E, char * input_E, int digest,
                              int hash, data_t * message_str, char * salt,
                              data_t * result_str, int result )
{
    unsigned char hash_result[MBEDTLS_MD_MAX_SIZE];
    mbedtls_rsa_context ctx;
    mbedtls_mpi N, E;
    ((void) salt);

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &E );
    mbedtls_rsa_init( &ctx, MBEDTLS_RSA_PKCS_V21, hash );
    memset( hash_result, 0x00, sizeof( hash_result ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, NULL, NULL, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( ( mod + 7 ) / 8 ) );
    TEST_ASSERT( mbedtls_rsa_check_pubkey( &ctx ) == 0 );


    if( mbedtls_md_info_from_type( digest ) != NULL )
        TEST_ASSERT( mbedtls_md( mbedtls_md_info_from_type( digest ), message_str->x, message_str->len, hash_result ) == 0 );

    TEST_ASSERT( mbedtls_rsa_pkcs1_verify( &ctx, NULL, NULL, MBEDTLS_RSA_PUBLIC, digest, 0, hash_result, result_str->x ) == result );

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &E );
    mbedtls_rsa_free( &ctx );
}

void test_pkcs1_rsassa_pss_verify_wrapper( void ** params )
{
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};

    test_pkcs1_rsassa_pss_verify( *( (int *) params[0] ), *( (int *) params[1] ), (char *) params[2], *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ), *( (int *) params[6] ), &data7, (char *) params[9], &data10, *( (int *) params[12] ) );
}
void test_pkcs1_rsassa_pss_verify_ext( int mod, int radix_N, char * input_N,
                                  int radix_E, char * input_E,
                                  int msg_digest_id, int ctx_hash,
                                  int mgf_hash, int salt_len,
                                  data_t * message_str,
                                  data_t * result_str, int result_simple,
                                  int result_full )
{
    unsigned char hash_result[MBEDTLS_MD_MAX_SIZE];
    mbedtls_rsa_context ctx;
    size_t hash_len;
    mbedtls_mpi N, E;

    mbedtls_mpi_init( &N ); mbedtls_mpi_init( &E );
    mbedtls_rsa_init( &ctx, MBEDTLS_RSA_PKCS_V21, ctx_hash );
    memset( hash_result, 0x00, sizeof( hash_result ) );

    TEST_ASSERT( mbedtls_mpi_read_string( &N, radix_N, input_N ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &E, radix_E, input_E ) == 0 );

    TEST_ASSERT( mbedtls_rsa_import( &ctx, &N, NULL, NULL, NULL, &E ) == 0 );
    TEST_ASSERT( mbedtls_rsa_get_len( &ctx ) == (size_t) ( ( mod + 7 ) / 8 ) );
    TEST_ASSERT( mbedtls_rsa_check_pubkey( &ctx ) == 0 );


    if( msg_digest_id != MBEDTLS_MD_NONE )
    {
        TEST_ASSERT( mbedtls_md( mbedtls_md_info_from_type( msg_digest_id ),
                     message_str->x, message_str->len, hash_result ) == 0 );
        hash_len = 0;
    }
    else
    {
        memcpy( hash_result, message_str->x, message_str->len );
        hash_len = message_str->len;
    }

    TEST_ASSERT( mbedtls_rsa_pkcs1_verify( &ctx, NULL, NULL, MBEDTLS_RSA_PUBLIC,
                                   msg_digest_id, hash_len, hash_result,
                                   result_str->x ) == result_simple );

    TEST_ASSERT( mbedtls_rsa_rsassa_pss_verify_ext( &ctx, NULL, NULL, MBEDTLS_RSA_PUBLIC,
                                        msg_digest_id, hash_len, hash_result,
                                        mgf_hash, salt_len,
                                        result_str->x ) == result_full );

exit:
    mbedtls_mpi_free( &N ); mbedtls_mpi_free( &E );
    mbedtls_rsa_free( &ctx );
}

void test_pkcs1_rsassa_pss_verify_ext_wrapper( void ** params )
{
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};
    data_t data11 = {(uint8_t *) params[11], *( (uint32_t *) params[12] )};

    test_pkcs1_rsassa_pss_verify_ext( *( (int *) params[0] ), *( (int *) params[1] ), (char *) params[2], *( (int *) params[3] ), (char *) params[4], *( (int *) params[5] ), *( (int *) params[6] ), *( (int *) params[7] ), *( (int *) params[8] ), &data9, &data11, *( (int *) params[13] ), *( (int *) params[14] ) );
}
#endif /* MBEDTLS_SHA1_C */
#endif /* MBEDTLS_RSA_C */
#endif /* MBEDTLS_PKCS1_V21 */

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

#if defined(MBEDTLS_PKCS1_V21) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_SHA1_C)

        case 0:
            {
                *out_value = MBEDTLS_MD_SHA1;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_ERR_RSA_BAD_INPUT_DATA;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_MD_SHA512;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_MD_NONE;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_MD_SHA256;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_ERR_RSA_INVALID_PADDING;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_ERR_RSA_VERIFY_FAILED;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_RSA_SALT_LEN_ANY;
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

#if defined(MBEDTLS_PKCS1_V21) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_SHA1_C)

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
#if defined(MBEDTLS_SHA512_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_SHA256_C)
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

#if defined(MBEDTLS_PKCS1_V21) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_SHA1_C)
    test_pkcs1_rsaes_oaep_encrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_PKCS1_V21) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_SHA1_C)
    test_pkcs1_rsaes_oaep_decrypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_PKCS1_V21) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_SHA1_C)
    test_pkcs1_rsassa_pss_sign_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_PKCS1_V21) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_SHA1_C)
    test_pkcs1_rsassa_pss_verify_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_PKCS1_V21) && defined(MBEDTLS_RSA_C) && defined(MBEDTLS_SHA1_C)
    test_pkcs1_rsassa_pss_verify_ext_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_pkcs1_v21.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
