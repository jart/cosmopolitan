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
 * Test file      : ./test_suite_hkdf.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_hkdf.function
 *      Test suite data     : suites/test_suite_hkdf.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_HKDF_C)
#include "third_party/mbedtls/hkdf.h"

void test_test_hkdf( int md_alg, data_t *ikm, data_t *salt, data_t *info,
                     data_t *expected_okm )
{
    int ret;
    unsigned char okm[128] = { '\0' };
    const mbedtls_md_info_t *md = mbedtls_md_info_from_type( md_alg );
    TEST_ASSERT( md != NULL );
    TEST_ASSERT( expected_okm->len <= sizeof( okm ) );
    ret = mbedtls_hkdf( md, salt->x, salt->len, ikm->x, ikm->len,
                        info->x, info->len, okm, expected_okm->len );
    TEST_ASSERT( ret == 0 );
    ASSERT_COMPARE( okm            , expected_okm->len,
                    expected_okm->x, expected_okm->len );
}

void test_test_hkdf_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};
    data_t data3 = {(uint8_t *) params[3], *( (uint32_t *) params[4] )};
    data_t data5 = {(uint8_t *) params[5], *( (uint32_t *) params[6] )};
    data_t data7 = {(uint8_t *) params[7], *( (uint32_t *) params[8] )};
    test_test_hkdf( *( (int *) params[0] ), &data1, &data3, &data5, &data7 );
}

void test_test_hkdf_extract( int md_alg, char *hex_ikm_string,
                             char *hex_salt_string, char *hex_prk_string )
{
    int ret;
    unsigned char *ikm = NULL;
    unsigned char *salt = NULL;
    unsigned char *prk = NULL;
    unsigned char *output_prk = NULL;
    size_t ikm_len, salt_len, prk_len, output_prk_len;
    const mbedtls_md_info_t *md = mbedtls_md_info_from_type( md_alg );
    TEST_ASSERT( md != NULL );
    output_prk_len = mbedtls_md_get_size( md );
    output_prk = mbedtls_calloc( 1, output_prk_len );
    ikm = mbedtls_test_unhexify_alloc( hex_ikm_string, &ikm_len );
    salt = mbedtls_test_unhexify_alloc( hex_salt_string, &salt_len );
    prk = mbedtls_test_unhexify_alloc( hex_prk_string, &prk_len );
    ret = mbedtls_hkdf_extract( md, salt, salt_len, ikm, ikm_len, output_prk );
    TEST_ASSERT( ret == 0 );
    ASSERT_COMPARE( output_prk, output_prk_len, prk, prk_len );
exit:
    mbedtls_free(ikm);
    mbedtls_free(salt);
    mbedtls_free(prk);
    mbedtls_free(output_prk);
}

void test_test_hkdf_extract_wrapper( void ** params )
{
    test_test_hkdf_extract( *( (int  *) params[0] ),
                               (char *) params[1],
                               (char *) params[2],
                               (char *) params[3] );
}

void test_test_hkdf_expand( int md_alg, char *hex_info_string,
                       char *hex_prk_string, char *hex_okm_string )
{
    enum { OKM_LEN = 1024 };
    int ret;
    unsigned char *info = NULL;
    unsigned char *prk = NULL;
    unsigned char *okm = NULL;
    unsigned char *output_okm = NULL;
    size_t info_len, prk_len, okm_len;
    const mbedtls_md_info_t *md = mbedtls_md_info_from_type( md_alg );
    TEST_ASSERT( md != NULL );
    output_okm = mbedtls_calloc( OKM_LEN, 1 );
    prk = mbedtls_test_unhexify_alloc( hex_prk_string, &prk_len );
    info = mbedtls_test_unhexify_alloc( hex_info_string, &info_len );
    okm = mbedtls_test_unhexify_alloc( hex_okm_string, &okm_len );
    TEST_ASSERT( prk_len == mbedtls_md_get_size( md ) );
    TEST_ASSERT( okm_len < OKM_LEN );
    ret = mbedtls_hkdf_expand( md, prk, prk_len, info, info_len,
                               output_okm, OKM_LEN );
    TEST_ASSERT( ret == 0 );
    ASSERT_COMPARE( output_okm, okm_len, okm, okm_len );
exit:
    mbedtls_free(info);
    mbedtls_free(prk);
    mbedtls_free(okm);
    mbedtls_free(output_okm);
}

