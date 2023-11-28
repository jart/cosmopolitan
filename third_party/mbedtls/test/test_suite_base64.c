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
 * Test file      : ./test_suite_base64.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_base64.function
 *      Test suite data     : suites/test_suite_base64.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_BASE64_C)
#include "third_party/mbedtls/base64.h"
void test_mbedtls_base64_encode( char * src_string, char * dst_string,
                            int dst_buf_size, int result )
{
    unsigned char src_str[1000];
    unsigned char dst_str[1000];
    size_t len, src_len;

    memset(src_str, 0x00, 1000);
    memset(dst_str, 0x00, 1000);

    strncpy( (char *) src_str, src_string, sizeof(src_str) - 1 );
    src_len = strlen( (char *) src_str );

    TEST_CF_SECRET( src_str, sizeof( src_str ) );
    TEST_ASSERT( mbedtls_base64_encode( dst_str, dst_buf_size, &len, src_str, src_len) == result );
    TEST_CF_PUBLIC( src_str, sizeof( src_str ) );

    /* dest_str will have had tainted data copied to it, prevent the TEST_ASSERT below from triggering
       CF failures by unmarking it. */
    TEST_CF_PUBLIC( dst_str, len );

    if( result == 0 )
    {
        TEST_ASSERT( strcmp( (char *) dst_str, dst_string ) == 0 );
    }
exit:
    ;
}

void test_mbedtls_base64_encode_wrapper( void ** params )
{

    test_mbedtls_base64_encode( (char *) params[0], (char *) params[1], *( (int *) params[2] ), *( (int *) params[3] ) );
}
void test_mbedtls_base64_decode( char * src_string, char * dst_string, int result )
{
    unsigned char src_str[1000];
    unsigned char dst_str[1000];
    size_t len;
    int res;

    memset(src_str, 0x00, 1000);
    memset(dst_str, 0x00, 1000);

    strncpy( (char *) src_str, src_string, sizeof(src_str) - 1 );
    res = mbedtls_base64_decode( dst_str, sizeof( dst_str ), &len, src_str, strlen( (char *) src_str ) );
    TEST_ASSERT( res == result );
    if( result == 0 )
    {
        TEST_ASSERT( strcmp( (char *) dst_str, dst_string ) == 0 );
    }
exit:
    ;
}

void test_mbedtls_base64_decode_wrapper( void ** params )
{

    test_mbedtls_base64_decode( (char *) params[0], (char *) params[1], *( (int *) params[2] ) );
}
void test_base64_encode_hex( data_t * src, char * dst, int dst_buf_size,
                        int result )
{
    unsigned char *res = NULL;
    size_t len;

    res = mbedtls_test_zero_alloc( dst_buf_size );

    TEST_CF_SECRET( src->x, src->len );
    TEST_ASSERT( mbedtls_base64_encode( res, dst_buf_size, &len, src->x, src->len ) == result );
    TEST_CF_PUBLIC( src->x, src->len );

    /* res will have had tainted data copied to it, prevent the TEST_ASSERT below from triggering
       CF failures by unmarking it. */
    TEST_CF_PUBLIC( res, len );

    if( result == 0 )
    {
        TEST_ASSERT( len == strlen( dst ) );
        TEST_ASSERT( timingsafe_bcmp( dst, res, len ) == 0 );
    }

exit:
    mbedtls_free( res );
}

void test_base64_encode_hex_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_base64_encode_hex( &data0, (char *) params[2], *( (int *) params[3] ), *( (int *) params[4] ) );
}
void test_base64_decode_hex( char * src, data_t * dst, int dst_buf_size,
                        int result )
{
    unsigned char *res = NULL;
    size_t len;

    res = mbedtls_test_zero_alloc( dst_buf_size );

    TEST_ASSERT( mbedtls_base64_decode( res, dst_buf_size, &len, (unsigned char *) src,
                                strlen( src ) ) == result );
    if( result == 0 )
    {
        TEST_ASSERT( len == dst->len );
        TEST_ASSERT( timingsafe_bcmp( dst->x, res, len ) == 0 );
    }

exit:
    mbedtls_free( res );
}

void test_base64_decode_hex_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_base64_decode_hex( (char *) params[0], &data1, *( (int *) params[3] ), *( (int *) params[4] ) );
}
void test_base64_decode_hex_src( data_t * src, char * dst_ref, int result )
{
    unsigned char dst[1000] = { 0 };
    size_t len;

    TEST_ASSERT( mbedtls_base64_decode( dst, sizeof( dst ), &len, src->x, src->len ) == result );
    if( result == 0 )
    {
        TEST_ASSERT( len == strlen( dst_ref ) );
        TEST_ASSERT( timingsafe_bcmp( dst, dst_ref, len ) == 0 );
    }

exit:
    ;;
}

void test_base64_decode_hex_src_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_base64_decode_hex_src( &data0, (char *) params[2], *( (int *) params[3] ) );
}
#if defined(MBEDTLS_SELF_TEST)
void test_base64_selftest(  )
{
    TEST_ASSERT( mbedtls_base64_self_test( 1 ) == 0 );
exit:
    ;
}

void test_base64_selftest_wrapper( void ** params )
{
    (void)params;

    test_base64_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_BASE64_C */

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

#if defined(MBEDTLS_BASE64_C)

        case 0:
            {
                *out_value = MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_ERR_BASE64_INVALID_CHARACTER;
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

#if defined(MBEDTLS_BASE64_C)

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

#if defined(MBEDTLS_BASE64_C)
    test_mbedtls_base64_encode_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_BASE64_C)
    test_mbedtls_base64_decode_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_BASE64_C)
    test_base64_encode_hex_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_BASE64_C)
    test_base64_decode_hex_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_BASE64_C)
    test_base64_decode_hex_src_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_BASE64_C) && defined(MBEDTLS_SELF_TEST)
    test_base64_selftest_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_base64.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
