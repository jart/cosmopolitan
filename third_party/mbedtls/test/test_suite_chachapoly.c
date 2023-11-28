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
 * Test file      : ./test_suite_chachapoly.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_chachapoly.function
 *      Test suite data     : suites/test_suite_chachapoly.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_CHACHAPOLY_C)
#include "third_party/mbedtls/chachapoly.h"
void test_mbedtls_chachapoly_enc( data_t *key_str, data_t *nonce_str, data_t *aad_str, data_t *input_str, data_t *output_str, data_t *mac_str )
{
    unsigned char output[265];
    unsigned char mac[16]; /* size set by the standard */
    mbedtls_chachapoly_context ctx;

    TEST_ASSERT( key_str->len   == 32 );
    TEST_ASSERT( nonce_str->len == 12 );
    TEST_ASSERT( mac_str->len   == 16 );

    mbedtls_chachapoly_init( &ctx );

    TEST_ASSERT( mbedtls_chachapoly_setkey( &ctx, key_str->x ) == 0 );

    TEST_ASSERT( mbedtls_chachapoly_encrypt_and_tag( &ctx,
                                      input_str->len, nonce_str->x,
                                      aad_str->x, aad_str->len,
                                      input_str->x, output, mac ) == 0 );

    TEST_ASSERT( memcmp( output_str->x, output, output_str->len ) == 0 );
    TEST_ASSERT( memcmp( mac_str->x, mac, 16U ) == 0 );

exit:
    mbedtls_chachapoly_free( &ctx );
}

void test_mbedtls_chachapoly_enc_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};

    test_mbedtls_chachapoly_enc( &data0, &data2, &data4, &data6, &data8, &data10 );
}
void test_mbedtls_chachapoly_dec( data_t *key_str, data_t *nonce_str, data_t *aad_str, data_t *input_str, data_t *output_str, data_t *mac_str, int ret_exp )
{
    unsigned char output[265];
    int ret;
    mbedtls_chachapoly_context ctx;

    TEST_ASSERT( key_str->len   == 32 );
    TEST_ASSERT( nonce_str->len == 12 );
    TEST_ASSERT( mac_str->len   == 16 );

    mbedtls_chachapoly_init( &ctx );

    TEST_ASSERT( mbedtls_chachapoly_setkey( &ctx, key_str->x ) == 0 );

    ret = mbedtls_chachapoly_auth_decrypt( &ctx,
                                           input_str->len, nonce_str->x,
                                           aad_str->x, aad_str->len,
                                           mac_str->x, input_str->x, output );

    TEST_ASSERT( ret == ret_exp );
    if( ret_exp == 0 )
    {
        TEST_ASSERT( memcmp( output_str->x, output, output_str->len ) == 0 );
    }

exit:
    mbedtls_chachapoly_free( &ctx );
}

