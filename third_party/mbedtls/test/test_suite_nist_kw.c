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
 * Test file      : ./test_suite_nist_kw.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_nist_kw.function
 *      Test suite data     : suites/test_suite_nist_kw.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_NIST_KW_C)
#include "third_party/mbedtls/nist_kw.h"
#if defined(MBEDTLS_SELF_TEST)
#if defined(MBEDTLS_AES_C)
void test_mbedtls_nist_kw_self_test( )
{
    TEST_ASSERT( mbedtls_nist_kw_self_test( 1 ) == 0 );
exit:
    ;
}

void test_mbedtls_nist_kw_self_test_wrapper( void ** params )
{
    (void)params;

    test_mbedtls_nist_kw_self_test(  );
}
#endif /* MBEDTLS_AES_C */
#endif /* MBEDTLS_SELF_TEST */
#if defined(MBEDTLS_AES_C)
void test_mbedtls_nist_kw_mix_contexts( )
{
    mbedtls_nist_kw_context ctx1, ctx2;
    unsigned char key[16];
    unsigned char plaintext[32];
    unsigned char ciphertext1[40];
    unsigned char ciphertext2[40];
    size_t output_len, i;

    memset( plaintext, 0, sizeof( plaintext ) );
    memset( ciphertext1, 0, sizeof( ciphertext1 ) );
    memset( ciphertext2, 0, sizeof( ciphertext2 ) );
    memset( key, 0, sizeof( key ) );

    /*
     * 1. Check wrap and unwrap with two separate contexts
     */
    mbedtls_nist_kw_init( &ctx1 );
    mbedtls_nist_kw_init( &ctx2 );

    TEST_ASSERT( mbedtls_nist_kw_setkey( &ctx1,
                                         MBEDTLS_CIPHER_ID_AES,
                                         key, sizeof( key ) * 8,
                                         1 ) == 0 );

    TEST_ASSERT( mbedtls_nist_kw_wrap( &ctx1, MBEDTLS_KW_MODE_KW,
                                       plaintext, sizeof( plaintext ),
                                       ciphertext1, &output_len,
                                       sizeof( ciphertext1 ) ) == 0 );
    TEST_ASSERT( output_len == sizeof( ciphertext1 ) );

    TEST_ASSERT( mbedtls_nist_kw_setkey( &ctx2,
                                         MBEDTLS_CIPHER_ID_AES,
                                         key, sizeof( key ) * 8,
                                         0 ) == 0 );

    TEST_ASSERT( mbedtls_nist_kw_unwrap( &ctx2, MBEDTLS_KW_MODE_KW,
                                         ciphertext1, output_len,
                                         plaintext, &output_len,
                                         sizeof( plaintext ) ) == 0 );

    TEST_ASSERT( output_len == sizeof( plaintext ) );
    for( i = 0; i < sizeof( plaintext ); i++ )
    {
        TEST_ASSERT( plaintext[i] == 0 );
    }
    mbedtls_nist_kw_free( &ctx1 );
    mbedtls_nist_kw_free( &ctx2 );

    /*
     * 2. Check wrapping with two modes, on same context
     */
    mbedtls_nist_kw_init( &ctx1 );
    mbedtls_nist_kw_init( &ctx2 );
    output_len = sizeof( ciphertext1 );

    TEST_ASSERT( mbedtls_nist_kw_setkey( &ctx1,
                                         MBEDTLS_CIPHER_ID_AES,
                                         key, sizeof( key ) * 8,
                                         1 ) == 0 );

    TEST_ASSERT( mbedtls_nist_kw_wrap( &ctx1, MBEDTLS_KW_MODE_KW,
                                       plaintext, sizeof( plaintext ),
                                       ciphertext1, &output_len,
                                       sizeof( ciphertext1 ) ) == 0 );
    TEST_ASSERT( output_len == sizeof( ciphertext1 ) );

    TEST_ASSERT( mbedtls_nist_kw_wrap( &ctx1, MBEDTLS_KW_MODE_KWP,
                                       plaintext, sizeof( plaintext ),
                                       ciphertext2, &output_len,
                                       sizeof( ciphertext2 ) ) == 0 );

    TEST_ASSERT( output_len == sizeof( ciphertext2 ) );

    TEST_ASSERT( mbedtls_nist_kw_setkey( &ctx2,
                                         MBEDTLS_CIPHER_ID_AES,
                                         key, sizeof( key ) * 8,
                                         0 ) == 0 );

    TEST_ASSERT( mbedtls_nist_kw_unwrap( &ctx2, MBEDTLS_KW_MODE_KW,
                                         ciphertext1, sizeof( ciphertext1 ),
                                         plaintext, &output_len,
                                         sizeof( plaintext ) ) == 0 );

    TEST_ASSERT( output_len == sizeof( plaintext ) );

    for( i = 0; i < sizeof( plaintext ); i++ )
    {
        TEST_ASSERT( plaintext[i] == 0 );
    }

    TEST_ASSERT( mbedtls_nist_kw_unwrap( &ctx2, MBEDTLS_KW_MODE_KWP,
                                         ciphertext2, sizeof( ciphertext2 ),
                                         plaintext, &output_len,
                                         sizeof( plaintext ) ) == 0 );

    TEST_ASSERT( output_len == sizeof( plaintext ) );

    for( i = 0; i < sizeof( plaintext ); i++ )
    {
        TEST_ASSERT( plaintext[i] == 0 );
    }

exit:
    mbedtls_nist_kw_free( &ctx1 );
    mbedtls_nist_kw_free( &ctx2 );
}

