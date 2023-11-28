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
 * Test file      : ./test_suite_des.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_des.function
 *      Test suite data     : suites/test_suite_des.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_DES_C)
#include "third_party/mbedtls/des.h"
void test_des_check_weak( data_t * key, int ret )
{
    TEST_ASSERT( mbedtls_des_key_check_weak( key->x ) == ret );
exit:
    ;
}

void test_des_check_weak_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_des_check_weak( &data0, *( (int *) params[2] ) );
}
void test_des_encrypt_ecb( data_t * key_str, data_t * src_str, data_t * dst )
{
    unsigned char output[100];
    mbedtls_des_context ctx;

    memset(output, 0x00, 100);
    mbedtls_des_init( &ctx );


    mbedtls_des_setkey_enc( &ctx, key_str->x );
    TEST_ASSERT( mbedtls_des_crypt_ecb( &ctx, src_str->x, output ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x, 8, dst->len ) == 0 );

exit:
    mbedtls_des_free( &ctx );
}

void test_des_encrypt_ecb_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};

    test_des_encrypt_ecb( &data0, &data2, &data4 );
}
void test_des_decrypt_ecb( data_t * key_str, data_t * src_str, data_t * dst )
{
    unsigned char output[100];
    mbedtls_des_context ctx;

    memset(output, 0x00, 100);
    mbedtls_des_init( &ctx );


    mbedtls_des_setkey_dec( &ctx, key_str->x );
    TEST_ASSERT( mbedtls_des_crypt_ecb( &ctx, src_str->x, output ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x, 8, dst->len ) == 0 );

exit:
    mbedtls_des_free( &ctx );
}

void test_des_decrypt_ecb_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};

    test_des_decrypt_ecb( &data0, &data2, &data4 );
}
#if defined(MBEDTLS_CIPHER_MODE_CBC)
void test_des_encrypt_cbc( data_t * key_str, data_t * iv_str,
                      data_t * src_str, data_t * dst, int cbc_result )
{
    unsigned char output[100];
    mbedtls_des_context ctx;

    memset(output, 0x00, 100);
    mbedtls_des_init( &ctx );


    mbedtls_des_setkey_enc( &ctx, key_str->x );
    TEST_ASSERT( mbedtls_des_crypt_cbc( &ctx, MBEDTLS_DES_ENCRYPT, src_str->len, iv_str->x, src_str->x, output ) == cbc_result );
    if( cbc_result == 0 )
    {

        TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x, src_str->len,
                                          dst->len ) == 0 );
    }

exit:
    mbedtls_des_free( &ctx );
}

void test_des_encrypt_cbc_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_des_encrypt_cbc( &data0, &data2, &data4, &data6, *( (int *) params[8] ) );
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */
#if defined(MBEDTLS_CIPHER_MODE_CBC)
void test_des_decrypt_cbc( data_t * key_str, data_t * iv_str,
                      data_t * src_str, data_t * dst,
                      int cbc_result )
{
    unsigned char output[100];
    mbedtls_des_context ctx;

    memset(output, 0x00, 100);
    mbedtls_des_init( &ctx );


    mbedtls_des_setkey_dec( &ctx, key_str->x );
    TEST_ASSERT( mbedtls_des_crypt_cbc( &ctx, MBEDTLS_DES_DECRYPT, src_str->len, iv_str->x, src_str->x, output ) == cbc_result );
    if( cbc_result == 0 )
    {

        TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x, src_str->len,
                                          dst->len ) == 0 );
    }

exit:
    mbedtls_des_free( &ctx );
}

void test_des_decrypt_cbc_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};

    test_des_decrypt_cbc( &data0, &data2, &data4, &data6, *( (int *) params[8] ) );
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */
void test_des3_encrypt_ecb( int key_count, data_t * key_str,
                       data_t * src_str, data_t * dst )
{
    unsigned char output[100];
    mbedtls_des3_context ctx;

    memset(output, 0x00, 100);
    mbedtls_des3_init( &ctx );


    if( key_count == 2 )
        mbedtls_des3_set2key_enc( &ctx, key_str->x );
    else if( key_count == 3 )
        mbedtls_des3_set3key_enc( &ctx, key_str->x );
    else
        TEST_ASSERT( 0 );

    TEST_ASSERT( mbedtls_des3_crypt_ecb( &ctx, src_str->x, output ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x, 8, dst->len ) == 0 );

exit:
    mbedtls_des3_free( &ctx );
}

void test_des3_encrypt_ecb_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};

    test_des3_encrypt_ecb( *( (int *) params[0] ), &data1, &data3, &data5 );
}
void test_des3_decrypt_ecb( int key_count, data_t * key_str,
                       data_t * src_str, data_t * dst )
{
    unsigned char output[100];
    mbedtls_des3_context ctx;

    memset(output, 0x00, 100);
    mbedtls_des3_init( &ctx );


    if( key_count == 2 )
        mbedtls_des3_set2key_dec( &ctx, key_str->x );
    else if( key_count == 3 )
        mbedtls_des3_set3key_dec( &ctx, key_str->x );
    else
        TEST_ASSERT( 0 );

    TEST_ASSERT( mbedtls_des3_crypt_ecb( &ctx, src_str->x, output ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x, 8, dst->len ) == 0 );

exit:
    mbedtls_des3_free( &ctx );
}

