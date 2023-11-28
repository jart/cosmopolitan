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
 * Test file      : ./test_suite_gcm.misc.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_gcm.function
 *      Test suite data     : suites/test_suite_gcm.misc.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_GCM_C)
#include "third_party/mbedtls/gcm.h"
void test_gcm_bad_parameters( int cipher_id, int direction,
                         data_t *key_str, data_t *src_str,
                         data_t *iv_str, data_t *add_str,
                         int tag_len_bits, int gcm_result )
{
    unsigned char output[128];
    unsigned char tag_output[16];
    mbedtls_gcm_context ctx;
    size_t tag_len = tag_len_bits / 8;

    mbedtls_gcm_init( &ctx );

    memset( output, 0x00, sizeof( output ) );
    memset( tag_output, 0x00, sizeof( tag_output ) );

    TEST_ASSERT( mbedtls_gcm_setkey( &ctx, cipher_id, key_str->x, key_str->len * 8 ) == 0 );
    TEST_ASSERT( mbedtls_gcm_crypt_and_tag( &ctx, direction, src_str->len, iv_str->x, iv_str->len,
                 add_str->x, add_str->len, src_str->x, output, tag_len, tag_output ) == gcm_result );

exit:
    mbedtls_gcm_free( &ctx );
}

void test_gcm_bad_parameters_wrapper( void ** params )
{
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_gcm_bad_parameters( *( (int *) params[0] ), *( (int *) params[1] ), &data2, &data4, &data6, &data8, *( (int *) params[10] ), *( (int *) params[11] ) );
}
void test_gcm_encrypt_and_tag( int cipher_id, data_t * key_str,
                          data_t * src_str, data_t * iv_str,
                          data_t * add_str, data_t * dst,
                          int tag_len_bits, data_t * tag,
                          int init_result )
{
    unsigned char output[128];
    unsigned char tag_output[16];
    mbedtls_gcm_context ctx;
    size_t tag_len = tag_len_bits / 8;

    mbedtls_gcm_init( &ctx );

    memset(output, 0x00, 128);
    memset(tag_output, 0x00, 16);


    TEST_ASSERT( mbedtls_gcm_setkey( &ctx, cipher_id, key_str->x, key_str->len * 8 ) == init_result );
    if( init_result == 0 )
    {
        TEST_ASSERT( mbedtls_gcm_crypt_and_tag( &ctx, MBEDTLS_GCM_ENCRYPT, src_str->len, iv_str->x, iv_str->len, add_str->x, add_str->len, src_str->x, output, tag_len, tag_output ) == 0 );

        TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x,
                                          src_str->len, dst->len ) == 0 );
        TEST_ASSERT( mbedtls_test_hexcmp( tag_output, tag->x,
                                          tag_len, tag->len ) == 0 );
    }

exit:
    mbedtls_gcm_free( &ctx );
}

void test_gcm_encrypt_and_tag_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};
    data_t data12 = {(uint8_t *) params[12], *( (uint32_t *) params[13] )};

    test_gcm_encrypt_and_tag( *( (int *) params[0] ), &data1, &data3, &data5, &data7, &data9, *( (int *) params[11] ), &data12, *( (int *) params[14] ) );
}
void test_gcm_decrypt_and_verify( int cipher_id, data_t * key_str,
                             data_t * src_str, data_t * iv_str,
                             data_t * add_str, int tag_len_bits,
                             data_t * tag_str, char * result,
                             data_t * pt_result, int init_result )
{
    unsigned char output[128];
    mbedtls_gcm_context ctx;
    int ret;
    size_t tag_len = tag_len_bits / 8;

    mbedtls_gcm_init( &ctx );

    memset(output, 0x00, 128);


    TEST_ASSERT( mbedtls_gcm_setkey( &ctx, cipher_id, key_str->x, key_str->len * 8 ) == init_result );
    if( init_result == 0 )
    {
        ret = mbedtls_gcm_auth_decrypt( &ctx, src_str->len, iv_str->x, iv_str->len, add_str->x, add_str->len, tag_str->x, tag_len, src_str->x, output );

        if( strcmp( "FAIL", result ) == 0 )
        {
            TEST_ASSERT( ret == MBEDTLS_ERR_GCM_AUTH_FAILED );
        }
        else
        {
            TEST_ASSERT( ret == 0 );

            TEST_ASSERT( mbedtls_test_hexcmp( output, pt_result->x,
                                              src_str->len,
                                              pt_result->len ) == 0 );
        }
    }

exit:
    mbedtls_gcm_free( &ctx );
}

