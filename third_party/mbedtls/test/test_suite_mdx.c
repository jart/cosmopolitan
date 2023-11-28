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
 * Test file      : ./test_suite_mdx.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_mdx.function
 *      Test suite data     : suites/test_suite_mdx.data
 *
 */


#define TEST_SUITE_ACTIVE

#include "third_party/mbedtls/md5.h"
#if defined(MBEDTLS_MD2_C)
void test_md2_text( char * text_src_string, data_t * hash )
{
    int ret;
    unsigned char src_str[100];
    unsigned char output[16];

    memset( src_str, 0x00, sizeof src_str );
    memset( output, 0x00, sizeof output );

    strncpy( (char *) src_str, text_src_string, sizeof(src_str) - 1 );

    ret = mbedtls_md2_ret( src_str, strlen( (char *) src_str ), output );
    TEST_ASSERT( ret == 0 ) ;

    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x,
                                      sizeof  output, hash->len ) == 0 );
exit:
    ;
}

void test_md2_text_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_md2_text( (char *) params[0], &data1 );
}
#endif /* MBEDTLS_MD2_C */
#if defined(MBEDTLS_MD4_C)
void test_md4_text( char * text_src_string, data_t * hash )
{
    int ret;
    unsigned char src_str[100];
    unsigned char output[16];

    memset( src_str, 0x00, sizeof src_str );
    memset( output, 0x00, sizeof output );

    strncpy( (char *) src_str, text_src_string, sizeof(src_str) - 1 );

    ret = mbedtls_md4_ret( src_str, strlen( (char *) src_str ), output );
    TEST_ASSERT( ret == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x,
                                      sizeof  output, hash->len ) == 0 );
exit:
    ;
}

void test_md4_text_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_md4_text( (char *) params[0], &data1 );
}
#endif /* MBEDTLS_MD4_C */
#if defined(MBEDTLS_MD5_C)
void test_md5_text( char * text_src_string, data_t * hash )
{
    int ret;
    unsigned char src_str[100];
    unsigned char output[16];

    memset( src_str, 0x00, sizeof src_str );
    memset( output, 0x00, sizeof output );

    strncpy( (char *) src_str, text_src_string, sizeof(src_str) - 1 );

    ret = mbedtls_md5_ret( src_str, strlen( (char *) src_str ), output );
    TEST_ASSERT( ret == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x,
                                      sizeof  output, hash->len ) == 0 );
exit:
    ;
}

void test_md5_text_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_md5_text( (char *) params[0], &data1 );
}
#endif /* MBEDTLS_MD5_C */
#if defined(MBEDTLS_RIPEMD160_C)
void test_ripemd160_text( char * text_src_string, data_t * hash )
{
    int ret;
    unsigned char src_str[100];
    unsigned char output[20];

    memset(src_str, 0x00, sizeof src_str);
    memset(output, 0x00, sizeof output);

    strncpy( (char *) src_str, text_src_string, sizeof(src_str) - 1 );

    ret = mbedtls_ripemd160_ret( src_str, strlen( (char *) src_str ), output );
    TEST_ASSERT( ret == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x,
                                      sizeof output, hash->len ) == 0 );
exit:
    ;
}

void test_ripemd160_text_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_ripemd160_text( (char *) params[0], &data1 );
}
#endif /* MBEDTLS_RIPEMD160_C */
#if defined(MBEDTLS_MD2_C)
#if defined(MBEDTLS_SELF_TEST)
void test_md2_selftest(  )
{
    TEST_ASSERT( mbedtls_md2_self_test( 1 ) == 0 );
exit:
    ;
}

void test_md2_selftest_wrapper( void ** params )
{
    (void)params;

    test_md2_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_MD2_C */
#if defined(MBEDTLS_MD4_C)
#if defined(MBEDTLS_SELF_TEST)
void test_md4_selftest(  )
{
    TEST_ASSERT( mbedtls_md4_self_test( 1 ) == 0 );
exit:
    ;
}

void test_md4_selftest_wrapper( void ** params )
{
    (void)params;

    test_md4_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_MD4_C */
#if defined(MBEDTLS_MD5_C)
#if defined(MBEDTLS_SELF_TEST)
void test_md5_selftest(  )
{
    TEST_ASSERT( mbedtls_md5_self_test( 1 ) == 0 );
exit:
    ;
}

void test_md5_selftest_wrapper( void ** params )
{
    (void)params;

    test_md5_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_MD5_C */
#if defined(MBEDTLS_RIPEMD160_C)
#if defined(MBEDTLS_SELF_TEST)
void test_ripemd160_selftest(  )
{
    TEST_ASSERT( mbedtls_ripemd160_self_test( 1 ) == 0 );
exit:
    ;
}

void test_ripemd160_selftest_wrapper( void ** params )
{
    (void)params;

    test_ripemd160_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_RIPEMD160_C */

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

#if defined(MBEDTLS_MD2_C)
    test_md2_text_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_MD4_C)
    test_md4_text_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_MD5_C)
    test_md5_text_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_RIPEMD160_C)
    test_ripemd160_text_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_MD2_C) && defined(MBEDTLS_SELF_TEST)
    test_md2_selftest_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_MD4_C) && defined(MBEDTLS_SELF_TEST)
    test_md4_selftest_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_MD5_C) && defined(MBEDTLS_SELF_TEST)
    test_md5_selftest_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_RIPEMD160_C) && defined(MBEDTLS_SELF_TEST)
    test_ripemd160_selftest_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_mdx.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
