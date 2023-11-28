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
 * Test file      : ./test_suite_hmac_drbg.nopr.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_hmac_drbg.function
 *      Test suite data     : suites/test_suite_hmac_drbg.nopr.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_HMAC_DRBG_C)
#include "third_party/mbedtls/hmac_drbg.h"

typedef struct
{
    unsigned char *p;
    size_t len;
} entropy_ctx;

static int mbedtls_test_entropy_func( void *data, unsigned char *buf, size_t len )
{
    entropy_ctx *ctx = (entropy_ctx *) data;

    if( len > ctx->len )
        return( -1 );

    memcpy( buf, ctx->p, len );

    ctx->p += len;
    ctx->len -= len;

    return( 0 );
}
void test_hmac_drbg_entropy_usage( int md_alg )
{
    unsigned char out[16];
    unsigned char buf[1024];
    const mbedtls_md_info_t *md_info;
    mbedtls_hmac_drbg_context ctx;
    entropy_ctx entropy;
    size_t i, reps = 10;
    size_t default_entropy_len;
    size_t expected_consumed_entropy = 0;

    mbedtls_hmac_drbg_init( &ctx );
    memset( buf, 0, sizeof( buf ) );
    memset( out, 0, sizeof( out ) );

    entropy.len = sizeof( buf );
    entropy.p = buf;

    md_info = mbedtls_md_info_from_type( md_alg );
    TEST_ASSERT( md_info != NULL );
    if( mbedtls_md_get_size( md_info ) <= 20 )
        default_entropy_len = 16;
    else if( mbedtls_md_get_size( md_info ) <= 28 )
        default_entropy_len = 24;
    else
        default_entropy_len = 32;

    /* Set reseed interval before seed */
    mbedtls_hmac_drbg_set_reseed_interval( &ctx, 2 * reps );

    /* Init must use entropy */
    TEST_ASSERT( mbedtls_hmac_drbg_seed( &ctx, md_info, mbedtls_test_entropy_func, &entropy,
                                 NULL, 0 ) == 0 );
    /* default_entropy_len of entropy, plus half as much for the nonce */
    expected_consumed_entropy += default_entropy_len * 3 / 2;
    TEST_EQUAL( sizeof( buf )  - entropy.len, expected_consumed_entropy );

    /* By default, PR is off, and reseed interval was set to
     * 2 * reps so the next few calls should not use entropy */
    for( i = 0; i < reps; i++ )
    {
        TEST_ASSERT( mbedtls_hmac_drbg_random( &ctx, out, sizeof( out ) - 4 ) == 0 );
        TEST_ASSERT( mbedtls_hmac_drbg_random_with_add( &ctx, out, sizeof( out ) - 4,
                                                buf, 16 ) == 0 );
    }
    TEST_EQUAL( sizeof( buf )  - entropy.len, expected_consumed_entropy );

    /* While at it, make sure we didn't write past the requested length */
    TEST_ASSERT( out[sizeof( out ) - 4] == 0 );
    TEST_ASSERT( out[sizeof( out ) - 3] == 0 );
    TEST_ASSERT( out[sizeof( out ) - 2] == 0 );
    TEST_ASSERT( out[sizeof( out ) - 1] == 0 );

    /* There have been 2 * reps calls to random. The next call should reseed */
    TEST_ASSERT( mbedtls_hmac_drbg_random( &ctx, out, sizeof( out ) ) == 0 );
    expected_consumed_entropy += default_entropy_len;
    TEST_EQUAL( sizeof( buf )  - entropy.len, expected_consumed_entropy );

    /* Set reseed interval after seed */
    mbedtls_hmac_drbg_set_reseed_interval( &ctx, 4 * reps + 1);

    /* The new few calls should not reseed */
    for( i = 0; i < (2 * reps); i++ )
    {
        TEST_ASSERT( mbedtls_hmac_drbg_random( &ctx, out, sizeof( out ) ) == 0 );
        TEST_ASSERT( mbedtls_hmac_drbg_random_with_add( &ctx, out, sizeof( out ) ,
                                                buf, 16 ) == 0 );
    }
    TEST_EQUAL( sizeof( buf )  - entropy.len, expected_consumed_entropy );

    /* Now enable PR, so the next few calls should all reseed */
    mbedtls_hmac_drbg_set_prediction_resistance( &ctx, MBEDTLS_HMAC_DRBG_PR_ON );
    TEST_ASSERT( mbedtls_hmac_drbg_random( &ctx, out, sizeof( out ) ) == 0 );
    expected_consumed_entropy += default_entropy_len;
    TEST_EQUAL( sizeof( buf )  - entropy.len, expected_consumed_entropy );

    /* Finally, check setting entropy_len */
    mbedtls_hmac_drbg_set_entropy_len( &ctx, 42 );
    TEST_ASSERT( mbedtls_hmac_drbg_random( &ctx, out, sizeof( out ) ) == 0 );
    expected_consumed_entropy += 42;
    TEST_EQUAL( sizeof( buf )  - entropy.len, expected_consumed_entropy );

    mbedtls_hmac_drbg_set_entropy_len( &ctx, 13 );
    TEST_ASSERT( mbedtls_hmac_drbg_random( &ctx, out, sizeof( out ) ) == 0 );
    expected_consumed_entropy += 13;
    TEST_EQUAL( sizeof( buf )  - entropy.len, expected_consumed_entropy );

exit:
    mbedtls_hmac_drbg_free( &ctx );
}

