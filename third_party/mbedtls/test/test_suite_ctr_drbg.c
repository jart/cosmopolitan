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
 * Test file      : ./test_suite_ctr_drbg.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_ctr_drbg.function
 *      Test suite data     : suites/test_suite_ctr_drbg.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_CTR_DRBG_C)
#include "third_party/mbedtls/entropy.h"
#include "third_party/mbedtls/ctr_drbg.h"

/* Modes for ctr_drbg_validate */
enum reseed_mode
{
    RESEED_NEVER, /* never reseed */
    RESEED_FIRST, /* instantiate, reseed, generate, generate */
    RESEED_SECOND, /* instantiate, generate, reseed, generate */
    RESEED_ALWAYS /* prediction resistance, no explicit reseed */
};

static size_t test_offset_idx = 0;
static size_t test_max_idx  = 0;
static int mbedtls_test_entropy_func( void *data, unsigned char *buf, size_t len )
{
    const unsigned char *p = (unsigned char *) data;
    if( test_offset_idx + len > test_max_idx )
        return( MBEDTLS_ERR_ENTROPY_SOURCE_FAILED );
    memcpy( buf, p + test_offset_idx, len );
    test_offset_idx += len;
    return( 0 );
}

static void ctr_drbg_validate_internal( int reseed_mode, data_t * nonce,
                        int entropy_len_arg, data_t * entropy,
                        data_t * reseed,
                        data_t * add1, data_t * add2,
                        data_t * result )
{
    mbedtls_ctr_drbg_context ctx;
    unsigned char buf[64];

    size_t entropy_chunk_len = (size_t) entropy_len_arg;

    TEST_ASSERT( entropy_chunk_len <= sizeof( buf ) );

    test_offset_idx = 0;
    mbedtls_ctr_drbg_init( &ctx );

    test_max_idx = entropy->len;

    /* CTR_DRBG_Instantiate(entropy[:entropy->len], nonce, perso, <ignored>)
     * where nonce||perso = nonce[nonce->len] */
    mbedtls_ctr_drbg_set_entropy_len( &ctx, entropy_chunk_len );
    mbedtls_ctr_drbg_set_nonce_len( &ctx, 0 );
    TEST_ASSERT( mbedtls_ctr_drbg_seed(
                     &ctx,
                     mbedtls_test_entropy_func, entropy->x,
                     nonce->x, nonce->len ) == 0 );
    if( reseed_mode == RESEED_ALWAYS )
        mbedtls_ctr_drbg_set_prediction_resistance(
            &ctx,
            MBEDTLS_CTR_DRBG_PR_ON );

    if( reseed_mode == RESEED_FIRST )
    {
        /* CTR_DRBG_Reseed(entropy[idx:idx+entropy->len],
         *                 reseed[:reseed->len]) */
        TEST_ASSERT( mbedtls_ctr_drbg_reseed(
                         &ctx,
                         reseed->x, reseed->len ) == 0 );
    }

    /* CTR_DRBG_Generate(result->len * 8 bits, add1[:add1->len]) -> buf */
    /* Then reseed if prediction resistance is enabled. */
    TEST_ASSERT( mbedtls_ctr_drbg_random_with_add(
                     &ctx,
                     buf, result->len,
                     add1->x, add1->len ) == 0 );


    if( reseed_mode == RESEED_SECOND )
    {
        /* CTR_DRBG_Reseed(entropy[idx:idx+entropy->len],
         *                 reseed[:reseed->len]) */
        TEST_ASSERT( mbedtls_ctr_drbg_reseed(
                         &ctx,
                         reseed->x, reseed->len ) == 0 );
    }

    /* CTR_DRBG_Generate(result->len * 8 bits, add2->x[:add2->len]) -> buf */
    /* Then reseed if prediction resistance is enabled. */
    TEST_ASSERT( mbedtls_ctr_drbg_random_with_add(
                     &ctx,
                     buf, result->len,
                     add2->x, add2->len ) == 0 );
    TEST_ASSERT( timingsafe_bcmp( buf, result->x, result->len ) == 0 );

exit:
    mbedtls_ctr_drbg_free( &ctx );
}

