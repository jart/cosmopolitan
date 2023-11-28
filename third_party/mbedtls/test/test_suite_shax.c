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
 * Test file      : ./test_suite_shax.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_shax.function
 *      Test suite data     : suites/test_suite_shax.data
 *
 */


#define TEST_SUITE_ACTIVE

#include "third_party/mbedtls/sha1.h"
#include "third_party/mbedtls/sha256.h"
#include "third_party/mbedtls/sha512.h"
#if defined(MBEDTLS_SHA1_C)
void test_sha1_valid_param( )
{
    TEST_VALID_PARAM( mbedtls_sha1_free( NULL ) );
exit:
    ;
}

void test_sha1_valid_param_wrapper( void ** params )
{
    (void)params;

    test_sha1_valid_param(  );
}
#endif /* MBEDTLS_SHA1_C */
#if defined(MBEDTLS_SHA1_C)
#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_sha1_invalid_param( )
{
    mbedtls_sha1_context ctx;
    unsigned char buf[64] = { 0 };
    size_t const buflen = sizeof( buf );

    TEST_INVALID_PARAM( mbedtls_sha1_init( NULL ) );

    TEST_INVALID_PARAM( mbedtls_sha1_clone( NULL, &ctx ) );
    TEST_INVALID_PARAM( mbedtls_sha1_clone( &ctx, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA1_BAD_INPUT_DATA,
                            mbedtls_sha1_starts_ret( NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA1_BAD_INPUT_DATA,
                            mbedtls_sha1_update_ret( NULL, buf, buflen ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA1_BAD_INPUT_DATA,
                            mbedtls_sha1_update_ret( &ctx, NULL, buflen ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA1_BAD_INPUT_DATA,
                            mbedtls_sha1_finish_ret( NULL, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA1_BAD_INPUT_DATA,
                            mbedtls_sha1_finish_ret( &ctx, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA1_BAD_INPUT_DATA,
                            mbedtls_internal_sha1_process( NULL, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA1_BAD_INPUT_DATA,
                            mbedtls_internal_sha1_process( &ctx, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA1_BAD_INPUT_DATA,
                            mbedtls_sha1_ret( NULL, buflen, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA1_BAD_INPUT_DATA,
                            mbedtls_sha1_ret( buf, buflen, NULL ) );

exit:
    return;
}

void test_sha1_invalid_param_wrapper( void ** params )
{
    (void)params;

    test_sha1_invalid_param(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
#endif /* MBEDTLS_SHA1_C */
#if defined(MBEDTLS_SHA1_C)
void test_mbedtls_sha1( data_t * src_str, data_t * hash )
{
    unsigned char output[41];

    memset(output, 0x00, 41);


    TEST_ASSERT( mbedtls_sha1_ret( src_str->x, src_str->len, output ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x, 20, hash->len ) == 0 );
exit:
    ;
}

void test_mbedtls_sha1_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};

    test_mbedtls_sha1( &data0, &data2 );
}
#endif /* MBEDTLS_SHA1_C */
#if defined(MBEDTLS_SHA256_C)
void test_sha256_valid_param( )
{
    TEST_VALID_PARAM( mbedtls_sha256_free( NULL ) );
exit:
    ;
}

void test_sha256_valid_param_wrapper( void ** params )
{
    (void)params;

    test_sha256_valid_param(  );
}
#endif /* MBEDTLS_SHA256_C */
#if defined(MBEDTLS_SHA256_C)
#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_sha256_invalid_param( )
{
    mbedtls_sha256_context ctx;
    unsigned char buf[64] = { 0 };
    size_t const buflen = sizeof( buf );
    int valid_type = 0;
    int invalid_type = 42;

    TEST_INVALID_PARAM( mbedtls_sha256_init( NULL ) );

    TEST_INVALID_PARAM( mbedtls_sha256_clone( NULL, &ctx ) );
    TEST_INVALID_PARAM( mbedtls_sha256_clone( &ctx, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA256_BAD_INPUT_DATA,
                            mbedtls_sha256_starts_ret( NULL, valid_type ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA256_BAD_INPUT_DATA,
                            mbedtls_sha256_starts_ret( &ctx, invalid_type ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA256_BAD_INPUT_DATA,
                            mbedtls_sha256_update_ret( NULL, buf, buflen ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA256_BAD_INPUT_DATA,
                            mbedtls_sha256_update_ret( &ctx, NULL, buflen ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA256_BAD_INPUT_DATA,
                            mbedtls_sha256_finish_ret( NULL, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA256_BAD_INPUT_DATA,
                            mbedtls_sha256_finish_ret( &ctx, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA256_BAD_INPUT_DATA,
                            mbedtls_internal_sha256_process( NULL, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA256_BAD_INPUT_DATA,
                            mbedtls_internal_sha256_process( &ctx, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA256_BAD_INPUT_DATA,
                            mbedtls_sha256_ret( NULL, buflen,
                                                buf, valid_type ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA256_BAD_INPUT_DATA,
                            mbedtls_sha256_ret( buf, buflen,
                                                NULL, valid_type ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA256_BAD_INPUT_DATA,
                            mbedtls_sha256_ret( buf, buflen,
                                                buf, invalid_type ) );

exit:
    return;
}

void test_sha256_invalid_param_wrapper( void ** params )
{
    (void)params;

    test_sha256_invalid_param(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
#endif /* MBEDTLS_SHA256_C */
#if defined(MBEDTLS_SHA256_C)
void test_sha224( data_t * src_str, data_t * hash )
{
    unsigned char output[57];

    memset(output, 0x00, 57);


    TEST_ASSERT( mbedtls_sha256_ret( src_str->x, src_str->len, output, 1 ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x, 28, hash->len ) == 0 );
exit:
    ;
}

void test_sha224_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};

    test_sha224( &data0, &data2 );
}
#endif /* MBEDTLS_SHA256_C */
#if defined(MBEDTLS_SHA256_C)
void test_mbedtls_sha256( data_t * src_str, data_t * hash )
{
    unsigned char output[65];

    memset(output, 0x00, 65);


    TEST_ASSERT( mbedtls_sha256_ret( src_str->x, src_str->len, output, 0 ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x, 32, hash->len ) == 0 );
exit:
    ;
}

void test_mbedtls_sha256_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};

    test_mbedtls_sha256( &data0, &data2 );
}
#endif /* MBEDTLS_SHA256_C */
#if defined(MBEDTLS_SHA512_C)
void test_sha512_valid_param( )
{
    TEST_VALID_PARAM( mbedtls_sha512_free( NULL ) );
exit:
    ;
}

void test_sha512_valid_param_wrapper( void ** params )
{
    (void)params;

    test_sha512_valid_param(  );
}
#endif /* MBEDTLS_SHA512_C */
#if defined(MBEDTLS_SHA512_C)
#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_sha512_invalid_param( )
{
    mbedtls_sha512_context ctx;
    unsigned char buf[128] = { 0 };
    size_t const buflen = sizeof( buf );
    int valid_type = 0;
    int invalid_type = 42;

    TEST_INVALID_PARAM( mbedtls_sha512_init( NULL ) );

    TEST_INVALID_PARAM( mbedtls_sha512_clone( NULL, &ctx ) );
    TEST_INVALID_PARAM( mbedtls_sha512_clone( &ctx, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA512_BAD_INPUT_DATA,
                            mbedtls_sha512_starts_ret( NULL, valid_type ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA512_BAD_INPUT_DATA,
                            mbedtls_sha512_starts_ret( &ctx, invalid_type ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA512_BAD_INPUT_DATA,
                            mbedtls_sha512_update_ret( NULL, buf, buflen ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA512_BAD_INPUT_DATA,
                            mbedtls_sha512_update_ret( &ctx, NULL, buflen ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA512_BAD_INPUT_DATA,
                            mbedtls_sha512_finish_ret( NULL, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA512_BAD_INPUT_DATA,
                            mbedtls_sha512_finish_ret( &ctx, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA512_BAD_INPUT_DATA,
                            mbedtls_internal_sha512_process( NULL, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA512_BAD_INPUT_DATA,
                            mbedtls_internal_sha512_process( &ctx, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA512_BAD_INPUT_DATA,
                            mbedtls_sha512_ret( NULL, buflen,
                                                buf, valid_type ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA512_BAD_INPUT_DATA,
                            mbedtls_sha512_ret( buf, buflen,
                                                NULL, valid_type ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_SHA512_BAD_INPUT_DATA,
                            mbedtls_sha512_ret( buf, buflen,
                                                buf, invalid_type ) );

exit:
    return;
}

void test_sha512_invalid_param_wrapper( void ** params )
{
    (void)params;

    test_sha512_invalid_param(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
#endif /* MBEDTLS_SHA512_C */
#if defined(MBEDTLS_SHA512_C)
void test_sha384( data_t * src_str, data_t * hash )
{
    unsigned char output[97];

    memset(output, 0x00, 97);


    TEST_ASSERT( mbedtls_sha512_ret( src_str->x, src_str->len, output, 1 ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x, 48, hash->len ) == 0 );
exit:
    ;
}

void test_sha384_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};

    test_sha384( &data0, &data2 );
}
#endif /* MBEDTLS_SHA512_C */
#if defined(MBEDTLS_SHA512_C)
void test_mbedtls_sha512( data_t * src_str, data_t * hash )
{
    unsigned char output[129];

    memset(output, 0x00, 129);


    TEST_ASSERT( mbedtls_sha512_ret( src_str->x, src_str->len, output, 0 ) == 0 );

    TEST_ASSERT( mbedtls_test_hexcmp( output, hash->x, 64, hash->len ) == 0 );
exit:
    ;
}

void test_mbedtls_sha512_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};

    test_mbedtls_sha512( &data0, &data2 );
}
#endif /* MBEDTLS_SHA512_C */
#if defined(MBEDTLS_SHA1_C)
#if defined(MBEDTLS_SELF_TEST)
void test_sha1_selftest(  )
{
    TEST_ASSERT( mbedtls_sha1_self_test( 1 ) == 0 );
exit:
    ;
}

void test_sha1_selftest_wrapper( void ** params )
{
    (void)params;

    test_sha1_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_SHA1_C */
#if defined(MBEDTLS_SHA256_C)
#if defined(MBEDTLS_SELF_TEST)
void test_sha256_selftest(  )
{
    TEST_ASSERT( mbedtls_sha256_self_test( 1 ) == 0 );
exit:
    ;
}

void test_sha256_selftest_wrapper( void ** params )
{
    (void)params;

    test_sha256_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_SHA256_C */
#if defined(MBEDTLS_SHA512_C)
#if defined(MBEDTLS_SELF_TEST)
void test_sha512_selftest(  )
{
    TEST_ASSERT( mbedtls_sha512_self_test( 1 ) == 0 );
exit:
    ;
}

void test_sha512_selftest_wrapper( void ** params )
{
    (void)params;

    test_sha512_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_SHA512_C */

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
#if defined(MBEDTLS_SHA256_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 2:
            {
#if defined(MBEDTLS_SHA512_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 3:
            {
#if !defined(MBEDTLS_SHA512_NO_SHA384)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if defined(MBEDTLS_SELF_TEST)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
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

#if defined(MBEDTLS_SHA1_C)
    test_sha1_valid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_SHA1_C) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_sha1_invalid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_SHA1_C)
    test_mbedtls_sha1_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_SHA256_C)
    test_sha256_valid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_SHA256_C) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_sha256_invalid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_SHA256_C)
    test_sha224_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_SHA256_C)
    test_mbedtls_sha256_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_SHA512_C)
    test_sha512_valid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_SHA512_C) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_sha512_invalid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 9 */

#if defined(MBEDTLS_SHA512_C)
    test_sha384_wrapper,
#else
    NULL,
#endif
/* Function Id: 10 */

#if defined(MBEDTLS_SHA512_C)
    test_mbedtls_sha512_wrapper,
#else
    NULL,
#endif
/* Function Id: 11 */

#if defined(MBEDTLS_SHA1_C) && defined(MBEDTLS_SELF_TEST)
    test_sha1_selftest_wrapper,
#else
    NULL,
#endif
/* Function Id: 12 */

#if defined(MBEDTLS_SHA256_C) && defined(MBEDTLS_SELF_TEST)
    test_sha256_selftest_wrapper,
#else
    NULL,
#endif
/* Function Id: 13 */

#if defined(MBEDTLS_SHA512_C) && defined(MBEDTLS_SELF_TEST)
    test_sha512_selftest_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_shax.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
