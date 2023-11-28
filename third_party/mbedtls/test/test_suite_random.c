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
 * Test file      : ./test_suite_random.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_random.function
 *      Test suite data     : suites/test_suite_random.data
 *
 */


#define TEST_SUITE_ACTIVE


/* Test random generation as a whole. */

#include "third_party/mbedtls/bignum.h"
#include "third_party/mbedtls/ctr_drbg.h"
#include "third_party/mbedtls/ecdsa.h"
#include "third_party/mbedtls/entropy.h"
#include "third_party/mbedtls/hmac_drbg.h"

/* How many bytes to generate in each test case for repeated generation.
 * This must be high enough that the probability of generating the same
 * output twice is infinitesimal, but low enough that random generators
 * are willing to deliver that much. */
#define OUTPUT_SIZE 32

#if defined(MBEDTLS_ENTROPY_C)
#if !defined(MBEDTLS_TEST_NULL_ENTROPY)
#if defined(MBEDTLS_CTR_DRBG_C)
void test_random_twice_with_ctr_drbg( )
{
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context drbg;
    unsigned char output1[OUTPUT_SIZE];
    unsigned char output2[OUTPUT_SIZE];

    /* First round */
    mbedtls_entropy_init( &entropy );
    mbedtls_ctr_drbg_init( &drbg );
    TEST_EQUAL( 0, mbedtls_ctr_drbg_seed( &drbg,
                                          mbedtls_entropy_func, &entropy,
                                          NULL, 0 ) );
    TEST_EQUAL( 0, mbedtls_ctr_drbg_random( &drbg,
                                            output1, sizeof( output1 ) ) );
    mbedtls_ctr_drbg_free( &drbg );
    mbedtls_entropy_free( &entropy );

    /* Second round */
    mbedtls_entropy_init( &entropy );
    mbedtls_ctr_drbg_init( &drbg );
    TEST_EQUAL( 0, mbedtls_ctr_drbg_seed( &drbg,
                                          mbedtls_entropy_func, &entropy,
                                          NULL, 0 ) );
    TEST_EQUAL( 0, mbedtls_ctr_drbg_random( &drbg,
                                            output2, sizeof( output2 ) ) );
    mbedtls_ctr_drbg_free( &drbg );
    mbedtls_entropy_free( &entropy );

    /* The two rounds must generate different random data. */
    TEST_ASSERT( timingsafe_bcmp( output1, output2, OUTPUT_SIZE ) != 0 );

exit:
    mbedtls_ctr_drbg_free( &drbg );
    mbedtls_entropy_free( &entropy );
}

void test_random_twice_with_ctr_drbg_wrapper( void ** params )
{
    (void)params;

    test_random_twice_with_ctr_drbg(  );
}
#endif /* MBEDTLS_CTR_DRBG_C */
#endif /* !MBEDTLS_TEST_NULL_ENTROPY */
#endif /* MBEDTLS_ENTROPY_C */
#if defined(MBEDTLS_ENTROPY_C)
#if !defined(MBEDTLS_TEST_NULL_ENTROPY)
#if defined(MBEDTLS_HMAC_DRBG_C)
void test_random_twice_with_hmac_drbg( int md_type )
{
    mbedtls_entropy_context entropy;
    mbedtls_hmac_drbg_context drbg;
    unsigned char output1[OUTPUT_SIZE];
    unsigned char output2[OUTPUT_SIZE];
    const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type( md_type );

    /* First round */
    mbedtls_entropy_init( &entropy );
    mbedtls_hmac_drbg_init( &drbg );
    TEST_EQUAL( 0, mbedtls_hmac_drbg_seed( &drbg, md_info,
                                           mbedtls_entropy_func, &entropy,
                                           NULL, 0 ) );
    TEST_EQUAL( 0, mbedtls_hmac_drbg_random( &drbg,
                                             output1, sizeof( output1 ) ) );
    mbedtls_hmac_drbg_free( &drbg );
    mbedtls_entropy_free( &entropy );

    /* Second round */
    mbedtls_entropy_init( &entropy );
    mbedtls_hmac_drbg_init( &drbg );
    TEST_EQUAL( 0, mbedtls_hmac_drbg_seed( &drbg, md_info,
                                           mbedtls_entropy_func, &entropy,
                                           NULL, 0 ) );
    TEST_EQUAL( 0, mbedtls_hmac_drbg_random( &drbg,
                                             output2, sizeof( output2 ) ) );
    mbedtls_hmac_drbg_free( &drbg );
    mbedtls_entropy_free( &entropy );

    /* The two rounds must generate different random data. */
    TEST_ASSERT( timingsafe_bcmp( output1, output2, OUTPUT_SIZE ) != 0 );

exit:
    mbedtls_hmac_drbg_free( &drbg );
    mbedtls_entropy_free( &entropy );
}