void test_ctr_drbg_special_behaviours( )
{
    mbedtls_ctr_drbg_context ctx;
    unsigned char output[512];
    unsigned char additional[512];

    mbedtls_ctr_drbg_init( &ctx );
    memset( output, 0, sizeof( output ) );
    memset( additional, 0, sizeof( additional ) );

    TEST_ASSERT( mbedtls_ctr_drbg_random_with_add( &ctx,
                        output, MBEDTLS_CTR_DRBG_MAX_REQUEST + 1,
                        additional, 16 ) ==
                        MBEDTLS_ERR_CTR_DRBG_REQUEST_TOO_BIG );
    TEST_ASSERT( mbedtls_ctr_drbg_random_with_add( &ctx,
                        output, 16,
                        additional, MBEDTLS_CTR_DRBG_MAX_INPUT + 1 ) ==
                        MBEDTLS_ERR_CTR_DRBG_INPUT_TOO_BIG );

    TEST_ASSERT( mbedtls_ctr_drbg_reseed( &ctx, additional,
                        MBEDTLS_CTR_DRBG_MAX_SEED_INPUT + 1 ) ==
                        MBEDTLS_ERR_CTR_DRBG_INPUT_TOO_BIG );

    mbedtls_ctr_drbg_set_entropy_len( &ctx, ~0 );
    TEST_ASSERT( mbedtls_ctr_drbg_reseed( &ctx, additional,
                        MBEDTLS_CTR_DRBG_MAX_SEED_INPUT ) ==
                        MBEDTLS_ERR_CTR_DRBG_INPUT_TOO_BIG );
exit:
    mbedtls_ctr_drbg_free( &ctx );
}

void test_ctr_drbg_special_behaviours_wrapper( void ** params )
{
    (void)params;

    test_ctr_drbg_special_behaviours(  );
}
void test_ctr_drbg_validate_no_reseed( data_t * add_init, data_t * entropy,
                                  data_t * add1, data_t * add2,
                                  data_t * result_string )
{
    data_t empty = { 0, 0 };
    ctr_drbg_validate_internal( RESEED_NEVER, add_init,
                                entropy->len, entropy,
                                &empty, add1, add2,
                                result_string );
    goto exit; // goto is needed to avoid warning ( no test assertions in func)
exit:
    ;
}

void test_ctr_drbg_validate_no_reseed_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_ctr_drbg_validate_no_reseed( &data0, &data2, &data4, &data6, &data8 );
}
void test_ctr_drbg_validate_pr( data_t * add_init, data_t * entropy,
                           data_t * add1, data_t * add2,
                           data_t * result_string )
{
    data_t empty = { 0, 0 };
    ctr_drbg_validate_internal( RESEED_ALWAYS, add_init,
                                entropy->len / 3, entropy,
                                &empty, add1, add2,
                                result_string );
    goto exit; // goto is needed to avoid warning ( no test assertions in func)
exit:
    ;
}

void test_ctr_drbg_validate_pr_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};

    test_ctr_drbg_validate_pr( &data0, &data2, &data4, &data6, &data8 );
}
void test_ctr_drbg_validate_reseed_between( data_t * add_init, data_t * entropy,
                             data_t * add1, data_t * add_reseed,
                             data_t * add2, data_t * result_string )
{
    ctr_drbg_validate_internal( RESEED_SECOND, add_init,
                                entropy->len / 2, entropy,
                                add_reseed, add1, add2,
                                result_string );
    goto exit; // goto is needed to avoid warning ( no test assertions in func)
exit:
    ;
}

void test_ctr_drbg_validate_reseed_between_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};

    test_ctr_drbg_validate_reseed_between( &data0, &data2, &data4, &data6, &data8, &data10 );
}
void test_ctr_drbg_validate_reseed_first( data_t * add_init, data_t * entropy,
                             data_t * add1, data_t * add_reseed,
                             data_t * add2, data_t * result_string )
{
    ctr_drbg_validate_internal( RESEED_FIRST, add_init,
                                entropy->len / 2, entropy,
                                add_reseed, add1, add2,
                                result_string );
    goto exit; // goto is needed to avoid warning ( no test assertions in func)
exit:
    ;
}

void test_ctr_drbg_validate_reseed_first_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};
    data_t data2 = {(uint8_t *) params[2], *( (uint32_t *) params[3] )};
    data_t data4 = {(uint8_t *) params[4], *( (uint32_t *) params[5] )};
    data_t data6 = {(uint8_t *) params[6], *( (uint32_t *) params[7] )};
    data_t data8 = {(uint8_t *) params[8], *( (uint32_t *) params[9] )};
    data_t data10 = {(uint8_t *) params[10], *( (uint32_t *) params[11] )};

    test_ctr_drbg_validate_reseed_first( &data0, &data2, &data4, &data6, &data8, &data10 );
}
void test_ctr_drbg_entropy_strength( int expected_bit_strength )
{
    unsigned char entropy[/*initial entropy*/ MBEDTLS_CTR_DRBG_ENTROPY_LEN +
                          /*nonce*/     MBEDTLS_CTR_DRBG_ENTROPY_NONCE_LEN +
                          /*reseed*/          MBEDTLS_CTR_DRBG_ENTROPY_LEN];
    mbedtls_ctr_drbg_context ctx;
    size_t last_idx;
    size_t byte_strength = expected_bit_strength / 8;

    mbedtls_ctr_drbg_init( &ctx );
    test_offset_idx = 0;
    test_max_idx = sizeof( entropy );
    memset( entropy, 0, sizeof( entropy ) );

    /* The initial seeding must grab at least byte_strength bytes of entropy
     * for the entropy input and byte_strength/2 bytes for a nonce. */
    TEST_ASSERT( mbedtls_ctr_drbg_seed( &ctx,
                                        mbedtls_test_entropy_func, entropy,
                                        NULL, 0 ) == 0 );
    TEST_ASSERT( test_offset_idx >= ( byte_strength * 3 + 1 ) / 2 );
    last_idx = test_offset_idx;

    /* A reseed must grab at least byte_strength bytes of entropy. */
    TEST_ASSERT( mbedtls_ctr_drbg_reseed( &ctx, NULL, 0 ) == 0 );
    TEST_ASSERT( test_offset_idx - last_idx >= byte_strength );

exit:
    mbedtls_ctr_drbg_free( &ctx );
}