void test_test_hkdf_expand_wrapper( void ** params )
{
    test_test_hkdf_expand( *( (int  *) params[0] ),
                              (char *) params[1],
                              (char *) params[2],
                              (char *) params[3] );
}

void test_test_hkdf_extract_ret( int hash_len, int ret )
{
    int output_ret;
    unsigned char *salt = NULL;
    unsigned char *ikm = NULL;
    unsigned char *prk = NULL;
    size_t salt_len, ikm_len;
    struct mbedtls_md_info_t fake_md_info;
    memset( &fake_md_info, 0, sizeof( fake_md_info ) );
    fake_md_info.type = MBEDTLS_MD_NONE;
    fake_md_info.size = hash_len;
    prk = mbedtls_calloc( MBEDTLS_MD_MAX_SIZE, 1 );
    salt_len = 0;
    ikm_len = 0;
    output_ret = mbedtls_hkdf_extract( &fake_md_info, salt, salt_len,
                                       ikm, ikm_len, prk );
    TEST_ASSERT( output_ret == ret );
exit:
    mbedtls_free(prk);
}

void test_test_hkdf_extract_ret_wrapper( void ** params )
{
    test_test_hkdf_extract_ret( *( (int *) params[0] ),
                                *( (int *) params[1] ) );
}

void test_test_hkdf_expand_ret( int hash_len, int prk_len, int okm_len, int ret )
{
    int output_ret;
    unsigned char *info = NULL;
    unsigned char *prk = NULL;
    unsigned char *okm = NULL;
    size_t info_len;
    struct mbedtls_md_info_t fake_md_info;
    memset( &fake_md_info, 0, sizeof( fake_md_info ) );
    fake_md_info.type = MBEDTLS_MD_NONE;
    fake_md_info.size = hash_len;
    info_len = 0;
    if (prk_len > 0)
        prk = mbedtls_calloc( prk_len, 1 );
    if (okm_len > 0)
        okm = mbedtls_calloc( okm_len, 1 );
    output_ret = mbedtls_hkdf_expand( &fake_md_info, prk, prk_len,
                                      info, info_len, okm, okm_len );
    TEST_ASSERT( output_ret == ret );
exit:
    mbedtls_free(prk);
    mbedtls_free(okm);
}

void test_test_hkdf_expand_ret_wrapper( void ** params )
{
    test_test_hkdf_expand_ret( *( (int *) params[0] ), *( (int *) params[1] ), *( (int *) params[2] ), *( (int *) params[3] ) );
}
#endif /* MBEDTLS_HKDF_C */

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
    switch( exp_id )
    {
#if defined(MBEDTLS_HKDF_C)
        case 0:
            *out_value = MBEDTLS_ERR_HKDF_BAD_INPUT_DATA;
            break;
#endif
        default:
            ret = KEY_VALUE_MAPPING_NOT_FOUND;
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
    switch( dep_id )
    {
#if defined(MBEDTLS_HKDF_C)
        case 0:
            {
#if defined(MBEDTLS_SHA256_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_SHA1_C)
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
#if defined(MBEDTLS_HKDF_C)
    test_test_hkdf_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */
#if defined(MBEDTLS_HKDF_C)
    test_test_hkdf_extract_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */
#if defined(MBEDTLS_HKDF_C)
    test_test_hkdf_expand_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */
#if defined(MBEDTLS_HKDF_C)
    test_test_hkdf_extract_ret_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */
#if defined(MBEDTLS_HKDF_C)
    test_test_hkdf_expand_ret_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_hkdf.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
