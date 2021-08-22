/* clang-format off */
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
 * Test file      : ./test_suite_ecjpake.c
 *
 * The following files were used to create this file.
 *
 *      Main code file      : suites/main_test.function
 *      Platform code file  : suites/host_test.function
 *      Helper file         : suites/helpers.function
 *      Test suite file     : suites/test_suite_ecjpake.function
 *      Test suite data     : suites/test_suite_ecjpake.data
 *
 */


#define TEST_SUITE_ACTIVE

#if defined(MBEDTLS_ECJPAKE_C)

#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) && defined(MBEDTLS_SHA256_C)
static const unsigned char ecjpake_test_x1[] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
    0x0d, 0x0e, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
    0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x21
};

static const unsigned char ecjpake_test_x2[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c,
    0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x81
};

static const unsigned char ecjpake_test_x3[] = {
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c,
    0x6d, 0x6e, 0x6f, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 0x81
};

static const unsigned char ecjpake_test_x4[] = {
    0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcc,
    0xcd, 0xce, 0xcf, 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8,
    0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, 0xe1
};

static const unsigned char ecjpake_test_X1[] = {
    0x04, 0xac, 0xcf, 0x01, 0x06, 0xef, 0x85, 0x8f, 0xa2, 0xd9, 0x19, 0x33,
    0x13, 0x46, 0x80, 0x5a, 0x78, 0xb5, 0x8b, 0xba, 0xd0, 0xb8, 0x44, 0xe5,
    0xc7, 0x89, 0x28, 0x79, 0x14, 0x61, 0x87, 0xdd, 0x26, 0x66, 0xad, 0xa7,
    0x81, 0xbb, 0x7f, 0x11, 0x13, 0x72, 0x25, 0x1a, 0x89, 0x10, 0x62, 0x1f,
    0x63, 0x4d, 0xf1, 0x28, 0xac, 0x48, 0xe3, 0x81, 0xfd, 0x6e, 0xf9, 0x06,
    0x07, 0x31, 0xf6, 0x94, 0xa4
};

static const unsigned char ecjpake_test_X2[] = {
    0x04, 0x7e, 0xa6, 0xe3, 0xa4, 0x48, 0x70, 0x37, 0xa9, 0xe0, 0xdb, 0xd7,
    0x92, 0x62, 0xb2, 0xcc, 0x27, 0x3e, 0x77, 0x99, 0x30, 0xfc, 0x18, 0x40,
    0x9a, 0xc5, 0x36, 0x1c, 0x5f, 0xe6, 0x69, 0xd7, 0x02, 0xe1, 0x47, 0x79,
    0x0a, 0xeb, 0x4c, 0xe7, 0xfd, 0x65, 0x75, 0xab, 0x0f, 0x6c, 0x7f, 0xd1,
    0xc3, 0x35, 0x93, 0x9a, 0xa8, 0x63, 0xba, 0x37, 0xec, 0x91, 0xb7, 0xe3,
    0x2b, 0xb0, 0x13, 0xbb, 0x2b
};

static const unsigned char ecjpake_test_X3[] = {
    0x04, 0x7e, 0xa6, 0xe3, 0xa4, 0x48, 0x70, 0x37, 0xa9, 0xe0, 0xdb, 0xd7,
    0x92, 0x62, 0xb2, 0xcc, 0x27, 0x3e, 0x77, 0x99, 0x30, 0xfc, 0x18, 0x40,
    0x9a, 0xc5, 0x36, 0x1c, 0x5f, 0xe6, 0x69, 0xd7, 0x02, 0xe1, 0x47, 0x79,
    0x0a, 0xeb, 0x4c, 0xe7, 0xfd, 0x65, 0x75, 0xab, 0x0f, 0x6c, 0x7f, 0xd1,
    0xc3, 0x35, 0x93, 0x9a, 0xa8, 0x63, 0xba, 0x37, 0xec, 0x91, 0xb7, 0xe3,
    0x2b, 0xb0, 0x13, 0xbb, 0x2b
};

static const unsigned char ecjpake_test_X4[] = {
    0x04, 0x19, 0x0a, 0x07, 0x70, 0x0f, 0xfa, 0x4b, 0xe6, 0xae, 0x1d, 0x79,
    0xee, 0x0f, 0x06, 0xae, 0xb5, 0x44, 0xcd, 0x5a, 0xdd, 0xaa, 0xbe, 0xdf,
    0x70, 0xf8, 0x62, 0x33, 0x21, 0x33, 0x2c, 0x54, 0xf3, 0x55, 0xf0, 0xfb,
    0xfe, 0xc7, 0x83, 0xed, 0x35, 0x9e, 0x5d, 0x0b, 0xf7, 0x37, 0x7a, 0x0f,
    0xc4, 0xea, 0x7a, 0xce, 0x47, 0x3c, 0x9c, 0x11, 0x2b, 0x41, 0xcc, 0xd4,
    0x1a, 0xc5, 0x6a, 0x56, 0x12
};