void test_ctr_drbg_entropy_strength_wrapper( void ** params )
{

    test_ctr_drbg_entropy_strength( *( (int *) params[0] ) );
}
void test_ctr_drbg_entropy_usage( int entropy_nonce_len )
{
    unsigned char out[16];
    unsigned char add[16];
    unsigned char entropy[1024];
    mbedtls_ctr_drbg_context ctx;
    size_t i, reps = 10;
    size_t expected_idx = 0;

    mbedtls_ctr_drbg_init( &ctx );
    test_offset_idx = 0;
    test_max_idx = sizeof( entropy );
    memset( entropy, 0, sizeof( entropy ) );
    memset( out, 0, sizeof( out ) );
    memset( add, 0, sizeof( add ) );

    if( entropy_nonce_len >= 0 )
        TEST_ASSERT( mbedtls_ctr_drbg_set_nonce_len( &ctx, entropy_nonce_len ) == 0 );

    /* Set reseed interval before seed */
    mbedtls_ctr_drbg_set_reseed_interval( &ctx, 2 * reps );

    /* Init must use entropy */
    TEST_ASSERT( mbedtls_ctr_drbg_seed( &ctx, mbedtls_test_entropy_func, entropy, NULL, 0 ) == 0 );
    expected_idx += MBEDTLS_CTR_DRBG_ENTROPY_LEN;
    if( entropy_nonce_len >= 0 )
        expected_idx += entropy_nonce_len;
    else
        expected_idx += MBEDTLS_CTR_DRBG_ENTROPY_NONCE_LEN;
    TEST_EQUAL( test_offset_idx, expected_idx );

    /* By default, PR is off, and reseed interval was set to
     * 2 * reps so the next few calls should not use entropy */
    for( i = 0; i < reps; i++ )
    {
        TEST_ASSERT( mbedtls_ctr_drbg_random( &ctx, out, sizeof( out ) - 4 ) == 0 );
        TEST_ASSERT( mbedtls_ctr_drbg_random_with_add( &ctx, out, sizeof( out ) - 4,
                                                add, sizeof( add ) ) == 0 );
    }
    TEST_EQUAL( test_offset_idx, expected_idx );

    /* While at it, make sure we didn't write past the requested length */
    TEST_ASSERT( out[sizeof( out ) - 4] == 0 );
    TEST_ASSERT( out[sizeof( out ) - 3] == 0 );
    TEST_ASSERT( out[sizeof( out ) - 2] == 0 );
    TEST_ASSERT( out[sizeof( out ) - 1] == 0 );

    /* There have been 2 * reps calls to random. The next call should reseed */
    TEST_ASSERT( mbedtls_ctr_drbg_random( &ctx, out, sizeof( out ) ) == 0 );
    expected_idx += MBEDTLS_CTR_DRBG_ENTROPY_LEN;
    TEST_EQUAL( test_offset_idx, expected_idx );

    /* Set reseed interval after seed */
    mbedtls_ctr_drbg_set_reseed_interval( &ctx, 4 * reps + 1 );

    /* The next few calls should not reseed */
    for( i = 0; i < (2 * reps); i++ )
    {
        TEST_ASSERT( mbedtls_ctr_drbg_random( &ctx, out, sizeof( out ) ) == 0 );
        TEST_ASSERT( mbedtls_ctr_drbg_random_with_add( &ctx, out, sizeof( out ) ,
                                                add, sizeof( add ) ) == 0 );
    }
    TEST_EQUAL( test_offset_idx, expected_idx );

    /* Call update with too much data (sizeof entropy > MAX(_SEED)_INPUT).
     * Make sure it's detected as an error and doesn't cause memory
     * corruption. */
    TEST_ASSERT( mbedtls_ctr_drbg_update_ret(
                     &ctx, entropy, sizeof( entropy ) ) != 0 );

    /* Now enable PR, so the next few calls should all reseed */
    mbedtls_ctr_drbg_set_prediction_resistance( &ctx, MBEDTLS_CTR_DRBG_PR_ON );
    TEST_ASSERT( mbedtls_ctr_drbg_random( &ctx, out, sizeof( out ) ) == 0 );
    expected_idx += MBEDTLS_CTR_DRBG_ENTROPY_LEN;
    TEST_EQUAL( test_offset_idx, expected_idx );

    /* Finally, check setting entropy_len */
    mbedtls_ctr_drbg_set_entropy_len( &ctx, 42 );
    TEST_ASSERT( mbedtls_ctr_drbg_random( &ctx, out, sizeof( out ) ) == 0 );
    expected_idx += 42;
    TEST_EQUAL( test_offset_idx, expected_idx );

    mbedtls_ctr_drbg_set_entropy_len( &ctx, 13 );
    TEST_ASSERT( mbedtls_ctr_drbg_random( &ctx, out, sizeof( out ) ) == 0 );
    expected_idx += 13;
    TEST_EQUAL( test_offset_idx, expected_idx );

exit:
    mbedtls_ctr_drbg_free( &ctx );
}