void test_hmac_drbg_entropy_usage_wrapper( void ** params )
{

    test_hmac_drbg_entropy_usage( *( (int *) params[0] ) );
}
#if defined(MBEDTLS_FS_IO)
void test_hmac_drbg_seed_file( int md_alg, char * path, int ret )
{
    const mbedtls_md_info_t *md_info;
    mbedtls_hmac_drbg_context ctx;

    mbedtls_hmac_drbg_init( &ctx );

    md_info = mbedtls_md_info_from_type( md_alg );
    TEST_ASSERT( md_info != NULL );

    TEST_ASSERT( mbedtls_hmac_drbg_seed( &ctx, md_info,
                                         mbedtls_test_rnd_std_rand, NULL,
                                         NULL, 0 ) == 0 );

    TEST_ASSERT( mbedtls_hmac_drbg_write_seed_file( &ctx, path ) == ret );
    TEST_ASSERT( mbedtls_hmac_drbg_update_seed_file( &ctx, path ) == ret );

exit:
    mbedtls_hmac_drbg_free( &ctx );
}

void test_hmac_drbg_seed_file_wrapper( void ** params )
{

    test_hmac_drbg_seed_file( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ) );
}
#endif /* MBEDTLS_FS_IO */
void test_hmac_drbg_buf( int md_alg )
{
    unsigned char out[16];
    unsigned char buf[100];
    const mbedtls_md_info_t *md_info;
    mbedtls_hmac_drbg_context ctx;
    size_t i;

    mbedtls_hmac_drbg_init( &ctx );
    memset( buf, 0, sizeof( buf ) );
    memset( out, 0, sizeof( out ) );

    md_info = mbedtls_md_info_from_type( md_alg );
    TEST_ASSERT( md_info != NULL );
    TEST_ASSERT( mbedtls_hmac_drbg_seed_buf( &ctx, md_info, buf, sizeof( buf ) ) == 0 );

    /* Make sure it never tries to reseed (would segfault otherwise) */
    mbedtls_hmac_drbg_set_reseed_interval( &ctx, 3 );
    mbedtls_hmac_drbg_set_prediction_resistance( &ctx, MBEDTLS_HMAC_DRBG_PR_ON );

    for( i = 0; i < 30; i++ )
        TEST_ASSERT( mbedtls_hmac_drbg_random( &ctx, out, sizeof( out ) ) == 0 );

exit:
    mbedtls_hmac_drbg_free( &ctx );
}

void test_hmac_drbg_buf_wrapper( void ** params )
{

    test_hmac_drbg_buf( *( (int *) params[0] ) );
}
void test_hmac_drbg_no_reseed( int md_alg, data_t * entropy,
                          data_t * custom, data_t * add1,
                          data_t * add2, data_t * output )
{
    unsigned char data[1024];
    unsigned char my_output[512];
    entropy_ctx p_entropy;
    const mbedtls_md_info_t *md_info;
    mbedtls_hmac_drbg_context ctx;

    mbedtls_hmac_drbg_init( &ctx );

    p_entropy.p = entropy->x;
    p_entropy.len = entropy->len;

    md_info = mbedtls_md_info_from_type( md_alg );
    TEST_ASSERT( md_info != NULL );

    /* Test the simplified buffer-based variant */
    memcpy( data, entropy->x, p_entropy.len );
    memcpy( data + p_entropy.len, custom->x, custom->len );
    TEST_ASSERT( mbedtls_hmac_drbg_seed_buf( &ctx, md_info,
                                     data, p_entropy.len + custom->len ) == 0 );
    TEST_ASSERT( mbedtls_hmac_drbg_random_with_add( &ctx, my_output, output->len,
                                            add1->x, add1->len ) == 0 );
    TEST_ASSERT( mbedtls_hmac_drbg_random_with_add( &ctx, my_output, output->len,
                                            add2->x, add2->len ) == 0 );

    /* Reset context for second run */
    mbedtls_hmac_drbg_free( &ctx );

    TEST_ASSERT( memcmp( my_output, output->x, output->len ) == 0 );

    /* And now the normal entropy-based variant */
    TEST_ASSERT( mbedtls_hmac_drbg_seed( &ctx, md_info, mbedtls_test_entropy_func, &p_entropy,
                                 custom->x, custom->len ) == 0 );
    TEST_ASSERT( mbedtls_hmac_drbg_random_with_add( &ctx, my_output, output->len,
                                            add1->x, add1->len ) == 0 );
    TEST_ASSERT( mbedtls_hmac_drbg_random_with_add( &ctx, my_output, output->len,
                                            add2->x, add2->len ) == 0 );
    TEST_ASSERT( memcmp( my_output, output->x, output->len ) == 0 );

exit:
    mbedtls_hmac_drbg_free( &ctx );
}