void test_mbedtls_chachapoly_dec_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};

    test_mbedtls_chachapoly_dec( &data0, &data2, &data4, &data6, &data8, &data10, *( (int *) params[12] ) );
}
#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_chachapoly_bad_params()
{
    unsigned char key[32];
    unsigned char nonce[12];
    unsigned char aad[1];
    unsigned char input[1];
    unsigned char output[1];
    unsigned char mac[16];
    size_t input_len = sizeof( input );
    size_t aad_len = sizeof( aad );
    mbedtls_chachapoly_context ctx;

    memset( key,    0x00, sizeof( key ) );
    memset( nonce,  0x00, sizeof( nonce ) );
    memset( aad,    0x00, sizeof( aad ) );
    memset( input,  0x00, sizeof( input ) );
    memset( output, 0x00, sizeof( output ) );
    memset( mac,    0x00, sizeof( mac ) );

    TEST_INVALID_PARAM( mbedtls_chachapoly_init( NULL ) );
    TEST_VALID_PARAM( mbedtls_chachapoly_free( NULL ) );

    /* setkey */
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_setkey( NULL, key ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_setkey( &ctx, NULL ) );

    /* encrypt_and_tag */
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_encrypt_and_tag( NULL,
                                      0, nonce,
                                      aad, 0,
                                      input, output, mac ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_encrypt_and_tag( &ctx,
                                      0, NULL,
                                      aad, 0,
                                      input, output, mac ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_encrypt_and_tag( &ctx,
                                      0, nonce,
                                      NULL, aad_len,
                                      input, output, mac ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_encrypt_and_tag( &ctx,
                                      input_len, nonce,
                                      aad, 0,
                                      NULL, output, mac ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_encrypt_and_tag( &ctx,
                                      input_len, nonce,
                                      aad, 0,
                                      input, NULL, mac ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_encrypt_and_tag( &ctx,
                                      0, nonce,
                                      aad, 0,
                                      input, output, NULL ) );

    /* auth_decrypt */
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_auth_decrypt( NULL,
                                           0, nonce,
                                           aad, 0,
                                           mac, input, output ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_auth_decrypt( &ctx,
                                           0, NULL,
                                           aad, 0,
                                           mac, input, output ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_auth_decrypt( &ctx,
                                           0, nonce,
                                           NULL, aad_len,
                                           mac, input, output ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_auth_decrypt( &ctx,
                                           0, nonce,
                                           aad, 0,
                                           NULL, input, output ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_auth_decrypt( &ctx,
                                           input_len, nonce,
                                           aad, 0,
                                           mac, NULL, output ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_auth_decrypt( &ctx,
                                           input_len, nonce,
                                           aad, 0,
                                           mac, input, NULL ) );

    /* starts */
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_starts( NULL, nonce,
                                               MBEDTLS_CHACHAPOLY_ENCRYPT ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_starts( &ctx, NULL,
                                               MBEDTLS_CHACHAPOLY_ENCRYPT ) );

    /* update_aad */
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_update_aad( NULL, aad,
                                                           aad_len ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_update_aad( &ctx, NULL,
                                                           aad_len ) );

    /* update */
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_update( NULL, input_len,
                                                       input, output ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_update( &ctx, input_len,
                                                       NULL, output ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_update( &ctx, input_len,
                                                       input, NULL ) );

    /* finish */
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_finish( NULL, mac ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_POLY1305_BAD_INPUT_DATA,
                            mbedtls_chachapoly_finish( &ctx, NULL ) );

exit:
    return;
}

void test_chachapoly_bad_params_wrapper( void ** params )
{
    (void)params;

    test_chachapoly_bad_params(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
void test_chachapoly_state()
{
    unsigned char key[32];
    unsigned char nonce[12];
    unsigned char aad[1];
    unsigned char input[1];
    unsigned char output[1];
    unsigned char mac[16];
    size_t input_len = sizeof( input );
    size_t aad_len = sizeof( aad );
    mbedtls_chachapoly_context ctx;

    memset( key,    0x00, sizeof( key ) );
    memset( nonce,  0x00, sizeof( nonce ) );
    memset( aad,    0x00, sizeof( aad ) );
    memset( input,  0x00, sizeof( input ) );
    memset( output, 0x00, sizeof( output ) );
    memset( mac,    0x00, sizeof( mac ) );

    /* Initial state: finish, update, update_aad forbidden */
    mbedtls_chachapoly_init( &ctx );

    TEST_ASSERT( mbedtls_chachapoly_finish( &ctx, mac )
                 == MBEDTLS_ERR_CHACHAPOLY_BAD_STATE );
    TEST_ASSERT( mbedtls_chachapoly_update( &ctx, input_len, input, output )
                 == MBEDTLS_ERR_CHACHAPOLY_BAD_STATE );
    TEST_ASSERT( mbedtls_chachapoly_update_aad( &ctx, aad, aad_len )
                 == MBEDTLS_ERR_CHACHAPOLY_BAD_STATE );

    /* Still initial state: finish, update, update_aad forbidden */
    TEST_ASSERT( mbedtls_chachapoly_setkey( &ctx, key )
                 == 0 );

    TEST_ASSERT( mbedtls_chachapoly_finish( &ctx, mac )
                 == MBEDTLS_ERR_CHACHAPOLY_BAD_STATE );
    TEST_ASSERT( mbedtls_chachapoly_update( &ctx, input_len, input, output )
                 == MBEDTLS_ERR_CHACHAPOLY_BAD_STATE );
    TEST_ASSERT( mbedtls_chachapoly_update_aad( &ctx, aad, aad_len )
                 == MBEDTLS_ERR_CHACHAPOLY_BAD_STATE );

    /* Starts -> finish OK */
    TEST_ASSERT( mbedtls_chachapoly_starts( &ctx, nonce, MBEDTLS_CHACHAPOLY_ENCRYPT )
                 == 0 );
    TEST_ASSERT( mbedtls_chachapoly_finish( &ctx, mac )
                 == 0 );

    /* After finish: update, update_aad forbidden */
    TEST_ASSERT( mbedtls_chachapoly_update( &ctx, input_len, input, output )
                 == MBEDTLS_ERR_CHACHAPOLY_BAD_STATE );
    TEST_ASSERT( mbedtls_chachapoly_update_aad( &ctx, aad, aad_len )
                 == MBEDTLS_ERR_CHACHAPOLY_BAD_STATE );

    /* Starts -> update* OK */
    TEST_ASSERT( mbedtls_chachapoly_starts( &ctx, nonce, MBEDTLS_CHACHAPOLY_ENCRYPT )
                 == 0 );
    TEST_ASSERT( mbedtls_chachapoly_update( &ctx, input_len, input, output )
                 == 0 );
    TEST_ASSERT( mbedtls_chachapoly_update( &ctx, input_len, input, output )
                 == 0 );

    /* After update: update_aad forbidden */
    TEST_ASSERT( mbedtls_chachapoly_update_aad( &ctx, aad, aad_len )
                 == MBEDTLS_ERR_CHACHAPOLY_BAD_STATE );

    /* Starts -> update_aad* -> finish OK */
    TEST_ASSERT( mbedtls_chachapoly_starts( &ctx, nonce, MBEDTLS_CHACHAPOLY_ENCRYPT )
                 == 0 );
    TEST_ASSERT( mbedtls_chachapoly_update_aad( &ctx, aad, aad_len )
                 == 0 );
    TEST_ASSERT( mbedtls_chachapoly_update_aad( &ctx, aad, aad_len )
                 == 0 );
    TEST_ASSERT( mbedtls_chachapoly_finish( &ctx, mac )
                 == 0 );

exit:
    mbedtls_chachapoly_free( &ctx );
}

void test_chachapoly_state_wrapper( void ** params )
{
    (void)params;

    test_chachapoly_state(  );
}
#if defined(MBEDTLS_SELF_TEST)
void test_chachapoly_selftest()
{
    TEST_ASSERT( mbedtls_chachapoly_self_test( 1 ) == 0 );
exit:
    ;
}

void test_chachapoly_selftest_wrapper( void ** params )
{
    (void)params;

    test_chachapoly_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_CHACHAPOLY_C */

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

#if defined(MBEDTLS_CHACHAPOLY_C)

        case 0:
            {
                *out_value = MBEDTLS_ERR_CHACHAPOLY_AUTH_FAILED;
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

#if defined(MBEDTLS_CHACHAPOLY_C)

        case 0:
            {
#if defined(MBEDTLS_SELF_TEST)
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

#if defined(MBEDTLS_CHACHAPOLY_C)
    test_mbedtls_chachapoly_enc_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_CHACHAPOLY_C)
    test_mbedtls_chachapoly_dec_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_CHACHAPOLY_C) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_chachapoly_bad_params_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_CHACHAPOLY_C)
    test_chachapoly_state_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_CHACHAPOLY_C) && defined(MBEDTLS_SELF_TEST)
    test_chachapoly_selftest_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_chachapoly.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