void test_random_twice_with_hmac_drbg_wrapper( void ** params )
{

    test_random_twice_with_hmac_drbg( *( (int *) params[0] ) );
}
#endif /* MBEDTLS_HMAC_DRBG_C */
#endif /* !MBEDTLS_TEST_NULL_ENTROPY */
#endif /* MBEDTLS_ENTROPY_C */
#if defined(MBEDTLS_PSA_CRYPTO_C)
#if !defined(MBEDTLS_TEST_NULL_ENTROPY)
#if !defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
void test_random_twice_with_psa_from_classic( )
{
    unsigned char output1[OUTPUT_SIZE];
    unsigned char output2[OUTPUT_SIZE];

    /* First round */
    PSA_ASSERT( psa_crypto_init( ) );
    TEST_EQUAL( 0, mbedtls_psa_get_random( MBEDTLS_PSA_RANDOM_STATE,
                                           output1, sizeof( output1 ) ) );
    PSA_DONE( );

    /* Second round */
    PSA_ASSERT( psa_crypto_init( ) );
    TEST_EQUAL( 0, mbedtls_psa_get_random( MBEDTLS_PSA_RANDOM_STATE,
                                           output2, sizeof( output2 ) ) );
    PSA_DONE( );

    /* The two rounds must generate different random data. */
    TEST_ASSERT( timingsafe_bcmp( output1, output2, OUTPUT_SIZE ) != 0 );

exit:
    PSA_DONE( );
}

void test_random_twice_with_psa_from_classic_wrapper( void ** params )
{
    (void)params;

    test_random_twice_with_psa_from_classic(  );
}
#endif /* !MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */
#endif /* !MBEDTLS_TEST_NULL_ENTROPY */
#endif /* MBEDTLS_PSA_CRYPTO_C */
#if defined(MBEDTLS_PSA_CRYPTO_C)
#if !defined(MBEDTLS_TEST_NULL_ENTROPY)
#if !defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
void test_random_twice_with_psa_from_psa( )
{
    unsigned char output1[OUTPUT_SIZE];
    unsigned char output2[OUTPUT_SIZE];

    /* First round */
    PSA_ASSERT( psa_crypto_init( ) );
    PSA_ASSERT( psa_generate_random( output1, sizeof( output1 ) ) );
    PSA_DONE( );

    /* Second round */
    PSA_ASSERT( psa_crypto_init( ) );
    PSA_ASSERT( psa_generate_random( output2, sizeof( output2 ) ) );
    PSA_DONE( );

    /* The two rounds must generate different random data. */
    TEST_ASSERT( timingsafe_bcmp( output1, output2, OUTPUT_SIZE ) != 0 );

exit:
    PSA_DONE( );
}

void test_random_twice_with_psa_from_psa_wrapper( void ** params )
{
    (void)params;

    test_random_twice_with_psa_from_psa(  );
}
#endif /* !MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG */
#endif /* !MBEDTLS_TEST_NULL_ENTROPY */
#endif /* MBEDTLS_PSA_CRYPTO_C */
#if defined(MBEDTLS_PSA_CRYPTO_C)
void test_mbedtls_psa_get_random_no_init( )
{
    unsigned char output[1];

    TEST_ASSERT( mbedtls_psa_get_random( MBEDTLS_PSA_RANDOM_STATE,
                                         output, sizeof( output ) ) != 0 );
exit:
    ;
}