void test_des3_decrypt_ecb_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};

    test_des3_decrypt_ecb( *( (int *) params[0] ), &data1, &data3, &data5 );
}
#if defined(MBEDTLS_CIPHER_MODE_CBC)
void test_des3_encrypt_cbc( int key_count, data_t * key_str,
                       data_t * iv_str, data_t * src_str,
                       data_t * dst, int cbc_result )
{
    unsigned char output[100];
    mbedtls_des3_context ctx;

    memset(output, 0x00, 100);
    mbedtls_des3_init( &ctx );


    if( key_count == 2 )
        mbedtls_des3_set2key_enc( &ctx, key_str->x );
    else if( key_count == 3 )
        mbedtls_des3_set3key_enc( &ctx, key_str->x );
    else
        TEST_ASSERT( 0 );

    TEST_ASSERT( mbedtls_des3_crypt_cbc( &ctx, MBEDTLS_DES_ENCRYPT, src_str->len, iv_str->x, src_str->x, output ) == cbc_result );

    if( cbc_result == 0 )
    {

        TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x,
                                          src_str->len, dst->len ) == 0 );
    }

exit:
    mbedtls_des3_free( &ctx );
}

void test_des3_encrypt_cbc_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};

    test_des3_encrypt_cbc( *( (int *) params[0] ), &data1, &data3, &data5, &data7, *( (int *) params[9] ) );
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */
#if defined(MBEDTLS_CIPHER_MODE_CBC)
void test_des3_decrypt_cbc( int key_count, data_t * key_str,
                       data_t * iv_str, data_t * src_str,
                       data_t * dst, int cbc_result )
{
    unsigned char output[100];
    mbedtls_des3_context ctx;

    memset(output, 0x00, 100);
    mbedtls_des3_init( &ctx );


    if( key_count == 2 )
        mbedtls_des3_set2key_dec( &ctx, key_str->x );
    else if( key_count == 3 )
        mbedtls_des3_set3key_dec( &ctx, key_str->x );
    else
        TEST_ASSERT( 0 );

    TEST_ASSERT( mbedtls_des3_crypt_cbc( &ctx, MBEDTLS_DES_DECRYPT, src_str->len, iv_str->x, src_str->x, output ) == cbc_result );

    if( cbc_result == 0 )
    {

        TEST_ASSERT( mbedtls_test_hexcmp( output, dst->x, src_str->len,
                                          dst->len ) == 0 );
    }

exit:
    mbedtls_des3_free( &ctx );
}

void test_des3_decrypt_cbc_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};

    test_des3_decrypt_cbc( *( (int *) params[0] ), &data1, &data3, &data5, &data7, *( (int *) params[9] ) );
}
#endif /* MBEDTLS_CIPHER_MODE_CBC */
void test_des_key_parity_run(  )
{
    int i, j, cnt;
    unsigned char key[MBEDTLS_DES_KEY_SIZE];
    unsigned int parity;

    memset( key, 0, MBEDTLS_DES_KEY_SIZE );
    cnt = 0;

    // Iterate through all possible byte values
    //
    for( i = 0; i < 32; i++ )
    {
        for( j = 0; j < 8; j++ )
            key[j] = cnt++;

        // Set the key parity according to the table
        //
        mbedtls_des_key_set_parity( key );

        // Check the parity with a function
        //
        for( j = 0; j < 8; j++ )
        {
            parity = key[j] ^ ( key[j] >> 4 );
            parity = parity ^
                    ( parity >> 1 ) ^
                    ( parity >> 2 ) ^
                    ( parity >> 3 );
            parity &= 1;

            if( parity != 1 )
                TEST_ASSERT( 0 );
        }

        // Check the parity with the table
        //
        TEST_ASSERT( mbedtls_des_key_check_key_parity( key ) == 0 );
    }
exit:
    ;
}

void test_des_key_parity_run_wrapper( void ** params )
{
    (void)params;

    test_des_key_parity_run(  );
}
#if defined(MBEDTLS_SELF_TEST)
void test_des_selftest(  )
{
    TEST_ASSERT( mbedtls_des_self_test( 1 ) == 0 );
exit:
    ;
}

void test_des_selftest_wrapper( void ** params )
{
    (void)params;

    test_des_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_DES_C */

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

#if defined(MBEDTLS_DES_C)

        case 0:
            {
                *out_value = MBEDTLS_ERR_DES_INVALID_INPUT_LENGTH;
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

#if defined(MBEDTLS_DES_C)

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

#if defined(MBEDTLS_DES_C)
    test_des_check_weak_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_DES_C)
    test_des_encrypt_ecb_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_DES_C)
    test_des_decrypt_ecb_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_DES_C) && defined(MBEDTLS_CIPHER_MODE_CBC)
    test_des_encrypt_cbc_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_DES_C) && defined(MBEDTLS_CIPHER_MODE_CBC)
    test_des_decrypt_cbc_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_DES_C)
    test_des3_encrypt_ecb_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_DES_C)
    test_des3_decrypt_ecb_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_DES_C) && defined(MBEDTLS_CIPHER_MODE_CBC)
    test_des3_encrypt_cbc_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_DES_C) && defined(MBEDTLS_CIPHER_MODE_CBC)
    test_des3_decrypt_cbc_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_DES_C)
    test_des_key_parity_run_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_DES_C) && defined(MBEDTLS_SELF_TEST)
    test_des_selftest_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_des.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