void test_hmac_drbg_no_reseed_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};

    test_hmac_drbg_no_reseed( *( (int *) params[0] ), &data1, &data3, &data5, &data7, &data9 );
}
void test_hmac_drbg_nopr( int md_alg, data_t * entropy, data_t * custom,
                     data_t * add1, data_t * add2, data_t * add3,
                     data_t * output )
{
    unsigned char my_output[512];
    entropy_ctx p_entropy;
    const mbedtls_md_info_t *md_info;
    mbedtls_hmac_drbg_context ctx;

    mbedtls_hmac_drbg_init( &ctx );

    p_entropy.p = entropy->x;
    p_entropy.len = entropy->len;

    md_info = mbedtls_md_info_from_type( md_alg );
    TEST_ASSERT( md_info != NULL );

    TEST_ASSERT( mbedtls_hmac_drbg_seed( &ctx, md_info, mbedtls_test_entropy_func, &p_entropy,
                                 custom->x, custom->len ) == 0 );
    TEST_ASSERT( mbedtls_hmac_drbg_reseed( &ctx, add1->x, add1->len ) == 0 );
    TEST_ASSERT( mbedtls_hmac_drbg_random_with_add( &ctx, my_output, output->len,
                                            add2->x, add2->len ) == 0 );
    TEST_ASSERT( mbedtls_hmac_drbg_random_with_add( &ctx, my_output, output->len,
                                            add3->x, add3->len ) == 0 );

    TEST_ASSERT( memcmp( my_output, output->x, output->len ) == 0 );

exit:
    mbedtls_hmac_drbg_free( &ctx );
}

void test_hmac_drbg_nopr_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};
    data_t data11 = {(uint8_t *) params[11], *( (uint32_t *) params[12] )};

    test_hmac_drbg_nopr( *( (int *) params[0] ), &data1, &data3, &data5, &data7, &data9, &data11 );
}
void test_hmac_drbg_pr( int md_alg, data_t * entropy, data_t * custom,
                   data_t * add1, data_t * add2, data_t * output )
{
    unsigned char my_output[512];
    entropy_ctx p_entropy;
    const mbedtls_md_info_t *md_info;
    mbedtls_hmac_drbg_context ctx;

    mbedtls_hmac_drbg_init( &ctx );

    p_entropy.p = entropy->x;
    p_entropy.len = entropy->len;

    md_info = mbedtls_md_info_from_type( md_alg );
    TEST_ASSERT( md_info != NULL );

    TEST_ASSERT( mbedtls_hmac_drbg_seed( &ctx, md_info, mbedtls_test_entropy_func, &p_entropy,
                                 custom->x, custom->len ) == 0 );
    mbedtls_hmac_drbg_set_prediction_resistance( &ctx, MBEDTLS_HMAC_DRBG_PR_ON );
    TEST_ASSERT( mbedtls_hmac_drbg_random_with_add( &ctx, my_output, output->len,
                                            add1->x, add1->len ) == 0 );
    TEST_ASSERT( mbedtls_hmac_drbg_random_with_add( &ctx, my_output, output->len,
                                            add2->x, add2->len ) == 0 );

    TEST_ASSERT( memcmp( my_output, output->x, output->len ) == 0 );

exit:
    mbedtls_hmac_drbg_free( &ctx );
}

void test_hmac_drbg_pr_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    data_t data9 = {(uint8_t *) params[9], *( (uint32_t *) params[10] )};

    test_hmac_drbg_pr( *( (int *) params[0] ), &data1, &data3, &data5, &data7, &data9 );
}
#if defined(MBEDTLS_SELF_TEST)
void test_hmac_drbg_selftest(  )
{
    TEST_ASSERT( mbedtls_hmac_drbg_self_test( 1 ) == 0 );
exit:
    ;
}

void test_hmac_drbg_selftest_wrapper( void ** params )
{
    (void)params;

    test_hmac_drbg_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_HMAC_DRBG_C */

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

#if defined(MBEDTLS_HMAC_DRBG_C)

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

#if defined(MBEDTLS_HMAC_DRBG_C)

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

#if defined(MBEDTLS_HMAC_DRBG_C)
    test_hmac_drbg_entropy_usage_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_HMAC_DRBG_C) && defined(MBEDTLS_FS_IO)
    test_hmac_drbg_seed_file_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_HMAC_DRBG_C)
    test_hmac_drbg_buf_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_HMAC_DRBG_C)
    test_hmac_drbg_no_reseed_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_HMAC_DRBG_C)
    test_hmac_drbg_nopr_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_HMAC_DRBG_C)
    test_hmac_drbg_pr_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_HMAC_DRBG_C) && defined(MBEDTLS_SELF_TEST)
    test_hmac_drbg_selftest_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_hmac_drbg.nopr.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