void test_mbedtls_psa_get_random_no_init_wrapper( void ** params )
{
    (void)params;

    test_mbedtls_psa_get_random_no_init(  );
}
#endif /* MBEDTLS_PSA_CRYPTO_C */
#if defined(MBEDTLS_PSA_CRYPTO_C)
void test_mbedtls_psa_get_random_length( int n )
{
    unsigned char *output = NULL;

    PSA_ASSERT( psa_crypto_init( ) );
    ASSERT_ALLOC( output, n );

    TEST_EQUAL( 0, mbedtls_psa_get_random( MBEDTLS_PSA_RANDOM_STATE,
                                           output, n ) );
exit:
    mbedtls_free( output );
    PSA_DONE( );
}

void test_mbedtls_psa_get_random_length_wrapper( void ** params )
{

    test_mbedtls_psa_get_random_length( *( (int *) params[0] ) );
}
#endif /* MBEDTLS_PSA_CRYPTO_C */
#if defined(MBEDTLS_PSA_CRYPTO_C)
#if defined(MBEDTLS_ECDSA_C)
void test_mbedtls_psa_get_random_ecdsa_sign( int curve )
{
    mbedtls_ecp_group grp;
    mbedtls_mpi d, r, s;
    unsigned char buf[] = "This is not a hash.";

    mbedtls_ecp_group_init( &grp );
    mbedtls_mpi_init( &d );
    mbedtls_mpi_init( &r );
    mbedtls_mpi_init( &s );

    TEST_EQUAL( 0, mbedtls_mpi_lset( &d, 123456789 ) );
    TEST_EQUAL( 0, mbedtls_ecp_group_load( &grp, curve ) );
    PSA_ASSERT( psa_crypto_init( ) );
    TEST_EQUAL( 0, mbedtls_ecdsa_sign( &grp, &r, &s, &d,
                                       buf, sizeof( buf ),
                                       mbedtls_psa_get_random,
                                       MBEDTLS_PSA_RANDOM_STATE ) );
exit:
    mbedtls_mpi_free( &d );
    mbedtls_mpi_free( &r );
    mbedtls_mpi_free( &s );
    mbedtls_ecp_group_free( &grp );
    PSA_DONE( );
}

void test_mbedtls_psa_get_random_ecdsa_sign_wrapper( void ** params )
{

    test_mbedtls_psa_get_random_ecdsa_sign( *( (int *) params[0] ) );
}
#endif /* MBEDTLS_ECDSA_C */
#endif /* MBEDTLS_PSA_CRYPTO_C */

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

        case 0:
            {
                *out_value = MBEDTLS_MD_SHA1;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_MD_SHA256;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_MD_SHA512;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_CTR_DRBG_MAX_REQUEST;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_HMAC_DRBG_MAX_REQUEST;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_ECP_DP_SECP256R1;
            }
            break;
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
#if defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 4:
            {
#if !defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 5:
            {
#if defined(MBEDTLS_CTR_DRBG_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 6:
            {
#if !defined(MBEDTLS_CTR_DRBG_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 7:
            {
#if defined(MBEDTLS_HMAC_DRBG_C)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 8:
            {
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
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

#if defined(MBEDTLS_ENTROPY_C) && !defined(MBEDTLS_TEST_NULL_ENTROPY) && defined(MBEDTLS_CTR_DRBG_C)
    test_random_twice_with_ctr_drbg_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_ENTROPY_C) && !defined(MBEDTLS_TEST_NULL_ENTROPY) && defined(MBEDTLS_HMAC_DRBG_C)
    test_random_twice_with_hmac_drbg_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && !defined(MBEDTLS_TEST_NULL_ENTROPY) && !defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
    test_random_twice_with_psa_from_classic_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && !defined(MBEDTLS_TEST_NULL_ENTROPY) && !defined(MBEDTLS_PSA_CRYPTO_EXTERNAL_RNG)
    test_random_twice_with_psa_from_psa_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_PSA_CRYPTO_C)
    test_mbedtls_psa_get_random_no_init_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_PSA_CRYPTO_C)
    test_mbedtls_psa_get_random_length_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_PSA_CRYPTO_C) && defined(MBEDTLS_ECDSA_C)
    test_mbedtls_psa_get_random_ecdsa_sign_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_random.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