void test_mbedtls_nist_kw_mix_contexts_wrapper( void ** params )
{
    (void)params;

    test_mbedtls_nist_kw_mix_contexts(  );
}
#endif /* MBEDTLS_AES_C */
void test_mbedtls_nist_kw_setkey( int cipher_id, int key_size,
                             int is_wrap, int result )
{
    mbedtls_nist_kw_context ctx;
    unsigned char key[32];
    int ret;

    mbedtls_nist_kw_init( &ctx );

    memset( key, 0x2A, sizeof( key ) );
    TEST_ASSERT( (unsigned) key_size <= 8 * sizeof( key ) );

    ret = mbedtls_nist_kw_setkey( &ctx, cipher_id, key, key_size, is_wrap );
    TEST_ASSERT( ret == result );

exit:
    mbedtls_nist_kw_free( &ctx );
}

void test_mbedtls_nist_kw_setkey_wrapper( void ** params )
{

    test_mbedtls_nist_kw_setkey( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ) );
}
#if defined(MBEDTLS_AES_C)
void test_nist_kw_plaintext_lengths( int in_len, int out_len, int mode, int res )
{
    mbedtls_nist_kw_context ctx;
    unsigned char key[16];
    unsigned char *plaintext = NULL;
    unsigned char *ciphertext = NULL;
    size_t output_len = out_len;

    mbedtls_nist_kw_init( &ctx );

    memset( key, 0, sizeof( key ) );

    if( in_len != 0 )
    {
        plaintext = mbedtls_calloc( 1, in_len );
        TEST_ASSERT( plaintext != NULL );
    }

    if( out_len != 0 )
    {
        ciphertext = mbedtls_calloc( 1, output_len );
        TEST_ASSERT( ciphertext != NULL );
    }

    TEST_ASSERT( mbedtls_nist_kw_setkey( &ctx, MBEDTLS_CIPHER_ID_AES,
                                         key, 8 * sizeof( key ), 1 ) == 0 );

    TEST_ASSERT( mbedtls_nist_kw_wrap( &ctx, mode, plaintext, in_len,
                                      ciphertext, &output_len,
                                      output_len ) == res );
    if( res == 0 )
    {
        if( mode == MBEDTLS_KW_MODE_KWP )
            TEST_ASSERT( output_len == (size_t) in_len + 8 -
                         ( in_len % 8 ) + 8 );
        else
            TEST_ASSERT( output_len == (size_t) in_len + 8 );
    }
    else
    {
        TEST_ASSERT( output_len == 0 );
    }

exit:
    mbedtls_free( ciphertext );
    mbedtls_free( plaintext );
    mbedtls_nist_kw_free( &ctx );
}

void test_nist_kw_plaintext_lengths_wrapper( void ** params )
{

    test_nist_kw_plaintext_lengths( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ) );
}
#endif /* MBEDTLS_AES_C */
#if defined(MBEDTLS_AES_C)
void test_nist_kw_ciphertext_lengths( int in_len, int out_len, int mode, int res )
{
    mbedtls_nist_kw_context ctx;
    unsigned char key[16];
    unsigned char *plaintext = NULL;
    unsigned char *ciphertext = NULL;
    int unwrap_ret;
    size_t output_len = out_len;

    mbedtls_nist_kw_init( &ctx );

    memset( key, 0, sizeof( key ) );

    if( out_len != 0 )
    {
        plaintext = mbedtls_calloc( 1, output_len );
        TEST_ASSERT( plaintext != NULL );
    }
    if( in_len != 0 )
    {
        ciphertext = mbedtls_calloc( 1, in_len );
        TEST_ASSERT( ciphertext != NULL );
    }

    TEST_ASSERT( mbedtls_nist_kw_setkey( &ctx, MBEDTLS_CIPHER_ID_AES,
                                         key, 8 * sizeof( key ), 0 ) == 0 );
    unwrap_ret = mbedtls_nist_kw_unwrap( &ctx, mode, ciphertext, in_len,
                                         plaintext, &output_len,
                                         output_len );

    if( res == 0 )
        TEST_ASSERT( unwrap_ret == MBEDTLS_ERR_CIPHER_AUTH_FAILED );
    else
        TEST_ASSERT( unwrap_ret == res );

    TEST_ASSERT( output_len == 0 );

exit:
    mbedtls_free( ciphertext );
    mbedtls_free( plaintext );
    mbedtls_nist_kw_free( &ctx );
}