void test_ctr_drbg_entropy_usage_wrapper( void ** params )
{

    test_ctr_drbg_entropy_usage( *( (int *) params[0] ) );
}
#if defined(MBEDTLS_FS_IO)
void test_ctr_drbg_seed_file( char * path, int ret )
{
    mbedtls_ctr_drbg_context ctx;

    mbedtls_ctr_drbg_init( &ctx );

    TEST_ASSERT( mbedtls_ctr_drbg_seed( &ctx, mbedtls_test_rnd_std_rand,
                                        NULL, NULL, 0 ) == 0 );
    TEST_ASSERT( mbedtls_ctr_drbg_write_seed_file( &ctx, path ) == ret );
    TEST_ASSERT( mbedtls_ctr_drbg_update_seed_file( &ctx, path ) == ret );

exit:
    mbedtls_ctr_drbg_free( &ctx );
}

void test_ctr_drbg_seed_file_wrapper( void ** params )
{

    test_ctr_drbg_seed_file( (char *) params[0], *( (int *) params[1] ) );
}
#endif /* MBEDTLS_FS_IO */
#if defined(MBEDTLS_SELF_TEST)
void test_ctr_drbg_selftest(  )
{
    TEST_ASSERT( mbedtls_ctr_drbg_self_test( 1 ) == 0 );
exit:
    ;
}

void test_ctr_drbg_selftest_wrapper( void ** params )
{
    (void)params;

    test_ctr_drbg_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#endif /* MBEDTLS_CTR_DRBG_C */

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

#if defined(MBEDTLS_CTR_DRBG_C)

        case 0:
            {
                *out_value = -1;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_ERR_CTR_DRBG_FILE_IO_ERROR;
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

#if defined(MBEDTLS_CTR_DRBG_C)

        case 0:
            {
#if !defined(MBEDTLS_CTR_DRBG_USE_128_BIT_KEY)
                ret = DEPENDENCY_SUPPORTED;
#else
                ret = DEPENDENCY_NOT_SUPPORTED;
#endif
            }
            break;
        case 1:
            {
#if defined(MBEDTLS_CTR_DRBG_USE_128_BIT_KEY)
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

#if defined(MBEDTLS_CTR_DRBG_C)
    test_ctr_drbg_special_behaviours_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_CTR_DRBG_C)
    test_ctr_drbg_validate_no_reseed_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_CTR_DRBG_C)
    test_ctr_drbg_validate_pr_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_CTR_DRBG_C)
    test_ctr_drbg_validate_reseed_between_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_CTR_DRBG_C)
    test_ctr_drbg_validate_reseed_first_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_CTR_DRBG_C)
    test_ctr_drbg_entropy_strength_wrapper,
#else
    NULL,
#endif
/* Function Id: 6 */

#if defined(MBEDTLS_CTR_DRBG_C)
    test_ctr_drbg_entropy_usage_wrapper,
#else
    NULL,
#endif
/* Function Id: 7 */

#if defined(MBEDTLS_CTR_DRBG_C) && defined(MBEDTLS_FS_IO)
    test_ctr_drbg_seed_file_wrapper,
#else
    NULL,
#endif
/* Function Id: 8 */

#if defined(MBEDTLS_CTR_DRBG_C) && defined(MBEDTLS_SELF_TEST)
    test_ctr_drbg_selftest_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_ctr_drbg.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