void test_gcm_decrypt_and_verify_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};
    data_t data13 = {(uint8_t *) params[13], *( (uint32_t *) params[14] )};

    test_gcm_decrypt_and_verify( *( (int *) params[0] ), &data1, &data3, &data5, &data7, *( (int *) params[9] ), &data10, (char *) params[12], &data13, *( (int *) params[15] ) );
}
#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_gcm_invalid_param( )
{
    mbedtls_gcm_context ctx;
    unsigned char valid_buffer[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
    mbedtls_cipher_id_t valid_cipher = MBEDTLS_CIPHER_ID_AES;
    int valid_mode = MBEDTLS_GCM_ENCRYPT;
    int valid_len = sizeof(valid_buffer);
    int valid_bitlen = 128, invalid_bitlen = 1;

    mbedtls_gcm_init( &ctx );

    /* mbedtls_gcm_init() */
    TEST_INVALID_PARAM( mbedtls_gcm_init( NULL ) );

    /* mbedtls_gcm_setkey */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_setkey( NULL, valid_cipher, valid_buffer, valid_bitlen ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_setkey( &ctx, valid_cipher, NULL, valid_bitlen ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_setkey( &ctx, valid_cipher, valid_buffer, invalid_bitlen ) );

    /* mbedtls_gcm_crypt_and_tag() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_crypt_and_tag( NULL, valid_mode, valid_len,
                                   valid_buffer, valid_len,
                                   valid_buffer, valid_len,
                                   valid_buffer, valid_buffer,
                                   valid_len, valid_buffer ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_crypt_and_tag( &ctx, valid_mode, valid_len,
                                   NULL, valid_len,
                                   valid_buffer, valid_len,
                                   valid_buffer, valid_buffer,
                                   valid_len, valid_buffer ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_crypt_and_tag( &ctx, valid_mode, valid_len,
                                   valid_buffer, valid_len,
                                   NULL, valid_len,
                                   valid_buffer, valid_buffer,
                                   valid_len, valid_buffer ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_crypt_and_tag( &ctx, valid_mode, valid_len,
                                   valid_buffer, valid_len,
                                   valid_buffer, valid_len,
                                   NULL, valid_buffer,
                                   valid_len, valid_buffer ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_crypt_and_tag( &ctx, valid_mode, valid_len,
                                   valid_buffer, valid_len,
                                   valid_buffer, valid_len,
                                   valid_buffer, NULL,
                                   valid_len, valid_buffer ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_crypt_and_tag( &ctx, valid_mode, valid_len,
                                   valid_buffer, valid_len,
                                   valid_buffer, valid_len,
                                   valid_buffer, valid_buffer,
                                   valid_len, NULL ) );

    /* mbedtls_gcm_auth_decrypt() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_auth_decrypt( NULL, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_buffer) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_auth_decrypt( &ctx, valid_len,
                                  NULL, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_buffer) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_auth_decrypt( &ctx, valid_len,
                                  valid_buffer, valid_len,
                                  NULL, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_buffer) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_auth_decrypt( &ctx, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_len,
                                  NULL, valid_len,
                                  valid_buffer, valid_buffer) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_auth_decrypt( &ctx, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_len,
                                  NULL, valid_buffer) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_auth_decrypt( &ctx, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, valid_len,
                                  valid_buffer, NULL) );

    /* mbedtls_gcm_starts() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_starts( NULL, valid_mode,
                            valid_buffer, valid_len,
                            valid_buffer, valid_len ) );

    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_starts( &ctx, valid_mode,
                            NULL, valid_len,
                            valid_buffer, valid_len ) );

    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_starts( &ctx, valid_mode,
                            valid_buffer, valid_len,
                            NULL, valid_len ) );

    /* mbedtls_gcm_update() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_update( NULL, valid_len,
                            valid_buffer, valid_buffer ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_update( &ctx, valid_len,
                            NULL, valid_buffer ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_update( &ctx, valid_len,
                            valid_buffer, NULL ) );

    /* mbedtls_gcm_finish() */
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_finish( NULL, valid_buffer, valid_len ) );
    TEST_INVALID_PARAM_RET(
        MBEDTLS_ERR_GCM_BAD_INPUT,
        mbedtls_gcm_finish( &ctx, NULL, valid_len ) );

exit:
    mbedtls_gcm_free( &ctx );
}

void test_gcm_invalid_param_wrapper( void ** params )
{
    (void)params;

    test_gcm_invalid_param(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
void test_gcm_valid_param( )
{
    TEST_VALID_PARAM( mbedtls_gcm_free( NULL ) );
exit:
    return;
}

void test_gcm_valid_param_wrapper( void ** params )
{
    (void)params;

    test_gcm_valid_param(  );
}
#if defined(MBEDTLS_SELF_TEST)
void test_gcm_selftest(  )
{
    TEST_ASSERT( mbedtls_gcm_self_test( 1 ) == 0 );
exit:
    ;
}

void test_gcm_selftest_wrapper( void ** params )
{
    (void)params;

    test_gcm_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_GCM_C */

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

#if defined(MBEDTLS_GCM_C)

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

#if defined(MBEDTLS_GCM_C)

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

#if defined(MBEDTLS_GCM_C)
    test_gcm_bad_parameters_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_GCM_C)
    test_gcm_encrypt_and_tag_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_GCM_C)
    test_gcm_decrypt_and_verify_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_GCM_C) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_gcm_invalid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_GCM_C)
    test_gcm_valid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_GCM_C) && defined(MBEDTLS_SELF_TEST)
    test_gcm_selftest_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_gcm.misc.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