void test_nist_kw_ciphertext_lengths_wrapper( void ** params )
{

    test_nist_kw_ciphertext_lengths( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ) );
}
#endif /* MBEDTLS_AES_C */
void test_mbedtls_nist_kw_wrap( int cipher_id, int mode, data_t *key, data_t *msg,
                           data_t *expected_result )
{
    unsigned char result[528];
    mbedtls_nist_kw_context ctx;
    size_t result_len, i, padlen;

    mbedtls_nist_kw_init( &ctx );

    memset( result, '+', sizeof( result ) );

    TEST_ASSERT( mbedtls_nist_kw_setkey( &ctx, cipher_id,
                                         key->x, key->len * 8, 1 ) == 0 );

    /* Test with input == output */
    TEST_ASSERT( mbedtls_nist_kw_wrap( &ctx, mode, msg->x, msg->len,
                 result, &result_len, sizeof( result ) ) == 0 );

    TEST_ASSERT( result_len == expected_result->len );

    TEST_ASSERT( memcmp( expected_result->x, result, result_len ) == 0 );

    padlen = ( msg->len % 8 != 0 ) ? 8 - (msg->len % 8 ) : 0;
    /* Check that the function didn't write beyond the end of the buffer. */
    for( i = msg->len + 8 + padlen; i < sizeof( result ); i++ )
    {
        TEST_ASSERT( result[i] == '+' );
    }

exit:
    mbedtls_nist_kw_free( &ctx );
}

void test_mbedtls_nist_kw_wrap_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_mbedtls_nist_kw_wrap( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6 );
}
void test_mbedtls_nist_kw_unwrap( int cipher_id, int mode, data_t *key, data_t *msg,
                             data_t *expected_result, int expected_ret )
{
    unsigned char result[528];
    mbedtls_nist_kw_context ctx;
    size_t result_len, i;

    mbedtls_nist_kw_init( &ctx );

    memset( result, '+', sizeof( result ) );

    TEST_ASSERT( mbedtls_nist_kw_setkey( &ctx, cipher_id,
                                         key->x, key->len * 8, 0 ) == 0 );

    /* Test with input == output */
    TEST_ASSERT( mbedtls_nist_kw_unwrap( &ctx, mode, msg->x, msg->len,
                 result, &result_len, sizeof( result ) ) == expected_ret );
    if( expected_ret == 0 )
    {
        TEST_ASSERT( result_len == expected_result->len );
        TEST_ASSERT( memcmp( expected_result->x, result, result_len ) == 0 );
    }
    else
    {
        TEST_ASSERT( result_len == 0 );
    }

    /* Check that the function didn't write beyond the end of the buffer. */
    for( i = msg->len - 8; i < sizeof( result ); i++ )
    {
        TEST_ASSERT( result[i] == '+' );
    }

exit:
    mbedtls_nist_kw_free( &ctx );
}

void test_mbedtls_nist_kw_unwrap_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_mbedtls_nist_kw_unwrap( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, *( (int *) params[8] ) );
}
#endif /* MBEDTLS_NIST_KW_C */

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

#if defined(MBEDTLS_NIST_KW_C)

        case 0:
            {
                *out_value = MBEDTLS_CIPHER_ID_AES;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_CIPHER_ID_CAMELLIA;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ERR_CIPHER_FEATURE_UNAVAILABLE;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_ERR_CIPHER_BAD_INPUT_DATA;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_CIPHER_ID_BLOWFISH;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_KW_MODE_KW;
            }
            break;
        case 6:
            {
                *out_value = MBEDTLS_KW_MODE_KWP;
            }
            break;
        case 7:
            {
                *out_value = MBEDTLS_ERR_CIPHER_AUTH_FAILED;
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

#if defined(MBEDTLS_NIST_KW_C)

        case 0:
            {
#if defined(MBEDTLS_AES_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_CAMELLIA_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_BLOWFISH_C)
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

#if defined(MBEDTLS_NIST_KW_C) && defined(MBEDTLS_SELF_TEST) && defined(MBEDTLS_AES_C)
    test_mbedtls_nist_kw_self_test_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_NIST_KW_C) && defined(MBEDTLS_AES_C)
    test_mbedtls_nist_kw_mix_contexts_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_NIST_KW_C)
    test_mbedtls_nist_kw_setkey_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_NIST_KW_C) && defined(MBEDTLS_AES_C)
    test_nist_kw_plaintext_lengths_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_NIST_KW_C) && defined(MBEDTLS_AES_C)
    test_nist_kw_ciphertext_lengths_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_NIST_KW_C)
    test_mbedtls_nist_kw_wrap_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_NIST_KW_C)
    test_mbedtls_nist_kw_unwrap_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_nist_kw.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
