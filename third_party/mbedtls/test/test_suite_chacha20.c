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
 * Test file      : ./test_suite_chacha20.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_chacha20.function
 *      Test suite data     : suites/test_suite_chacha20.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_CHACHA20_C)
#include "third_party/mbedtls/chacha20.h"
void test_chacha20_crypt( data_t *key_str,
                     data_t *nonce_str,
                     int counter,
                     data_t *src_str,
                     data_t *expected_output_str )
{
    unsigned char output[375];
    mbedtls_chacha20_context ctx;

    memset( output, 0x00, sizeof( output ) );

    TEST_ASSERT( src_str->len   == expected_output_str->len );
    TEST_ASSERT( key_str->len   == 32U );
    TEST_ASSERT( nonce_str->len == 12U );

    /*
     * Test the integrated API
     */
    TEST_ASSERT( mbedtls_chacha20_crypt( key_str->x, nonce_str->x, counter, src_str->len, src_str->x, output ) == 0 );

    ASSERT_COMPARE( output, expected_output_str->len,
                    expected_output_str->x, expected_output_str->len );

    /*
     * Test the streaming API
     */
    mbedtls_chacha20_init( &ctx );

    TEST_ASSERT( mbedtls_chacha20_setkey( &ctx, key_str->x ) == 0 );

    TEST_ASSERT( mbedtls_chacha20_starts( &ctx, nonce_str->x, counter ) == 0 );

    memset( output, 0x00, sizeof( output ) );
    TEST_ASSERT( mbedtls_chacha20_update( &ctx, src_str->len, src_str->x, output ) == 0 );

    ASSERT_COMPARE( output, expected_output_str->len,
                    expected_output_str->x, expected_output_str->len );

    /*
     * Test the streaming API again, piecewise
     */

    /* Don't free/init the context nor set the key again,
     * in order to test that starts() does the right thing. */
    TEST_ASSERT( mbedtls_chacha20_starts( &ctx, nonce_str->x, counter ) == 0 );

    memset( output, 0x00, sizeof( output ) );
    TEST_ASSERT( mbedtls_chacha20_update( &ctx, 1, src_str->x, output ) == 0 );
    TEST_ASSERT( mbedtls_chacha20_update( &ctx, src_str->len - 1,
                                          src_str->x + 1, output + 1 ) == 0 );

    ASSERT_COMPARE( output, expected_output_str->len,
                    expected_output_str->x, expected_output_str->len );

    mbedtls_chacha20_free( &ctx );
exit:
    ;
}

void test_chacha20_crypt_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};

    test_chacha20_crypt( &data0, &data2, *( (int *) params[4] ), &data5, &data7 );
}
#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_chacha20_bad_params()
{
    unsigned char key[32];
    unsigned char nonce[12];
    unsigned char src[1];
    unsigned char dst[1];
    uint32_t counter = 0;
    size_t len = sizeof( src );
    mbedtls_chacha20_context ctx;

    TEST_INVALID_PARAM( mbedtls_chacha20_init( NULL ) );
    TEST_VALID_PARAM( mbedtls_chacha20_free( NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA,
                            mbedtls_chacha20_setkey( NULL, key ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA,
                            mbedtls_chacha20_setkey( &ctx, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA,
                            mbedtls_chacha20_starts( NULL, nonce, counter ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA,
                            mbedtls_chacha20_starts( &ctx, NULL, counter ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA,
                            mbedtls_chacha20_update( NULL, 0, src, dst ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA,
                            mbedtls_chacha20_update( &ctx, len, NULL, dst ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA,
                            mbedtls_chacha20_update( &ctx, len, src, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA,
                            mbedtls_chacha20_crypt( NULL, nonce, counter, 0, src, dst ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA,
                            mbedtls_chacha20_crypt( key, NULL, counter, 0, src, dst ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA,
                            mbedtls_chacha20_crypt( key, nonce, counter, len, NULL, dst ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_CHACHA20_BAD_INPUT_DATA,
                            mbedtls_chacha20_crypt( key, nonce, counter, len, src, NULL ) );

exit:
    return;

}

void test_chacha20_bad_params_wrapper( void ** params )
{
    (void)params;

    test_chacha20_bad_params(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
#if defined(MBEDTLS_SELF_TEST)
void test_chacha20_self_test()
{
    TEST_ASSERT( mbedtls_chacha20_self_test( 1 ) == 0 );
exit:
    ;
}

void test_chacha20_self_test_wrapper( void ** params )
{
    (void)params;

    test_chacha20_self_test(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_CHACHA20_C */

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

#if defined(MBEDTLS_CHACHA20_C)

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

#if defined(MBEDTLS_CHACHA20_C)

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

#if defined(MBEDTLS_CHACHA20_C)
    test_chacha20_crypt_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_CHACHA20_C) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_chacha20_bad_params_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_CHACHA20_C) && defined(MBEDTLS_SELF_TEST)
    test_chacha20_self_test_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_chacha20.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