/* Load my private and public keys, and peer's public keys */
static int ecjpake_test_load( mbedtls_ecjpake_context *ctx,
                              const unsigned char *xm1, size_t len_xm1,
                              const unsigned char *xm2, size_t len_xm2,
                              const unsigned char *Xm1, size_t len_Xm1,
                              const unsigned char *Xm2, size_t len_Xm2,
                              const unsigned char *Xp1, size_t len_Xp1,
                              const unsigned char *Xp2, size_t len_Xp2 )
{
    int ret;

    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->xm1, xm1, len_xm1 ) );
    MBEDTLS_MPI_CHK( mbedtls_mpi_read_binary( &ctx->xm2, xm2, len_xm2 ) );

    MBEDTLS_MPI_CHK( mbedtls_ecp_point_read_binary( &ctx->grp,
                     &ctx->Xm1, Xm1, len_Xm1 ) );
    MBEDTLS_MPI_CHK( mbedtls_ecp_point_read_binary( &ctx->grp,
                     &ctx->Xm2, Xm2, len_Xm2 ) );
    MBEDTLS_MPI_CHK( mbedtls_ecp_point_read_binary( &ctx->grp,
                     &ctx->Xp1, Xp1, len_Xp1 ) );
    MBEDTLS_MPI_CHK( mbedtls_ecp_point_read_binary( &ctx->grp,
                     &ctx->Xp2, Xp2, len_Xp2 ) );

cleanup:
    return( ret );
}

