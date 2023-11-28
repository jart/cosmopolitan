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
 * Test file      : ./test_suite_dhm.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_dhm.function
 *      Test suite data     : suites/test_suite_dhm.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_DHM_C)
#if defined(MBEDTLS_BIGNUM_C)
#include "third_party/mbedtls/dhm.h"
#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_dhm_invalid_params( )
{
    mbedtls_dhm_context ctx;
    unsigned char buf[42] = { 0 };
    unsigned char *buf_null = NULL;
    mbedtls_mpi X;
    size_t const buflen = sizeof( buf );
    size_t len;

    TEST_INVALID_PARAM( mbedtls_dhm_init( NULL ) );
    TEST_VALID_PARAM( mbedtls_dhm_free( NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_read_params( NULL,
                                                     (unsigned char**) &buf,
                                                     buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_read_params( &ctx, &buf_null, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_read_params( &ctx, NULL, buf ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_read_params( &ctx,
                                                     (unsigned char**) &buf,
                                                     NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_make_params( NULL, buflen,
                                                     buf, &len,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_make_params( &ctx, buflen,
                                                     NULL, &len,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_make_params( &ctx, buflen,
                                                     buf, NULL,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_make_params( &ctx, buflen,
                                                     buf, &len,
                                                     NULL,
                                                     NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_set_group( NULL, &X, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_set_group( &ctx, NULL, &X ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_set_group( &ctx, &X, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_read_public( NULL, buf, buflen ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_read_public( &ctx, NULL, buflen ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_make_public( NULL, buflen,
                                                     buf, buflen,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_make_public( &ctx, buflen,
                                                     NULL, buflen,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_make_public( &ctx, buflen,
                                                     buf, buflen,
                                                     NULL,
                                                     NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_calc_secret( NULL, buf, buflen, &len,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_calc_secret( &ctx, NULL, buflen, &len,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_calc_secret( &ctx, buf, buflen, NULL,
                                                     mbedtls_test_rnd_std_rand,
                                                     NULL ) );

#if defined(MBEDTLS_ASN1_PARSE_C)
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_parse_dhm( NULL, buf, buflen ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_parse_dhm( &ctx, NULL, buflen ) );

#if defined(MBEDTLS_FS_IO)
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_parse_dhmfile( NULL, "" ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_DHM_BAD_INPUT_DATA,
                            mbedtls_dhm_parse_dhmfile( &ctx, NULL ) );
#endif /* MBEDTLS_FS_IO */
#endif /* MBEDTLS_ASN1_PARSE_C */

exit:
    return;
}

void test_dhm_invalid_params_wrapper( void ** params )
{
    (void)params;

    test_dhm_invalid_params(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
void test_dhm_do_dhm( int radix_P, char *input_P,
                 int radix_G, char *input_G, int result )
{
    mbedtls_dhm_context ctx_srv;
    mbedtls_dhm_context ctx_cli;
    unsigned char ske[1000];
    unsigned char *p = ske;
    unsigned char pub_cli[1000];
    unsigned char sec_srv[1000];
    unsigned char sec_cli[1000];
    size_t ske_len = 0;
    size_t pub_cli_len = 0;
    size_t sec_srv_len;
    size_t sec_cli_len;
    int x_size, i;
    mbedtls_test_rnd_pseudo_info rnd_info;

    mbedtls_dhm_init( &ctx_srv );
    mbedtls_dhm_init( &ctx_cli );
    memset( ske, 0x00, 1000 );
    memset( pub_cli, 0x00, 1000 );
    memset( sec_srv, 0x00, 1000 );
    memset( sec_cli, 0x00, 1000 );
    memset( &rnd_info, 0x00, sizeof( mbedtls_test_rnd_pseudo_info ) );

    /*
     * Set params
     */
    TEST_ASSERT( mbedtls_mpi_read_string( &ctx_srv.P, radix_P, input_P ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &ctx_srv.G, radix_G, input_G ) == 0 );
    x_size = mbedtls_mpi_size( &ctx_srv.P );
    pub_cli_len = x_size;

    /*
     * First key exchange
     */
    TEST_ASSERT( mbedtls_dhm_make_params( &ctx_srv, x_size, ske, &ske_len,
                                          &mbedtls_test_rnd_pseudo_rand,
                                          &rnd_info ) == result );
    if ( result != 0 )
        goto exit;

    ske[ske_len++] = 0;
    ske[ske_len++] = 0;
    TEST_ASSERT( mbedtls_dhm_read_params( &ctx_cli, &p, ske + ske_len ) == 0 );

    TEST_ASSERT( mbedtls_dhm_make_public( &ctx_cli, x_size, pub_cli, pub_cli_len,
                                          &mbedtls_test_rnd_pseudo_rand,
                                          &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_dhm_read_public( &ctx_srv, pub_cli, pub_cli_len ) == 0 );

    TEST_ASSERT( mbedtls_dhm_calc_secret( &ctx_srv, sec_srv, sizeof( sec_srv ),
                                          &sec_srv_len,
                                          &mbedtls_test_rnd_pseudo_rand,
                                          &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_dhm_calc_secret( &ctx_cli, sec_cli, sizeof( sec_cli ), &sec_cli_len, NULL, NULL ) == 0 );

    TEST_ASSERT( sec_srv_len == sec_cli_len );
    TEST_ASSERT( sec_srv_len != 0 );
    TEST_ASSERT( timingsafe_bcmp( sec_srv, sec_cli, sec_srv_len ) == 0 );

    /* Re-do calc_secret on server a few times to test update of blinding values */
    for( i = 0; i < 3; i++ )
    {
        sec_srv_len = 1000;
        TEST_ASSERT( mbedtls_dhm_calc_secret( &ctx_srv, sec_srv,
                                              sizeof( sec_srv ), &sec_srv_len,
                                              &mbedtls_test_rnd_pseudo_rand,
                                              &rnd_info ) == 0 );

        TEST_ASSERT( sec_srv_len == sec_cli_len );
        TEST_ASSERT( sec_srv_len != 0 );
        TEST_ASSERT( timingsafe_bcmp( sec_srv, sec_cli, sec_srv_len ) == 0 );
    }

    /*
     * Second key exchange to test change of blinding values on server
     */
    p = ske;

    TEST_ASSERT( mbedtls_dhm_make_params( &ctx_srv, x_size, ske, &ske_len,
                                          &mbedtls_test_rnd_pseudo_rand,
                                          &rnd_info ) == 0 );
    ske[ske_len++] = 0;
    ske[ske_len++] = 0;
    TEST_ASSERT( mbedtls_dhm_read_params( &ctx_cli, &p, ske + ske_len ) == 0 );

    TEST_ASSERT( mbedtls_dhm_make_public( &ctx_cli, x_size, pub_cli, pub_cli_len,
                                          &mbedtls_test_rnd_pseudo_rand,
                                          &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_dhm_read_public( &ctx_srv, pub_cli, pub_cli_len ) == 0 );

    TEST_ASSERT( mbedtls_dhm_calc_secret( &ctx_srv, sec_srv, sizeof( sec_srv ),
                                          &sec_srv_len,
                                          &mbedtls_test_rnd_pseudo_rand,
                                          &rnd_info ) == 0 );
    TEST_ASSERT( mbedtls_dhm_calc_secret( &ctx_cli, sec_cli, sizeof( sec_cli ), &sec_cli_len, NULL, NULL ) == 0 );

    TEST_ASSERT( sec_srv_len == sec_cli_len );
    TEST_ASSERT( sec_srv_len != 0 );
    TEST_ASSERT( timingsafe_bcmp( sec_srv, sec_cli, sec_srv_len ) == 0 );

exit:
    mbedtls_dhm_free( &ctx_srv );
    mbedtls_dhm_free( &ctx_cli );
}

void test_dhm_do_dhm_wrapper( void ** params )
{

    test_dhm_do_dhm( *( (int *) params[0] ), (char *) params[1], *( (int *) params[2] ), (char *) params[3], *( (int *) params[4] ) );
}
void test_dhm_make_public( int P_bytes, int radix_G, char *input_G, int result )
{
    mbedtls_mpi P, G;
    mbedtls_dhm_context ctx;
    unsigned char output[MBEDTLS_MPI_MAX_SIZE];

    mbedtls_mpi_init( &P );
    mbedtls_mpi_init( &G );
    mbedtls_dhm_init( &ctx );

    TEST_ASSERT( mbedtls_mpi_lset( &P, 1 ) == 0 );
    TEST_ASSERT( mbedtls_mpi_shift_l( &P, ( P_bytes * 8 ) - 1 ) == 0 );
    TEST_ASSERT( mbedtls_mpi_set_bit( &P, 0, 1 ) == 0 );

    TEST_ASSERT( mbedtls_mpi_read_string( &G, radix_G, input_G ) == 0 );

    TEST_ASSERT( mbedtls_dhm_set_group( &ctx, &P, &G ) == 0 );
    TEST_ASSERT( mbedtls_dhm_make_public( &ctx, (int) mbedtls_mpi_size( &P ),
                                          output, sizeof(output),
                                          &mbedtls_test_rnd_pseudo_rand,
                                          NULL ) == result );

exit:
    mbedtls_mpi_free( &P );
    mbedtls_mpi_free( &G );
    mbedtls_dhm_free( &ctx );
}

void test_dhm_make_public_wrapper( void ** params )
{

    test_dhm_make_public( *( (int *) params[0] ), *( (int *) params[1] ), (char *) params[2], *( (int *) params[3] ) );
}
#if defined(MBEDTLS_FS_IO)
void test_dhm_file( char * filename, char * p, char * g, int len )
{
    mbedtls_dhm_context ctx;
    mbedtls_mpi P, G;

    mbedtls_dhm_init( &ctx );
    mbedtls_mpi_init( &P ); mbedtls_mpi_init( &G );

    TEST_ASSERT( mbedtls_mpi_read_string( &P, 16, p ) == 0 );
    TEST_ASSERT( mbedtls_mpi_read_string( &G, 16, g ) == 0 );

    TEST_ASSERT( mbedtls_dhm_parse_dhmfile( &ctx, filename ) == 0 );

    TEST_ASSERT( ctx.len == (size_t) len );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &ctx.P, &P ) == 0 );
    TEST_ASSERT( mbedtls_mpi_cmp_mpi( &ctx.G, &G ) == 0 );

exit:
    mbedtls_mpi_free( &P ); mbedtls_mpi_free( &G );
    mbedtls_dhm_free( &ctx );
}

void test_dhm_file_wrapper( void ** params )
{

    test_dhm_file( (char *) params[0], (char *) params[1], (char *) params[2], *( (int *) params[3] ) );
}
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_SELF_TEST)
void test_dhm_selftest(  )
{
    TEST_ASSERT( mbedtls_dhm_self_test( 1 ) == 0 );
exit:
    ;
}

void test_dhm_selftest_wrapper( void ** params )
{
    (void)params;

    test_dhm_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_BIGNUM_C */
#endif /* MBEDTLS_DHM_C */

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

#if defined(MBEDTLS_DHM_C) && defined(MBEDTLS_BIGNUM_C)

        case 0:
            {
                *out_value = MBEDTLS_ERR_DHM_BAD_INPUT_DATA;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_ERR_DHM_MAKE_PARAMS_FAILED;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_MPI_MAX_SIZE;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_MPI_MAX_SIZE + 1;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_ERR_DHM_MAKE_PUBLIC_FAILED+MBEDTLS_ERR_MPI_BAD_INPUT_DATA;
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

#if defined(MBEDTLS_DHM_C) && defined(MBEDTLS_BIGNUM_C)

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

#if defined(MBEDTLS_DHM_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_dhm_invalid_params_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_DHM_C) && defined(MBEDTLS_BIGNUM_C)
    test_dhm_do_dhm_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_DHM_C) && defined(MBEDTLS_BIGNUM_C)
    test_dhm_make_public_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_DHM_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_FS_IO)
    test_dhm_file_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_DHM_C) && defined(MBEDTLS_BIGNUM_C) && defined(MBEDTLS_SELF_TEST)
    test_dhm_selftest_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_dhm.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