#define ADD_SIZE( x )   x, sizeof( x )
#endif /* MBEDTLS_ECP_DP_SECP256R1_ENABLED && MBEDTLS_SHA256_C */
#if defined(MBEDTLS_CHECK_PARAMS)
#if !defined(MBEDTLS_PARAM_FAILED_ALT)
void test_ecjpake_invalid_param( )
{
    mbedtls_ecjpake_context ctx;
    unsigned char buf[42] = { 0 };
    size_t olen;
    size_t const len = sizeof( buf );
    mbedtls_ecjpake_role valid_role = MBEDTLS_ECJPAKE_SERVER;
    mbedtls_ecjpake_role invalid_role = (mbedtls_ecjpake_role) 42;
    mbedtls_md_type_t valid_md = MBEDTLS_MD_SHA256;
    mbedtls_ecp_group_id valid_group = MBEDTLS_ECP_DP_SECP256R1;

    TEST_INVALID_PARAM( mbedtls_ecjpake_init( NULL ) );
    TEST_VALID_PARAM( mbedtls_ecjpake_free( NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecjpake_setup( NULL,
                                                   valid_role,
                                                   valid_md,
                                                   valid_group,
                                                   buf, len ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecjpake_setup( &ctx,
                                                   invalid_role,
                                                   valid_md,
                                                   valid_group,
                                                   buf, len ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecjpake_setup( &ctx,
                                                   valid_role,
                                                   valid_md,
                                                   valid_group,
                                                   NULL, len ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecjpake_check( NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
        mbedtls_ecjpake_write_round_one( NULL, buf, len, &olen,
                                         mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
        mbedtls_ecjpake_write_round_one( &ctx, NULL, len, &olen,
                                         mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
        mbedtls_ecjpake_write_round_one( &ctx, buf, len, NULL,
                                         mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
        mbedtls_ecjpake_write_round_one( &ctx, buf, len, &olen, NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
        mbedtls_ecjpake_write_round_two( NULL, buf, len, &olen,
                                         mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
        mbedtls_ecjpake_write_round_two( &ctx, NULL, len, &olen,
                                         mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
        mbedtls_ecjpake_write_round_two( &ctx, buf, len, NULL,
                                         mbedtls_test_rnd_std_rand, NULL ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
        mbedtls_ecjpake_write_round_two( &ctx, buf, len, &olen, NULL, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecjpake_read_round_one( NULL,
                                                            buf, len ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecjpake_read_round_one( &ctx,
                                                            NULL, len ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecjpake_read_round_two( NULL,
                                                            buf, len ) );
    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
                            mbedtls_ecjpake_read_round_two( &ctx,
                                                            NULL, len ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
        mbedtls_ecjpake_derive_secret( NULL, buf, len, &olen,
                                       mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
        mbedtls_ecjpake_derive_secret( &ctx, NULL, len, &olen,
                                       mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
        mbedtls_ecjpake_derive_secret( &ctx, buf, len, NULL,
                                       mbedtls_test_rnd_std_rand, NULL ) );

    TEST_INVALID_PARAM_RET( MBEDTLS_ERR_ECP_BAD_INPUT_DATA,
        mbedtls_ecjpake_derive_secret( &ctx, buf, len, &olen, NULL, NULL ) );

exit:
    return;
}

void test_ecjpake_invalid_param_wrapper( void ** params )
{
    (void)params;

    test_ecjpake_invalid_param(  );
}
#endif /* !MBEDTLS_PARAM_FAILED_ALT */
#endif /* MBEDTLS_CHECK_PARAMS */
#if defined(MBEDTLS_SELF_TEST)
void test_ecjpake_selftest(  )
{
    TEST_ASSERT( mbedtls_ecjpake_self_test( 1 ) == 0 );
exit:
    ;
}

void test_ecjpake_selftest_wrapper( void ** params )
{
    (void)params;

    test_ecjpake_selftest(  );
}
#endif /* MBEDTLS_SELF_TEST */
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
#if defined(MBEDTLS_SHA256_C)
void test_read_bad_md( data_t *msg )
{
    mbedtls_ecjpake_context corrupt_ctx;
    const unsigned char * pw = NULL;
    const size_t pw_len = 0;
    int any_role = MBEDTLS_ECJPAKE_CLIENT;

    mbedtls_ecjpake_init( &corrupt_ctx );
    TEST_ASSERT( mbedtls_ecjpake_setup( &corrupt_ctx, any_role,
                 MBEDTLS_MD_SHA256, MBEDTLS_ECP_DP_SECP256R1, pw, pw_len ) == 0 );
    corrupt_ctx.md_info = NULL;

    TEST_ASSERT( mbedtls_ecjpake_read_round_one( &corrupt_ctx, msg->x,
                 msg->len ) == MBEDTLS_ERR_MD_BAD_INPUT_DATA );

exit:
    mbedtls_ecjpake_free( &corrupt_ctx );
}

void test_read_bad_md_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_read_bad_md( &data0 );
}
#endif /* MBEDTLS_SHA256_C */
#endif /* MBEDTLS_ECP_DP_SECP256R1_ENABLED */
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
#if defined(MBEDTLS_SHA256_C)
void test_read_round_one( int role, data_t * msg, int ref_ret )
{
    mbedtls_ecjpake_context ctx;
    const unsigned char * pw = NULL;
    const size_t pw_len = 0;

    mbedtls_ecjpake_init( &ctx );

    TEST_ASSERT( mbedtls_ecjpake_setup( &ctx, role,
                 MBEDTLS_MD_SHA256, MBEDTLS_ECP_DP_SECP256R1, pw, pw_len ) == 0 );

    TEST_ASSERT( mbedtls_ecjpake_read_round_one( &ctx, msg->x, msg->len ) == ref_ret );

exit:
    mbedtls_ecjpake_free( &ctx );
}

void test_read_round_one_wrapper( void ** params )
{
    data_t data1 = {(uint8_t *) params[1], *( (uint32_t *) params[2] )};

    test_read_round_one( *( (int *) params[0] ), &data1, *( (int *) params[3] ) );
}
#endif /* MBEDTLS_SHA256_C */
#endif /* MBEDTLS_ECP_DP_SECP256R1_ENABLED */
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
#if defined(MBEDTLS_SHA256_C)
void test_read_round_two_cli( data_t * msg, int ref_ret )
{
    mbedtls_ecjpake_context ctx;
    const unsigned char * pw = NULL;
    const size_t pw_len = 0;

    mbedtls_ecjpake_init( &ctx );

    TEST_ASSERT( mbedtls_ecjpake_setup( &ctx, MBEDTLS_ECJPAKE_CLIENT,
                 MBEDTLS_MD_SHA256, MBEDTLS_ECP_DP_SECP256R1, pw, pw_len ) == 0 );

    TEST_ASSERT( ecjpake_test_load( &ctx,
                 ADD_SIZE( ecjpake_test_x1 ), ADD_SIZE( ecjpake_test_x2 ),
                 ADD_SIZE( ecjpake_test_X1 ), ADD_SIZE( ecjpake_test_X2 ),
                 ADD_SIZE( ecjpake_test_X3 ), ADD_SIZE( ecjpake_test_X4 ) )
            == 0 );

    TEST_ASSERT( mbedtls_ecjpake_read_round_two( &ctx, msg->x, msg->len ) == ref_ret );

exit:
    mbedtls_ecjpake_free( &ctx );
}

void test_read_round_two_cli_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_read_round_two_cli( &data0, *( (int *) params[2] ) );
}
#endif /* MBEDTLS_SHA256_C */
#endif /* MBEDTLS_ECP_DP_SECP256R1_ENABLED */
#if defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED)
#if defined(MBEDTLS_SHA256_C)
void test_read_round_two_srv( data_t * msg, int ref_ret )
{
    mbedtls_ecjpake_context ctx;
    const unsigned char * pw = NULL;
    const size_t pw_len = 0;

    mbedtls_ecjpake_init( &ctx );

    TEST_ASSERT( mbedtls_ecjpake_setup( &ctx, MBEDTLS_ECJPAKE_SERVER,
                 MBEDTLS_MD_SHA256, MBEDTLS_ECP_DP_SECP256R1, pw, pw_len ) == 0 );

    TEST_ASSERT( ecjpake_test_load( &ctx,
                 ADD_SIZE( ecjpake_test_x3 ), ADD_SIZE( ecjpake_test_x4 ),
                 ADD_SIZE( ecjpake_test_X3 ), ADD_SIZE( ecjpake_test_X4 ),
                 ADD_SIZE( ecjpake_test_X1 ), ADD_SIZE( ecjpake_test_X2 ) )
            == 0 );

    TEST_ASSERT( mbedtls_ecjpake_read_round_two( &ctx, msg->x, msg->len ) == ref_ret );

exit:
    mbedtls_ecjpake_free( &ctx );
}

void test_read_round_two_srv_wrapper( void ** params )
{
    data_t data0 = {(uint8_t *) params[0], *( (uint32_t *) params[1] )};

    test_read_round_two_srv( &data0, *( (int *) params[2] ) );
}
#endif /* MBEDTLS_SHA256_C */
#endif /* MBEDTLS_ECP_DP_SECP256R1_ENABLED */
#endif /* MBEDTLS_ECJPAKE_C */

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

#if defined(MBEDTLS_ECJPAKE_C)

        case 0:
            {
                *out_value = MBEDTLS_ECJPAKE_CLIENT;
            }
            break;
        case 1:
            {
                *out_value = MBEDTLS_ECJPAKE_SERVER;
            }
            break;
        case 2:
            {
                *out_value = MBEDTLS_ERR_ECP_VERIFY_FAILED;
            }
            break;
        case 3:
            {
                *out_value = MBEDTLS_ERR_ECP_BAD_INPUT_DATA;
            }
            break;
        case 4:
            {
                *out_value = MBEDTLS_ERR_ECP_INVALID_KEY;
            }
            break;
        case 5:
            {
                *out_value = MBEDTLS_ERR_ECP_FEATURE_UNAVAILABLE;
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

#if defined(MBEDTLS_ECJPAKE_C)

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

#if defined(MBEDTLS_ECJPAKE_C) && defined(MBEDTLS_CHECK_PARAMS) && !defined(MBEDTLS_PARAM_FAILED_ALT)
    test_ecjpake_invalid_param_wrapper,
#else
    NULL,
#endif
/* Function Id: 1 */

#if defined(MBEDTLS_ECJPAKE_C) && defined(MBEDTLS_SELF_TEST)
    test_ecjpake_selftest_wrapper,
#else
    NULL,
#endif
/* Function Id: 2 */

#if defined(MBEDTLS_ECJPAKE_C) && defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) && defined(MBEDTLS_SHA256_C)
    test_read_bad_md_wrapper,
#else
    NULL,
#endif
/* Function Id: 3 */

#if defined(MBEDTLS_ECJPAKE_C) && defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) && defined(MBEDTLS_SHA256_C)
    test_read_round_one_wrapper,
#else
    NULL,
#endif
/* Function Id: 4 */

#if defined(MBEDTLS_ECJPAKE_C) && defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) && defined(MBEDTLS_SHA256_C)
    test_read_round_two_cli_wrapper,
#else
    NULL,
#endif
/* Function Id: 5 */

#if defined(MBEDTLS_ECJPAKE_C) && defined(MBEDTLS_ECP_DP_SECP256R1_ENABLED) && defined(MBEDTLS_SHA256_C)
    test_read_round_two_srv_wrapper,
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
    ret = execute_tests( argc, argv, "/zip/third_party/mbedtls/test/test_suite_ecjpake.datax" );
    mbedtls_test_platform_teardown();
    return( ret );
}
